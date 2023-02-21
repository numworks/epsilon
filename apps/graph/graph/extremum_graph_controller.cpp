#include "extremum_graph_controller.h"

#include <apps/shared/poincare_helpers.h>
#include <poincare/serialization_helper.h>

using namespace Escher;
using namespace Poincare;
using namespace Shared;

namespace Graph {

MinimumGraphController::MinimumGraphController(
    Responder* parentResponder, GraphView* graphView, BannerView* bannerView,
    Shared::InteractiveCurveViewRange* curveViewRange,
    Shared::CurveViewCursor* cursor)
    : CalculationGraphController(parentResponder, graphView, bannerView,
                                 curveViewRange, cursor,
                                 I18n::Message::NoMinimumFound) {}

const char* MinimumGraphController::title() {
  return I18n::translate(I18n::Message::Minimum);
}

MaximumGraphController::MaximumGraphController(
    Responder* parentResponder, GraphView* graphView, BannerView* bannerView,
    Shared::InteractiveCurveViewRange* curveViewRange,
    Shared::CurveViewCursor* cursor)
    : CalculationGraphController(parentResponder, graphView, bannerView,
                                 curveViewRange, cursor,
                                 I18n::Message::NoMaximumFound) {}

const char* MaximumGraphController::title() {
  return I18n::translate(I18n::Message::Maximum);
}

}  // namespace Graph
