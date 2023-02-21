#include "preimage_graph_controller.h"

#include <poincare/float.h>
#include <poincare/serialization_helper.h>

#include "../../shared/poincare_helpers.h"

using namespace Poincare;
using namespace Shared;
using namespace Escher;

namespace Graph {

PreimageGraphController::PreimageGraphController(
    Responder* parentResponder, GraphView* graphView, BannerView* bannerView,
    InteractiveCurveViewRange* curveViewRange, CurveViewCursor* cursor)
    : CalculationGraphController(parentResponder, graphView, bannerView,
                                 curveViewRange, cursor,
                                 I18n::Message::NoPreimageFound),
      m_image(NAN) {}

Coordinate2D<double> PreimageGraphController::computeNewPointOfInterest(
    double start, double max, Context* context) {
  Solver<double> solver = PoincareHelpers::Solver(
      start, max, ContinuousFunction::k_unknownName, context);
  Expression f =
      functionStore()->modelForRecord(m_record)->expressionReduced(context);
  return solver.nextIntersection(Float<double>::Builder(m_image), f);
}

}  // namespace Graph
