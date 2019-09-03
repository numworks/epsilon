#include "function_app.h"

using namespace Poincare;

namespace Shared {

FunctionApp::Snapshot::Snapshot() :
  m_cursor(),
  m_indexFunctionSelectedByCursor(0),
  m_modelVersion(0),
  m_rangeVersion(0),
  m_angleUnitVersion(Preferences::AngleUnit::Radian)
{
}

void FunctionApp::Snapshot::reset() {
  m_indexFunctionSelectedByCursor = 0;
  m_modelVersion = 0;
  m_rangeVersion = 0;
  setActiveTab(0);
}

void FunctionApp::Snapshot::storageDidChangeForRecord(const Ion::Storage::Record record) {
  functionStore()->storageDidChangeForRecord(record);
}

void FunctionApp::willBecomeInactive() {
  if (m_modalViewController.isDisplayingModal()) {
    m_modalViewController.dismissModalViewController();
  }
  if (inputViewController()->isDisplayingModal()) {
    inputViewController()->abortEditionAndDismiss();
  }
  ::App::willBecomeInactive();
}

bool FunctionApp::isAcceptableExpression(const Poincare::Expression expression) {
  /* We forbid functions whose type is equal because the input "2+f(3)" would be
   * simplify to an expression with an nested equal node which makes no sense. */
  if (!TextFieldDelegateApp::ExpressionCanBeSerialized(expression, false, Expression()) || expression.type() == ExpressionNode::Type::Equal) {
    return false;
  }
  return TextFieldDelegateApp::isAcceptableExpression(expression);
}

}
