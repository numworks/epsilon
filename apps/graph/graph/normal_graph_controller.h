#ifndef GRAPH_NORMAL_GRAPH_CONTROLLER_H
#define GRAPH_NORMAL_GRAPH_CONTROLLER_H

#include <apps/shared/function_banner_delegate.h>
#include <apps/shared/simple_interactive_curve_view_controller.h>

#include "banner_view.h"
#include "graph_controller_helper.h"
#include "graph_view.h"

namespace Graph {

class NormalGraphController
    : public Shared::SimpleInteractiveCurveViewController,
      public Shared::FunctionBannerDelegate,
      public GraphControllerHelper {
 public:
  NormalGraphController(Escher::Responder* parentResponder,
                         GraphView* graphView, BannerView* bannerView,
                         Shared::InteractiveCurveViewRange* curveViewRange,
                         Shared::CurveViewCursor* cursor);
  const char* title() override;
  void viewWillAppear() override;
  void didBecomeFirstResponder() override;
  TELEMETRY_ID("Normal");
  bool textFieldDidFinishEditing(Escher::AbstractTextField* textField,
                                 Ion::Events::Event event) override;
  void setRecord(Ion::Storage::Record record);

 private:
  float cursorBottomMarginRatio() const override {
    return cursorBottomMarginRatioForBannerHeight(
        m_bannerView->minimalSizeForOptimalDisplay().height());
  }
  Shared::InteractiveCurveViewRange* interactiveCurveViewRange() override {
    return m_graphRange;
  }
  Shared::AbstractPlotView* curveView() override { return m_graphView; }
  BannerView* bannerView() override { return m_bannerView; };
  GraphView* graphView() override { return m_graphView; };
  Shared::FunctionBannerDelegate* functionBannerDelegate() override {
    return this;
  }
  void reloadBannerView() override;
  bool moveCursorHorizontally(OMG::HorizontalDirection direction,
                              int scrollSpeed = 1) override;
  bool handleEnter() override;
  GraphView* m_graphView;
  BannerView* m_bannerView;
  Shared::InteractiveCurveViewRange* m_graphRange;
  Ion::Storage::Record m_record;
};

}  // namespace Graph

#endif
