#include "root_graph_controller.h"
#include "../../shared/poincare_helpers.h"
#include <poincare/serialization_helper.h>

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Graph {

RootGraphController::RootGraphController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, InteractiveCurveViewRange * curveViewRange, CurveViewCursor * cursor) :
  CalculationGraphController(parentResponder, graphView, bannerView, curveViewRange, cursor, I18n::Message::NoZeroFound)
{
}

const char * RootGraphController::title() {
  return I18n::translate(I18n::Message::Zeros);
}

void RootGraphController::viewWillAppear() {
  CalculationGraphController::viewWillAppear();
  m_graphView->setInterest(Solver<double>::Interest::Root);
}

Coordinate2D<double> RootGraphController::computeNewPointOfInterest(double start, double max, Context * context) {
  Solver<double> solver = PoincareHelpers::Solver(start, max, ContinuousFunction::k_unknownName, context);
  return solver.nextRoot(functionStore()->modelForRecord(m_record)->expressionReduced(context));
}

}
