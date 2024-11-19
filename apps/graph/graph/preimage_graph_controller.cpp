#include "preimage_graph_controller.h"

#include <poincare/expression.h>
#include <poincare/old/serialization_helper.h>

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
  Solver<double> solver = Poincare::Solver(start, max, context);
  SystemFunction f =
      functionStore()->modelForRecord(m_record)->expressionApproximated(
          context);
  return solver
      .nextIntersection(NewExpression::Builder<double>(m_image).tree(),
                        f.tree())
      .xy;
}

}  // namespace Graph
