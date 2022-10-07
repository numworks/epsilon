#include "extremum_graph_controller.h"
#include <apps/shared/poincare_helpers.h>
#include <poincare/serialization_helper.h>

using namespace Escher;
using namespace Poincare;
using namespace Shared;

namespace Graph {

MinimumGraphController::MinimumGraphController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, Shared::CurveViewCursor * cursor) :
  CalculationGraphController(parentResponder, graphView, bannerView, curveViewRange, cursor, I18n::Message::NoMinimumFound)
{
}

const char * MinimumGraphController::title() {
  return I18n::translate(I18n::Message::Minimum);
}

void MinimumGraphController::viewWillAppear() {
  CalculationGraphController::viewWillAppear();
  m_graphView->setInterest(Solver<double>::Interest::LocalMinimum);
}

Coordinate2D<double> MinimumGraphController::computeNewPointOfInterest(double start, double max, Poincare::Context * context) {
  Solver<double> solver = PoincareHelpers::Solver(start, max, ContinuousFunction::k_unknownName, context);
  return solver.nextMinimum(functionStore()->modelForRecord(m_record)->expressionReduced(context));
}

MaximumGraphController::MaximumGraphController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, Shared::CurveViewCursor * cursor) :
  CalculationGraphController(parentResponder, graphView, bannerView, curveViewRange, cursor, I18n::Message::NoMaximumFound)
{
}

const char * MaximumGraphController::title() {
  return I18n::translate(I18n::Message::Maximum);
}

void MaximumGraphController::viewWillAppear() {
  CalculationGraphController::viewWillAppear();
  m_graphView->setInterest(Solver<double>::Interest::LocalMaximum);
}

Coordinate2D<double> MaximumGraphController::computeNewPointOfInterest(double start, double max, Poincare::Context * context) {
  Solver<double> solver = PoincareHelpers::Solver(start, max, ContinuousFunction::k_unknownName, context);
  return solver.nextMaximum(functionStore()->modelForRecord(m_record)->expressionReduced(context));
}

}
