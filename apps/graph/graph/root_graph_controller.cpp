#include "root_graph_controller.h"
#include "../app.h"

using namespace Shared;
using namespace Poincare;

namespace Graph {

const char * RootGraphController::title() {
  return I18n::translate(I18n::Message::Zeros);
}

CartesianFunction::Point RootGraphController::computeNewPointOfInterest(double start, double step, double max, Context * context) {
  return {.abscissa = m_function->nextRootFrom(start, step, max, context), .value = 0.0};
}

}
