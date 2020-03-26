#include "plot_controller.h"

namespace Matplotlib {

void PlotController::viewWillAppear() {
  curveView()->reload();
}

void PlotController::viewDidDisappear() {
  m_store->flush();
}

}
