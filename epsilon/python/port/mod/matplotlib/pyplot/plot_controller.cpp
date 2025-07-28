#include "plot_controller.h"

namespace Matplotlib {

void PlotController::viewWillAppear() {
  m_store->initRange();
  curveView()->reload();
}

void PlotController::viewDidDisappear() { m_store->flush(); }

}  // namespace Matplotlib
