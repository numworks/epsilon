#include "function_app.h"

using namespace Poincare;

namespace Shared {

FunctionApp::Snapshot::Snapshot() :
  m_cursor(),
  m_interval(),
  m_indexFunctionSelectedByCursor(0),
  m_modelVersion(0),
  m_rangeVersion(0),
  m_angleUnitVersion(Preferences::AngleUnit::Radian)
{
  m_interval.setStart(0);
  m_interval.setEnd(10);
  m_interval.setStep(1);
}

void FunctionApp::Snapshot::reset() {
  m_interval.setStart(0);
  m_interval.setEnd(10);
  m_interval.setStep(1);
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
  if (!TextFieldDelegateApp::ExpressionCanBeSerialized(expression, false, Expression())) {
    return false;
  }
  return TextFieldDelegateApp::isAcceptableExpression(expression);
}

}
