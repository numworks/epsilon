#include "plot_controller.h"

namespace Matplotlib {

void PlotController::viewDidDisappear() {
  MicroPython::ExecutionViewControllerHelper::viewDidDisappear(this);
  m_store->flush();
}

}
