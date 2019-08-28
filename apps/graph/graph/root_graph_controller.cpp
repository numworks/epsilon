#include "root_graph_controller.h"
#include "../../shared/poincare_helpers.h"
#include <poincare/serialization_helper.h>

using namespace Shared;
using namespace Poincare;

namespace Graph {

RootGraphController::RootGraphController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, CurveViewCursor * cursor) :
  CalculationGraphController(parentResponder, graphView, bannerView, curveViewRange, cursor, I18n::Message::NoZeroFound)
{
}

const char * RootGraphController::title() {
  return I18n::translate(I18n::Message::Zeros);
}

Coordinate2D<double> RootGraphController::computeNewPointOfInterest(double start, double step, double max, Context * context) {
  // TODO The following three lines should be factored.
  constexpr int bufferSize = CodePoint::MaxCodePointCharLength + 1;
  char unknownX[bufferSize];
  Poincare::SerializationHelper::CodePoint(unknownX, bufferSize, UCodePointUnknownX);
  return Coordinate2D<double>(Shared::PoincareHelpers::NextRoot(functionStore()->modelForRecord(m_record)->expressionReduced(context), unknownX, start, step, max, context), 0.0);
}

}
