#ifndef GRAPH_GRAPH_ZOOM_PARAMETER_CONTROLLER_H
#define GRAPH_GRAPH_ZOOM_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "axis_interval.h"
#include "graph_view.h"

namespace Graph {

class ZoomParameterController : public ViewController {
public:
  ZoomParameterController(Responder * parentResponder, AxisInterval * axisInterval, GraphView * graphView);
  const char * title() const override;
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
private:
  class ContentView : public View {
  public:
    ContentView(GraphView * graphView);
    void layoutSubviews() override;
    GraphView * graphView();
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
    GraphView * m_graphView;
    LegendView m_legendView;
    constexpr static KDCoordinate k_legendHeight = 50;
  };
  ContentView m_contentView;
  AxisInterval * m_axisInterval;
};

}

#endif
