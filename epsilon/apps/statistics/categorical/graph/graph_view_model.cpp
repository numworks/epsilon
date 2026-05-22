#include "graph_view_model.h"

#include <apps/global_preferences.h>

#include "assert.h"
#include "bar_dark_icon.h"
#include "bar_light_icon.h"
#include "pie_dark_icon.h"
#include "pie_light_icon.h"

namespace Statistics::Categorical {

I18n::Message GraphViewModel::MessageForGraphView(GraphView graphView) {
  constexpr I18n::Message k_messages[k_numberOfGraphViews] = {
      I18n::Message::BarGraph, I18n::Message::PieGraph};
  return k_messages[IndexOfGraphView(graphView)];
}

const Escher::Image* const GraphViewModel::ImageForGraphView(
    GraphView graphView) {
  const bool dark = SharedGlobalPrefs().theme() == Escher::Theme::Dark;
  const Escher::Image* const k_images[k_numberOfGraphViews] = {
      dark ? ImageStore::BarDarkIcon : ImageStore::BarLightIcon,
      dark ? ImageStore::PieDarkIcon : ImageStore::PieLightIcon};
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
