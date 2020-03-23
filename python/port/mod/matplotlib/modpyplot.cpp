extern "C" {
#include "modpyplot.h"
}
#include <assert.h>
#include <escher/palette.h>
#include "port.h"
#include "plot_controller.h"

Matplotlib::PlotStore * sPlotStore = nullptr;
Matplotlib::PlotController * sPlotController = nullptr;
static int paletteIndex = 0;

// Private helper


static size_t extractAndValidatePlotInput(mp_obj_t x, mp_obj_t y, mp_obj_t ** xItems, mp_obj_t ** yItems) {
  // Input parameter validation
  size_t xLength, yLength;
  mp_obj_get_array(x, &xLength, xItems);
  mp_obj_get_array(y, &yLength, yItems);
  if (xLength != yLength) {
    mp_raise_ValueError("x and y must have same dimension");
  }
  return xLength;
}

// Internal functions

mp_obj_t modpyplot___init__() {
  static Matplotlib::PlotStore plotStore;
  static Matplotlib::PlotController plotController(&plotStore, MicroPython::ExecutionEnvironment::currentExecutionEnvironment());
  sPlotStore = &plotStore;
  sPlotController = &plotController;
  sPlotStore->flush();
  paletteIndex = 0;
  return mp_const_none;
}

void modpyplot_gc_collect() {
  if (sPlotStore != nullptr) {
    return;
  }
  MicroPython::collectRootsAtAddress(
    reinterpret_cast<char *>(&sPlotStore),
    sizeof(Matplotlib::PlotStore)
  );
}

mp_obj_t modpyplot_arrow(size_t n_args, const mp_obj_t *args) {
  assert(n_args == 4);
  assert(sPlotStore != nullptr);

  KDColor color = Palette::DataColor[paletteIndex++]; // FIXME: Share overflow routine
  sPlotStore->addSegment(args[0], args[1], mp_obj_new_float(mp_obj_get_float(args[0])+mp_obj_get_float(args[2])), mp_obj_new_float(mp_obj_get_float(args[1])+mp_obj_get_float(args[3])), color, true);

  return mp_const_none;
}

/* axis(arg)
 *  - arg = [xmin, xmax, ymin, ymax]
 *  - arg = True, False
 * Returns : xmin, xmax, ymin, ymax : float */

mp_obj_t modpyplot_axis(mp_obj_t arg) {
  assert(sPlotStore != nullptr);

#warning Use mp_obj_is_bool when upgrading uPy
  if (mp_obj_is_type(arg, &mp_type_bool)) {
    sPlotStore->setAxesRequested(mp_obj_is_true(arg));
  } else {
    mp_obj_t * items;
    mp_obj_get_array_fixed_n(arg, 4, &items);
    sPlotStore->setXMin(mp_obj_get_float(items[0]));
    sPlotStore->setXMax(mp_obj_get_float(items[1]));
    sPlotStore->setYMin(mp_obj_get_float(items[2]));
    sPlotStore->setYMax(mp_obj_get_float(items[3]));
  }

  // Build the return value
  mp_obj_t coords[4];
  coords[0] = mp_obj_new_float(sPlotStore->xMin());
  coords[1] = mp_obj_new_float(sPlotStore->xMax());
  coords[2] = mp_obj_new_float(sPlotStore->yMin());
  coords[3] = mp_obj_new_float(sPlotStore->yMax());
  return mp_obj_new_tuple(4, coords);
}

mp_obj_t modpyplot_bar(mp_obj_t x, mp_obj_t height) {
  assert(sPlotStore != nullptr);

  // Input parameter validation
  mp_obj_t * xItems, * hItems;
  size_t length = extractAndValidatePlotInput(x, height, &xItems, &hItems);
  mp_float_t w = 0.8; // TODO: w should be an optional parameter

  KDColor color = Palette::DataColor[paletteIndex++]; // FIXME: Share overflow routine
  for (size_t i=0; i<length; i++) {
    mp_float_t rectX = mp_obj_get_float(xItems[i])-w/2.0;
    mp_float_t h = mp_obj_get_float(hItems[i]);
    mp_float_t rectY = h < 0.0 ? 0.0 : h;
    sPlotStore->addRect(mp_obj_new_float(rectX), mp_obj_new_float(rectY), mp_obj_new_float(w), mp_obj_new_float(std::fabs(h)), color);
  }
  return mp_const_none;
}

mp_obj_t modpyplot_grid(mp_obj_t b) {
  if (mp_obj_is_type(b, &mp_type_bool)) {
    sPlotStore->setGridRequested(mp_obj_is_true(b));
  } else {
    sPlotStore->setGridRequested(!sPlotStore->gridRequested());
  }
  return mp_const_none;
}

// Dichotomia to find which bin a value belongs to
size_t belongs_to_bin_of_index(mp_float_t v, mp_obj_t * binsEdges, size_t minIndex, size_t maxIndex) {
  assert(mp_obj_get_float(binsEdges[minIndex]) <= v && v <= mp_obj_get_float(binsEdges[maxIndex]));
  if (maxIndex - minIndex < 2) {
    return minIndex;
  }
  size_t index = (minIndex+maxIndex)/2;
  mp_float_t pivot = mp_obj_get_float(binsEdges[index]);
  if (pivot == v) {
    return index;
  } else if (pivot < v) {
    return belongs_to_bin_of_index(v, binsEdges, index, maxIndex);
  } else {
    assert(pivot > v);
    return belongs_to_bin_of_index(v, binsEdges, minIndex, index);
  }
}

