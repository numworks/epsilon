#include "extremum_graph_controller.h"
#include "../app.h"

using namespace Shared;
using namespace Poincare;

namespace Graph {

const char * MinimumGraphController::title() {
  return I18n::translate(I18n::Message::Minimum);
}

CartesianFunction::Point MinimumGraphController::computeNewPointOfInterest(double start, double step, double max, Context * context) {
  return m_function->nextMinimumFrom(start, step, max, context);
}

const char * MaximumGraphController::title() {
  return I18n::translate(I18n::Message::Maximum);
}

CartesianFunction::Point MaximumGraphController::computeNewPointOfInterest(double start, double step, double max, Context * context) {
  return m_function->nextMaximumFrom(start, step, max, context);
}

}
