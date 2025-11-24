#include "preimage_graph_controller.h"

#include <omg/utf8_helper.h>
#include <poincare/system_expression.h>

#include "shared/global_store.h"

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
    double start, double max, double, bool stretch) {
  Solver<double> solver =
      Poincare::Solver(start, max, Shared::GlobalContextAccessor::Context());
  if (stretch) {
    solver.stretch();
  }
  PreparedFunction f =
      functionStore()->modelForRecord(m_record)->expressionApproximated();
  return solver
      .nextIntersection(SystemExpression::Builder<double>(m_image).tree(),
                        f.tree())
      .xy();
}

}  // namespace Graph
