#pragma once

#include <apps/i18n.h>
#include <escher/image.h>
#include <stdint.h>

namespace Statistics::Categorical {

class GraphViewModel {
 public:
  enum class GraphView : uint8_t {
    Bar = 0,
    Pie = 1,
    NumberOfGraphView,
  };
  constexpr static uint8_t k_numberOfGraphViews =
      static_cast<uint8_t>(GraphView::NumberOfGraphView);

  static I18n::Message MessageForGraphView(GraphView graphView);
  static const Escher::Image* const ImageForGraphView(GraphView graphView);
  static GraphView GraphViewAtIndex(uint8_t index);
  static uint8_t IndexOfGraphView(GraphView graphView);

  GraphViewModel() : m_selectedGraphView(GraphView::Bar) {}
  GraphView selectedGraphView() const { return m_selectedGraphView; }
  void selectGraphView(GraphView graphView) { m_selectedGraphView = graphView; }

 private:
  GraphView m_selectedGraphView;
};

}  // namespace Statistics::Categorical
