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
  return functionStore()->modelForRecord(m_record)->nextRootFrom(start, step, max, context);
}

}
