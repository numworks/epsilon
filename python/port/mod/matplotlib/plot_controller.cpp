#include "plot_controller.h"

namespace Matplotlib {

void PlotController::viewWillAppear() {
  MicroPython::ExecutionViewControllerHelper::viewWillAppear(this);
  curveView()->reload();
}

void PlotController::viewDidDisappear() {
  MicroPython::ExecutionViewControllerHelper::viewDidDisappear(this);
  m_store->flush();
}

}
