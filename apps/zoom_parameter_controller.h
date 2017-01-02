#ifndef APPS_ZOOM_PARAMETER_CONTROLLER_H
#define APPS_ZOOM_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "curve_view_window_with_cursor.h"
#include "curve_view_with_banner_and_cursor.h"

class ZoomParameterController : public ViewController {
public:
  ZoomParameterController(Responder * parentResponder, CurveViewWindowWithCursor * graphWindow, CurveViewWithBannerAndCursor * graphView);
  const char * title() const override;
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
private:
  class ContentView : public View {
  public:
    ContentView(CurveViewWithBannerAndCursor * graphView);
    void layoutSubviews() override;
    CurveViewWithBannerAndCursor * graphView();
  private:
    class LegendView : public View {
    public:
      LegendView();
      void drawRect(KDContext * ctx, KDRect rect) const override;
    private:
      constexpr static KDColor k_legendBackgroundColor = KDColor::RGB24(0xECECEC);
      constexpr static int k_numberOfLegends = 6;
      constexpr static KDCoordinate k_tokenWidth = 30;
      void layoutSubviews() override;
      int numberOfSubviews() const override;
      View * subviewAtIndex(int index) override;
      PointerTextView m_legends[k_numberOfLegends];
    };
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    CurveViewWithBannerAndCursor * m_graphView;
    LegendView m_legendView;
    constexpr static KDCoordinate k_legendHeight = 50;
  };
  ContentView m_contentView;
  CurveViewWindowWithCursor * m_graphWindow;
};

#endif