mp_obj_t modpyplot_hist(mp_obj_t x) {
  // Create a list of bins
  // TODO: the number of bins can be given as input
  size_t nBins = 10;
  // TODO: the list of bins can be given as input
  // TODO: skip the following computation if so,
  // `bins` must increase monotonically, when an array'
  mp_obj_t binsEdges = mp_obj_new_list(nBins+1, nullptr);

  // Sort data to easily get the minimal and maximal value and count bin sizes
  mp_obj_t * xItems;
  size_t xLength;
  mp_obj_get_array(x, &xLength, &xItems);
  mp_obj_t xList = mp_obj_new_list(xLength, xItems);
  mp_obj_list_sort(1, &xList, (mp_map_t*)&mp_const_empty_map);
  mp_obj_list_get(xList, &xLength, &xItems);
  mp_float_t min = mp_obj_get_float(xItems[0]);
  mp_float_t max = mp_obj_get_float(xItems[xLength - 1]);

  // Fill the bin edges list
  // TODO: skip if the binEdges were given as input
  mp_float_t binWidth = (max-min)/nBins;
  for (int i = 0; i < nBins+1; i++) {
    mp_obj_list_store(binsEdges, mp_obj_new_int(i), mp_obj_new_float(min+i*binWidth));
  }

  // Initialize bins list
  mp_obj_t bins = mp_obj_new_list(nBins, nullptr);
  for (size_t i=0; i<nBins; i++) {
    mp_obj_list_store(bins, mp_obj_new_int(i), mp_obj_new_int(0));
  }

  mp_obj_t * binItems;
  mp_obj_list_get(bins, &nBins, &binItems);

  mp_obj_t * edgeItems;
  size_t nEdges;
  mp_obj_list_get(binsEdges, &nEdges, &edgeItems);
  assert(nEdges == nBins + 1);

  // Fill bins list by linearly scanning the x and incrementing the bin count
  // Linearity is enabled thanks to sorting
  size_t binIndex = 0;
  size_t xIndex = 0;
  while (xIndex < xLength) {
    assert(binIndex < nBins);
    mp_float_t upperBound = mp_obj_get_float(edgeItems[binIndex+1]);
    while (mp_obj_get_float(xItems[xIndex]) < upperBound || (binIndex == nBins - 1 && mp_obj_get_float(xItems[xIndex]) == upperBound)) {
      // Increment the bin count
      binItems[binIndex] = mp_obj_new_int(mp_obj_get_int(binItems[binIndex]) + 1);
      xIndex++;
      if (xIndex == xLength) {
        break;
      }
    }
    binIndex++;
  }

  KDColor color = Palette::DataColor[paletteIndex++]; // FIXME: Share overflow routine
  for (size_t i=0; i<nBins; i++) {
    mp_float_t width = mp_obj_get_float(edgeItems[i+1]) - mp_obj_get_float(edgeItems[i]);
    sPlotStore->addRect(edgeItems[i], binItems[i], mp_obj_new_float(width), binItems[i], color);
  }
  return mp_const_none;
}

mp_obj_t modpyplot_scatter(mp_obj_t x, mp_obj_t y) {
  assert(sPlotStore != nullptr);

  // Input parameter validation
  mp_obj_t * xItems, * yItems;
  size_t length = extractAndValidatePlotInput(x, y, &xItems, &yItems);

  KDColor color = Palette::DataColor[paletteIndex++]; // FIXME: Share overflow routine
  for (size_t i=0; i<length; i++) {
    sPlotStore->addDot(xItems[i], yItems[i], color);
  }

  return mp_const_none;
}

mp_obj_t modpyplot_plot(mp_obj_t x, mp_obj_t y) {
  assert(sPlotStore != nullptr);

  // Input parameter validation
  mp_obj_t * xItems, * yItems;
  size_t length = extractAndValidatePlotInput(x, y, &xItems, &yItems);

  KDColor color = Palette::DataColor[paletteIndex++]; // FIXME: Share overflow routine
  for (size_t i=0; i<length-1; i++) {
    sPlotStore->addSegment(xItems[i], yItems[i], xItems[i+1], yItems[i+1], color, false);
  }

  return mp_const_none;
}

mp_obj_t modpyplot_text(mp_obj_t x, mp_obj_t y, mp_obj_t s) {
  assert(sPlotStore != nullptr);

  // Input parameter validation
  mp_obj_get_float(x);
  mp_obj_get_float(y);
  mp_obj_str_get_str(s);

  sPlotStore->addLabel(x, y, s);

  return mp_const_none;
}

mp_obj_t modpyplot_show() {
  MicroPython::ExecutionEnvironment * env = MicroPython::ExecutionEnvironment::currentExecutionEnvironment();
  env->displayViewController(sPlotController);
  return mp_const_none;
}
