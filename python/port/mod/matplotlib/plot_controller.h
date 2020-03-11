#ifndef PYTHON_MATPLOTLIB_PLOT_CONTROLLER_H
#define PYTHON_MATPLOTLIB_PLOT_CONTROLLER_H

#include <apps/shared/simple_interactive_curve_view_controller.h>
#include <apps/shared/curve_view_cursor.h>
#include "plot_view.h"
#include "plot_store.h"

namespace Matplotlib {

class PlotController : public Shared::SimpleInteractiveCurveViewController {
public:
  PlotController(PlotStore * store) : Shared::SimpleInteractiveCurveViewController(nullptr, &m_cursor), m_store(store), m_view(m_store) {}

  float cursorBottomMarginRatio() override {
    return 0.0f;
  }
  virtual void reloadBannerView() override {
  }

  virtual bool handleEnter() override {
    return false;
  }

protected:
  Shared::CurveView * curveView() override { return &m_view; }
  Shared::InteractiveCurveViewRange * interactiveCurveViewRange() override { return m_store; }
private:
  PlotStore * m_store;
  Shared::CurveViewCursor m_cursor;
  PlotView m_view;
};

}


#endif
