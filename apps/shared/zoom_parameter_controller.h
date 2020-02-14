#ifndef SHARED_ZOOM_PARAMETER_CONTROLLER_H
#define SHARED_ZOOM_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "interactive_curve_view_range.h"
#include "curve_view.h"
#include <apps/i18n.h>

namespace Shared {

class ZoomParameterController : public ViewController {
public:
  ZoomParameterController(Responder * parentResponder, InteractiveCurveViewRange * interactiveCurveViewRange, CurveView * curveView);
  const char * title() override;
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
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
  ContentView m_contentView;
  InteractiveCurveViewRange * m_interactiveRange;
};

}

#endif
