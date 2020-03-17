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


static size_t extract_and_validate_plot_input(mp_obj_t x, mp_obj_t y, mp_obj_t ** xItems, mp_obj_t ** yItems) {
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
  if (sPlotStore != nullptr) {
    return;
  }
  MicroPython::collectRootsAtAddress(
    reinterpret_cast<char *>(&sPlotStore),
    sizeof(Matplotlib::PlotStore)
  );
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
  size_t length = extract_and_validate_plot_input(x, height, &xItems, &hItems);
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

mp_obj_t modpyplot_scatter(mp_obj_t x, mp_obj_t y) {
  assert(sPlotStore != nullptr);

  // Input parameter validation
  mp_obj_t * xItems, * yItems;
  size_t length = extract_and_validate_plot_input(x, y, &xItems, &yItems);

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
  size_t length = extract_and_validate_plot_input(x, y, &xItems, &yItems);

  KDColor color = Palette::DataColor[paletteIndex++]; // FIXME: Share overflow routine
  for (size_t i=0; i<length-1; i++) {
    sPlotStore->addSegment(xItems[i], yItems[i], xItems[i+1], yItems[i+1], color);
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
