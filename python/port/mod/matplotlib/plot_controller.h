#ifndef PYTHON_MATPLOTLIB_PLOT_CONTROLLER_H
#define PYTHON_MATPLOTLIB_PLOT_CONTROLLER_H

//#include <escher/view_controller.h>
#include <apps/shared/simple_interactive_curve_view_controller.h>
#include <apps/shared/curve_view_cursor.h>
#include "plot_view.h"
#include "plot_store.h"

#include <stdio.h>

namespace Matplotlib {

class PlotController : public Shared::SimpleInteractiveCurveViewController {
public:
  PlotController() : Shared::SimpleInteractiveCurveViewController(nullptr, &m_cursor), m_store(), m_view(&m_store) {}
  //virtual View * view() override { return &m_view; }

  float cursorBottomMarginRatio() override {
    return 0.0f;
  }
  virtual void reloadBannerView() override {
  }

  bool handleEvent(Ion::Events::Event event) override {
    printf("EVENT!!\n");
    return Shared::SimpleInteractiveCurveViewController::handleEvent(event);
  }

  virtual bool handleEnter() override {
    printf("ENTER !!\n");
    return false;
  }

protected:
  Shared::CurveView * curveView() override { return &m_view; }
  Shared::InteractiveCurveViewRange * interactiveCurveViewRange() override { return &m_store; }
private:
  Shared::CurveViewCursor m_cursor;
  PlotStore m_store;
  PlotView m_view;
};

}


#endif
