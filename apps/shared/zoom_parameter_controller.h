#ifndef SHARED_ZOOM_PARAMETER_CONTROLLER_H
#define SHARED_ZOOM_PARAMETER_CONTROLLER_H

#include "zoom_and_pan_curve_view_controller.h"
#include <apps/i18n.h>

namespace Shared {

class ZoomParameterController : public ZoomAndPanCurveViewController {
public:
  ZoomParameterController(Responder * parentResponder, InteractiveCurveViewRange * interactiveCurveViewRange, CurveView * curveView);
  const char * title() override;
  View * view() override { return &m_contentView; }
  void viewWillAppear() override;
  void viewDidDisappear() override;
  void didBecomeFirstResponder() override;
  TELEMETRY_ID("Zoom");
private:
  constexpr static KDCoordinate k_standardViewHeight = 175;

  class ContentView : public View {
  public:
    constexpr static KDCoordinate k_legendHeight = 30;
    ContentView(CurveView * curveView);
    void layoutSubviews(bool force = false) override;
    CurveView * curveView();
  private:
    class LegendView : public View {
    public:
      LegendView();
      void drawRect(KDContext * ctx, KDRect rect) const override;
    private:
      constexpr static int k_numberOfLegends = 3;
      constexpr static int k_numberOfTokens = 6;
      constexpr static KDCoordinate k_tokenWidth = 10;
      void layoutSubviews(bool force = false) override;
      int numberOfSubviews() const override;
      View * subviewAtIndex(int index) override;
      MessageTextView m_legends[k_numberOfLegends];
      KeyView m_legendPictograms[k_numberOfTokens];
    };
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    CurveView * m_curveView;
    LegendView m_legendView;
  };

  void adaptCurveRange(bool viewWillAppear);

  // ZoomAndPanCurveViewController
  InteractiveCurveViewRange * interactiveCurveViewRange() override { return m_interactiveRange; }
  CurveView * curveView() override { return m_contentView.curveView(); }

  ContentView m_contentView;
  InteractiveCurveViewRange * m_interactiveRange;
};

}

#endif
