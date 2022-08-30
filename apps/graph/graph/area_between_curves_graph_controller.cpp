#include "area_between_curves_graph_controller.h"
#include "../app.h"
#include "../../shared/text_field_delegate.h"
#include <poincare/layout_helper.h>
#include <poincare/absolute_value.h>
#include <poincare/absolute_value_layout.h>
#include <poincare/integral.h>
#include <poincare/subtraction.h>

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

Poincare::Layout AreaBetweenCurvesGraphController::createFunctionLayout() {
  // TODO: Implement special case for y=...
  constexpr size_t bufferSize = SymbolAbstract::k_maxNameSize * 2 + 7; // f(x)-g(x)
  char buffer[bufferSize];
  ExpiringPointer<ContinuousFunction> functionF = App::app()->functionStore()->modelForRecord(m_record);
  size_t numberOfChars = functionF->nameWithArgument(buffer, SymbolAbstract::k_maxNameSize + 3);
  assert(numberOfChars <= bufferSize);
  numberOfChars += strlcpy(buffer + numberOfChars, "-", bufferSize-numberOfChars);
  ExpiringPointer<ContinuousFunction> functionG = App::app()->functionStore()->modelForRecord(m_secondRecord);
  numberOfChars += functionG->nameWithArgument(buffer + numberOfChars, SymbolAbstract::k_maxNameSize + 3);
  Poincare::Layout subtractionLayout = LayoutHelper::String(buffer, strlen(buffer));
  Poincare::Layout absoluteValue = AbsoluteValueLayout::Builder(subtractionLayout);
  const char * dx = "dx";
  Poincare::Layout dxLayout = LayoutHelper::String(dx, strlen(dx));
  return HorizontalLayout::Builder(absoluteValue, dxLayout);
}

Poincare::Expression AreaBetweenCurvesGraphController::createSumExpression(double startSum, double endSum, Poincare::Context * context) {
  ExpiringPointer<Shared::Function> functionF = FunctionApp::app()->functionStore()->modelForRecord(m_record);
  Poincare::Expression expressionF = functionF->expressionReduced(context).clone();
  ExpiringPointer<Shared::Function> functionG = FunctionApp::app()->functionStore()->modelForRecord(m_secondRecord);
  Poincare::Expression expressionG = functionG->expressionReduced(context).clone();
  return Integral::Builder(AbsoluteValue::Builder(Subtraction::Builder(expressionF, expressionG)), Symbol::Builder(UCodePointUnknown), Float<double>::Builder(startSum), Float<double>::Builder(endSum));
}

}
