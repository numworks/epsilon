#include "area_between_curves_graph_controller.h"
#include "../../shared/text_field_delegate.h"
#include <poincare/layout_helper.h>
#include "../app.h"

#include <assert.h>
#include <cmath>
#include <stdlib.h>

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Graph {

const char * AreaBetweenCurvesGraphController::title() {
  // TODO: New title
  return I18n::translate(I18n::Message::UseFunctionModel);
}

Layout AreaBetweenCurvesGraphController::createFunctionLayout(ExpiringPointer<Shared::Function> function) {
  // TODO: New function layout
  constexpr size_t bufferSize = SymbolAbstract::k_maxNameSize+5; // f(x)dx
  char buffer[bufferSize];
  const char * dx = "dx";
  size_t numberOfChars = function->nameWithArgument(buffer, bufferSize-strlen(dx));
  assert(numberOfChars <= bufferSize);
  strlcpy(buffer+numberOfChars, dx, bufferSize-numberOfChars);
  return LayoutHelper::String(buffer, strlen(buffer));
}

}
