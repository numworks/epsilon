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
  assert(m_previousModelsVersions[0] == 0);
}

void FunctionApp::Snapshot::reset() {
  m_indexFunctionSelectedByCursor = 0;
  m_modelVersion = 0;
  assert(sizeof(m_previousModelsVersions) == sizeof(uint32_t) * FunctionGraphController::sNumberOfMemoizedModelVersions);
  memset(m_previousModelsVersions, 0, sizeof(m_previousModelsVersions));
  m_rangeVersion = 0;
  setActiveTab(0);
}

void FunctionApp::Snapshot::storageDidChangeForRecord(const Ion::Storage::Record record) {
  functionStore()->storageDidChangeForRecord(record);
}

void FunctionApp::willBecomeInactive() {
  if (m_modalViewController.isDisplayingModal()) {
    m_modalViewController.dismissModalViewController(true);
  }
  if (inputViewController()->isDisplayingModal()) {
    inputViewController()->abortEditionAndDismiss();
  }
  ::App::willBecomeInactive();
}


bool FunctionApp::isAcceptableExpression(const Expression exp) {
 return TextFieldDelegateApp::isAcceptableExpression(exp) && ExpressionCanBeSerialized(exp, false, Expression(), localContext());
}

}
