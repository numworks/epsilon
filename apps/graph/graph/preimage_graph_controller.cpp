#include "preimage_graph_controller.h"
#include "../../shared/poincare_helpers.h"
#include <poincare/serialization_helper.h>

namespace Graph {

PreimageGraphController::PreimageGraphController(
  Responder * parentResponder,
  GraphView * graphView,
  BannerView * bannerView,
  Shared::InteractiveCurveViewRange * curveViewRange,
  Shared::CurveViewCursor * cursor
) :
  CalculationGraphController(
    parentResponder,
    graphView,
    bannerView,
    curveViewRange,
    cursor,
    I18n::Message::NoPreimageFound
  ),
  m_image(NAN)
{
}

Poincare::Coordinate2D<double> PreimageGraphController::computeNewPointOfInterest(double start, double step, double max, Poincare::Context * context) {
  // TODO The following three lines should be factored.
  constexpr int bufferSize = CodePoint::MaxCodePointCharLength + 1;
  char unknownX[bufferSize];
  Poincare::SerializationHelper::CodePoint(unknownX, bufferSize, UCodePointUnknownX);
  Poincare::Expression expression = Poincare::Float<double>::Builder(m_image);
  return Shared::PoincareHelpers::NextIntersection(functionStore()->modelForRecord(m_record)->expressionReduced(context), unknownX, start, step, max, context, expression);
}

}
