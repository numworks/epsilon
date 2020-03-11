extern "C" {
#include "modpyplot.h"
}
#include <assert.h>
#include <escher/palette.h>
#include "port.h"
#include "plot_controller.h"

Matplotlib::PlotStore * sPlotStore = nullptr;
Matplotlib::PlotController * sPlotController = nullptr;
static int paletteIndex = 0; // FIXME: Needs to be reset at some point

// Internal functions

mp_obj_t modpyplot___init__() {
  static Matplotlib::PlotStore plotStore;
  static Matplotlib::PlotController plotController(&plotStore);
  sPlotStore = &plotStore;
  sPlotController = &plotController;
  sPlotStore->flush();
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
    sPlotStore->setGrid(mp_obj_is_true(arg));
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

mp_obj_t modpyplot_scatter(mp_obj_t x, mp_obj_t y) {
  assert(sPlotStore != nullptr);

  // Input parameter validation
  size_t xLength, yLength;
  mp_obj_t * xItems, * yItems;
  mp_obj_get_array(x, &xLength, &xItems);
  mp_obj_get_array(y, &yLength, &yItems);
  if (xLength != yLength) {
    mp_raise_ValueError("x and y must have same dimension");
  }

  KDColor color = Palette::DataColor[paletteIndex++]; // FIXME: Share overflow routine
  for (size_t i=0; i<xLength; i++) {
    sPlotStore->addDot(xItems[i], yItems[i], color);
  }

  return mp_const_none;
}

mp_obj_t modpyplot_plot(mp_obj_t x, mp_obj_t y) {
  assert(sPlotStore != nullptr);
  assert(sPlotStore != nullptr);

  // Input parameter validation
  size_t xLength, yLength;
  mp_obj_t * xItems, * yItems;
  mp_obj_get_array(x, &xLength, &xItems);
  mp_obj_get_array(y, &yLength, &yItems);
  if (xLength != yLength) {
    mp_raise_ValueError("x and y must have same dimension");
  }

  KDColor color = Palette::DataColor[paletteIndex++]; // FIXME: Share overflow routine
  for (size_t i=0; i<xLength-1; i++) {
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
