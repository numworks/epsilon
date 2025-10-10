#include "graph_view_model.h"

#include "assert.h"
#include "bar_icon.h"
#include "pie_icon.h"

namespace Statistics::Categorical {

I18n::Message GraphViewModel::MessageForGraphView(GraphView graphView) {
  constexpr I18n::Message k_messages[k_numberOfGraphViews] = {
      I18n::Message::BarGraph, I18n::Message::PieGraph};
  return k_messages[IndexOfGraphView(graphView)];
}

const Escher::Image* const GraphViewModel::ImageForGraphView(
    GraphView graphView) {
  const Escher::Image* const k_images[k_numberOfGraphViews] = {
      ImageStore::BarIcon, ImageStore::PieIcon};
  return k_images[IndexOfGraphView(graphView)];
}

GraphViewModel::GraphView GraphViewModel::GraphViewAtIndex(uint8_t index) {
  return static_cast<GraphView>(index);
}

uint8_t GraphViewModel::IndexOfGraphView(GraphView graphView) {
  assert(static_cast<uint8_t>(graphView) < k_numberOfGraphViews);
  return static_cast<uint8_t>(graphView);
}

}  // namespace Statistics::Categorical
