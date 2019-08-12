#include "preimage_graph_controller.h"

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

Poincare::Coordinate2D PreimageGraphController::computeNewPointOfInterest(double start, double step, double max, Poincare::Context * context) {
  Poincare::Expression expression = Poincare::Float<double>::Builder(m_image);
  return functionStore()->modelForRecord(m_record)->nextIntersectionFrom(start, step, max, context, expression);
}

}
