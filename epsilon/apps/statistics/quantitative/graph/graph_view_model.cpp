#include "graph_view_model.h"

#include <apps/statistics/assets/box_icon.h>
#include <apps/statistics/assets/cumulative_icon.h>
#include <apps/statistics/assets/histogram_icon.h>
#include <apps/statistics/assets/statistics_normal_icon.h>
#include <escher/transparent_themed_image_view.h>

#include "assert.h"

using namespace Statistics;

I18n::Message GraphViewModel::MessageForGraphView(GraphView graphView) {
  constexpr I18n::Message k_messages[k_numberOfGraphViews] = {
      I18n::Message::Histogram, I18n::Message::BoxAndWhiskers,
      I18n::Message::CumulativeFrequency, I18n::Message::NormalProbabilityPlot};
  return k_messages[IndexOfGraphView(graphView)];
}

const Escher::Image* const GraphViewModel::ImageForGraphView(
    GraphView graphView) {
  const Escher::ThemedImage* const k_themedImages[k_numberOfGraphViews] = {
      ImageStore::HistogramIcon, ImageStore::BoxIcon,
      ImageStore::CumulativeIcon, ImageStore::StatisticsNormalIcon};
  return Escher::TransparentThemedImageView(
             *k_themedImages[IndexOfGraphView(graphView)])
      .currentImage();
}

GraphViewModel::GraphView GraphViewModel::GraphViewAtIndex(uint8_t index) {
  return static_cast<GraphView>(index);
}

uint8_t GraphViewModel::IndexOfGraphView(GraphView graphView) {
  assert(static_cast<uint8_t>(graphView) >= 0 &&
         static_cast<uint8_t>(graphView) < k_numberOfGraphViews);
  return static_cast<uint8_t>(graphView);
}
