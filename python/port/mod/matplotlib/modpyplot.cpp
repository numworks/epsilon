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
  static Matplotlib::PlotController plotController(&plotStore);
  sPlotStore = &plotStore;
  sPlotController = &plotController;
  sPlotStore->flush();
  paletteIndex = 0;
  return mp_const_none;
}

void modpyplot_gc_collect() {
  if (sPlotStore == nullptr) {
    return;
  }
  MicroPython::collectRootsAtAddress(
    reinterpret_cast<char *>(sPlotStore),
    sizeof(Matplotlib::PlotStore)
  );
}

mp_obj_t modpyplot_arrow(size_t n_args, const mp_obj_t *args) {
  assert(n_args == 4);
  assert(sPlotStore != nullptr);

  KDColor color = Palette::nextDataColor(&paletteIndex);
  sPlotStore->addSegment(args[0], args[1], mp_obj_new_float(mp_obj_get_float(args[0])+mp_obj_get_float(args[2])), mp_obj_new_float(mp_obj_get_float(args[1])+mp_obj_get_float(args[3])), color, true);

  return mp_const_none;
}

/* axis(arg)
 *  - arg = "on", "off", "auto"
 *  - arg = True, False
 *  - arg = [xmin, xmax, ymin, ymax], (xmin, xmax, ymin, ymax)
 * Returns : (xmin, xmax, ymin, ymax) : float */

mp_obj_t modpyplot_axis(size_t n_args, const mp_obj_t *args) {
  assert(sPlotStore != nullptr);

  if (n_args == 1) {
    mp_obj_t arg = args[0];
    if (mp_obj_is_str(arg)) {
      if (mp_obj_str_equal(arg, mp_obj_new_str("on", 2))) {
        sPlotStore->setAxesRequested(true);
      } else if (mp_obj_str_equal(arg, mp_obj_new_str("off", 3))) {
        sPlotStore->setAxesRequested(false);
      } else if (mp_obj_str_equal(arg, mp_obj_new_str("auto", 4))) {
        sPlotStore->setAxesRequested(true);
        sPlotStore->setAxesAuto(true);
      } else {
        mp_raise_ValueError("Unrecognized string given to axis; try 'on', 'off' or 'auto'");
      }
#warning Use mp_obj_is_bool when upgrading uPy
    } else if (mp_obj_is_type(arg, &mp_type_bool)) {
      sPlotStore->setAxesRequested(mp_obj_is_true(arg));
    } else if (mp_obj_is_type(arg, &mp_type_tuple) || mp_obj_is_type(arg, &mp_type_list)) {
      mp_obj_t * items;
      mp_obj_get_array_fixed_n(arg, 4, &items);
      sPlotStore->setXMin(mp_obj_get_float(items[0]));
      sPlotStore->setXMax(mp_obj_get_float(items[1]));
      sPlotStore->setYMin(mp_obj_get_float(items[2]));
      sPlotStore->setYMax(mp_obj_get_float(items[3]));
      sPlotStore->setAxesAuto(false);
    } else {
      mp_raise_TypeError("the first argument to axis() must be an interable of the form [xmin, xmax, ymin, ymax]");
    }
  }

  // Build the return value
  mp_obj_t coords[4];
  coords[0] = mp_obj_new_float(sPlotStore->xMin());
  coords[1] = mp_obj_new_float(sPlotStore->xMax());
  coords[2] = mp_obj_new_float(sPlotStore->yMin());
  coords[3] = mp_obj_new_float(sPlotStore->yMax());
  return mp_obj_new_tuple(4, coords);
}

/* bar(x, height, width, bottom)
 * 'height', 'width' and 'bottom' can either be a scalar or an array/tuple of
 * scalar.
 * 'width' default value is 0.8
 * 'bottom' default value is None
 * */

// TODO: accept keyword args?

void extract_argument(mp_obj_t arg, size_t length, mp_obj_t ** items, float * item) {
  if (mp_obj_is_type(arg, &mp_type_tuple) || mp_obj_is_type(arg, &mp_type_list)) {
    size_t itemLength;
    mp_obj_get_array(arg, &itemLength, items);
    if (itemLength != length) {
      mp_raise_ValueError("Shape mismatch");
    }
  } else {
    *item = mp_obj_get_float(arg);
  }
}

