#ifndef PYTHON_MATPLOTLIB_PLOT_CONTROLLER_H
#define PYTHON_MATPLOTLIB_PLOT_CONTROLLER_H

#include <apps/shared/zoom_and_pan_curve_view_controller.h>
#include "plot_view.h"
#include "plot_store.h"

namespace Matplotlib {

class PlotController : public Shared::ZoomAndPanCurveViewController {
public:
  PlotController(PlotStore * store) : Shared::ZoomAndPanCurveViewController(nullptr), m_store(store), m_view(m_store) {}

  void viewWillAppear() override;
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
