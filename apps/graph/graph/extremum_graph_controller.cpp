#include "extremum_graph_controller.h"
#include "../app.h"

using namespace Shared;
using namespace Poincare;

namespace Graph {

MinimumGraphController::MinimumGraphController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, Shared::CurveViewCursor * cursor) :
  CalculationGraphController(parentResponder, graphView, bannerView, curveViewRange, cursor, I18n::Message::NoMinimumFound)
{
}

const char * MinimumGraphController::title() {
  return I18n::translate(I18n::Message::Minimum);
}

CartesianFunction::Point MinimumGraphController::computeNewPointOfInterest(double start, double step, double max, Context * context) {
  return m_function->nextMinimumFrom(start, step, max, context);
}

MaximumGraphController::MaximumGraphController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, Shared::CurveViewCursor * cursor) :
  CalculationGraphController(parentResponder, graphView, bannerView, curveViewRange, cursor, I18n::Message::NoMaximumFound)
{
}

const char * MaximumGraphController::title() {
  return I18n::translate(I18n::Message::Maximum);
}

CartesianFunction::Point MaximumGraphController::computeNewPointOfInterest(double start, double step, double max, Context * context) {
  return m_function->nextMaximumFrom(start, step, max, context);
}

}