mp_obj_t modpyplot_bar(size_t n_args, const mp_obj_t *args) {
  assert(sPlotStore != nullptr);

  mp_obj_t * xItems;
  mp_obj_t * hItems = nullptr;
  float h;
  mp_obj_t * wItems = nullptr;
  float w = 0.8f;
  mp_obj_t * bItems = nullptr;
  float b = 0.0f;

  // x arg
  size_t xLength;
  mp_obj_get_array(args[0], &xLength, &xItems);

  // height arg
  extract_argument(args[1], xLength, &hItems, &h);

  // width arg
  if (n_args >= 3) {
    extract_argument(args[2], xLength, &wItems, &w);
  }

  // bottom arg
  if (n_args >= 4) {
    extract_argument(args[3], xLength, &bItems, &b);
  }

  KDColor color = Palette::nextDataColor(&paletteIndex);
  for (size_t i=0; i<xLength; i++) {
    mp_float_t iH = hItems ? mp_obj_get_float(hItems[i]) : h;
    mp_float_t iW = wItems ? mp_obj_get_float(wItems[i]) : w;
    mp_float_t iB = bItems ? mp_obj_get_float(bItems[i]) : b;
    mp_float_t iX = mp_obj_get_float(xItems[i])-iW/2.0;
    mp_float_t iY = iH < 0.0 ? iB : iB + iH;
    sPlotStore->addRect(mp_obj_new_float(iX), mp_obj_new_float(iY), mp_obj_new_float(iW), mp_obj_new_float(std::fabs(iH)), color);
  }
  return mp_const_none;
}

mp_obj_t modpyplot_grid(size_t n_args, const mp_obj_t *args) {
  assert(sPlotStore != nullptr);

  if (n_args == 0) {
    // Toggle the grid visibility
    sPlotStore->setGridRequested(!sPlotStore->gridRequested());
  } else {
    sPlotStore->setGridRequested(mp_obj_is_true(args[0]));
  }
  return mp_const_none;
}

/* hist(x, bins)
 * 'x' array
 * 'bins': (default value 10)
 *    - int (number of bins)
 *    - sequence of bins
 * */

mp_obj_t modpyplot_hist(size_t n_args, const mp_obj_t *args) {
  assert(sPlotStore != nullptr);

  // Sort data to easily get the minimal and maximal value and count bin sizes
  mp_obj_t * xItems;
  size_t xLength;
  mp_obj_get_array(args[0], &xLength, &xItems);
  mp_obj_t xList = mp_obj_new_list(xLength, xItems);
  mp_obj_list_sort(1, &xList, (mp_map_t*)&mp_const_empty_map);
  mp_obj_list_get(xList, &xLength, &xItems);
  mp_float_t min = mp_obj_get_float(xItems[0]);
  mp_float_t max = mp_obj_get_float(xItems[xLength - 1]);

  mp_obj_t binsEdges;
  size_t nBins = 10;
  // bin arg
  if (n_args >= 2 && (mp_obj_is_type(args[1], &mp_type_tuple) || mp_obj_is_type(args[1], &mp_type_list))) {
    binsEdges = args[1];
  } else {
    if (n_args >= 2) {
      nBins = mp_obj_get_int(args[1]);
    }
    // Create a list of bins
    binsEdges = mp_obj_new_list(nBins+1, nullptr);

    // Fill the bin edges list
    mp_float_t binWidth = (max-min)/nBins;
    for (int i = 0; i < nBins+1; i++) {
      mp_obj_list_store(binsEdges, mp_obj_new_int(i), mp_obj_new_float(min+i*binWidth));
    }
  }
  mp_obj_t * edgeItems;
  size_t nEdges;
  mp_obj_list_get(binsEdges, &nEdges, &edgeItems);
  nBins = nEdges - 1;

  // Initialize bins list
  mp_obj_t bins = mp_obj_new_list(nBins, nullptr);
  for (size_t i=0; i<nBins; i++) {
    mp_obj_list_store(bins, mp_obj_new_int(i), mp_obj_new_int(0));
  }

  mp_obj_t * binItems;
  mp_obj_list_get(bins, &nBins, &binItems);

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

  KDColor color = Palette::nextDataColor(&paletteIndex);
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

  KDColor color = Palette::nextDataColor(&paletteIndex);
  for (size_t i=0; i<length; i++) {
    sPlotStore->addDot(xItems[i], yItems[i], color);
  }

  return mp_const_none;
}

/* plot(x, y) plots the curve (x, y)
 * plot(y) plots the curve x as index array ([0,1,2...],y)
 * */

mp_obj_t modpyplot_plot(size_t n_args, const mp_obj_t *args) {
  assert(sPlotStore != nullptr);

  mp_obj_t * xItems, * yItems;
  size_t length;
  if (n_args == 1) {
    mp_obj_get_array(args[0], &length, &yItems);

    // Create the default xItems: [0, 1, 2,...]
    mp_obj_t x = mp_obj_new_list(length, nullptr);
    for (int i = 0; i < length; i++) {
      mp_obj_list_store(x, mp_obj_new_int(i), mp_obj_new_float((float)i));
    }
    mp_obj_get_array(x, &length, &xItems);
  } else {
    assert(n_args == 2);
    length = extractAndValidatePlotInput(args[0], args[1], &xItems, &yItems);
  }

  KDColor color = Palette::nextDataColor(&paletteIndex);
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
