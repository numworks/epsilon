extern "C" {
#include "modpyplot.h"
}
#include "port.h"
#include "plot_controller.h"

Matplotlib::PlotController sPlotController;

mp_obj_t modpyplot_plot(mp_obj_t x, mp_obj_t y) {
  // Ensure x and y are arrays
  // "Push" x and y on bigger arrays
}

mp_obj_t modpyplot_show() {
  MicroPython::ExecutionEnvironment * env = MicroPython::ExecutionEnvironment::currentExecutionEnvironment();
  env->displayViewController(&sPlotController);
}
