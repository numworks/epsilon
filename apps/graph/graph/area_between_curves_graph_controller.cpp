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

void AreaBetweenCurvesGraphController::viewDidDisappear() {
  m_graphView->selectSecondRecord(nullptr);
  m_secondRecord = nullptr;
  IntegralGraphController::viewDidDisappear();
}

void AreaBetweenCurvesGraphController::setSecondRecord(Ion::Storage::Record record) {
  m_graphView->selectSecondRecord(record);
  m_secondRecord = record;
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

void AreaBetweenCurvesGraphController::makeCursorVisible() {
  float position = m_cursor->x();
  ExpiringPointer<Shared::Function> functionF = FunctionApp::app()->functionStore()->modelForRecord(m_record);
  float yF = functionF->evaluateXYAtParameter(position, FunctionApp::app()->localContext()).x2();
  // Do not zoom out if user is selecting first parameter
  makeDotVisible(position, yF, m_step != Step::FirstParameter);
  assert(!m_secondRecord.isNull());
  ExpiringPointer<Shared::Function> functionG = FunctionApp::app()->functionStore()->modelForRecord(m_secondRecord);
  float yG = functionG->evaluateXYAtParameter(position, FunctionApp::app()->localContext()).x2();
  // zoomOut is always true so that the user can see both dots
  makeDotVisible(position, yG, true);
}

}
