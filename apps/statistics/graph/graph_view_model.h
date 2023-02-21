#ifndef STATISTICS_GRAPH_VIEW_MODEL_H
#define STATISTICS_GRAPH_VIEW_MODEL_H

#include <apps/i18n.h>
#include <escher/image.h>
#include <stdint.h>

namespace Statistics {

class GraphViewModel {
 public:
  enum class GraphView : uint8_t {
    Histogram = 0,
    Box = 1,
    Frequency = 2,
    NormalProbability = 3
  };
  constexpr static uint8_t k_numberOfGraphViews = 4;

  static I18n::Message MessageForGraphView(GraphView graphView);
  static const Escher::Image* const ImageForGraphView(GraphView graphView);
  static GraphView GraphViewAtIndex(uint8_t index);
  static uint8_t IndexOfGraphView(GraphView graphView);

  GraphViewModel() : m_selectedGraphView(GraphView::Histogram) {}
  GraphView selectedGraphView() { return m_selectedGraphView; }
  void selectGraphView(GraphView graphView) { m_selectedGraphView = graphView; }

 private:
  GraphView m_selectedGraphView;
};

}  // namespace Statistics

#endif
