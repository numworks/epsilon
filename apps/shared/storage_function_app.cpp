#include "storage_function_app.h"
#include "../apps_container.h"

using namespace Poincare;

namespace Shared {

StorageFunctionApp::Snapshot::Snapshot() :
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

void StorageFunctionApp::Snapshot::reset() {
  m_interval.setStart(0);
  m_interval.setEnd(10);
  m_interval.setStep(1);
  m_indexFunctionSelectedByCursor = 0;
  m_modelVersion = 0;
  m_rangeVersion = 0;
  setActiveTab(0);
}

void StorageFunctionApp::Snapshot::storageDidChangeForRecord(const Ion::Storage::Record record) {
  functionStore()->storageDidChangeForRecord(record);
}

void StorageFunctionApp::willBecomeInactive() {
  if (m_modalViewController.isDisplayingModal()) {
    m_modalViewController.dismissModalViewController();
  }
  if (inputViewController()->isDisplayingModal()) {
    inputViewController()->abortEditionAndDismiss();
  }
  ::App::willBecomeInactive();
}

bool StorageFunctionApp::isAcceptableExpression(const Poincare::Expression expression) {
  if (!TextFieldDelegateApp::ExpressionCanBeSerialized(expression, false, Expression())) {
    return false;
  }
  return TextFieldDelegateApp::isAcceptableExpression(expression);
}

}
