#ifndef PYTHON_MATPLOTLIB_PLOT_CONTROLLER_H
#define PYTHON_MATPLOTLIB_PLOT_CONTROLLER_H

#include <apps/shared/zoom_and_pan_curve_view_controller.h>
#include <python/port/port.h>
#include "plot_view.h"
#include "plot_store.h"

namespace Matplotlib {

class PlotController : public Shared::ZoomAndPanCurveViewController, public MicroPython::ExecutionViewControllerHelper {
public:
  PlotController(PlotStore * store, MicroPython::ExecutionEnvironment * executiveEnvironment) : Shared::ZoomAndPanCurveViewController(nullptr), ExecutionViewControllerHelper(executiveEnvironment), m_store(store), m_view(m_store) {}

  void viewWillAppear() override { MicroPython::ExecutionViewControllerHelper::viewWillAppear(this); }
  void viewDidDisappear() override;

protected:
  Shared::CurveView * curveView() override { return &m_view; }
  Shared::InteractiveCurveViewRange * interactiveCurveViewRange() override { return m_store; }
private:
  PlotStore * m_store;
  PlotView m_view;
};

}

#endif
