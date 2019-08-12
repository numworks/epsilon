#include "root_graph_controller.h"
#include "../app.h"

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

Coordinate2D RootGraphController::computeNewPointOfInterest(double start, double step, double max, Context * context) {
  return Coordinate2D(functionStore()->modelForRecord(m_record)->nextRootFrom(start, step, max, context), 0.0);
}

}
