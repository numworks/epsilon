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

CurveViewCursor * StorageFunctionApp::Snapshot::cursor() {
  return &m_cursor;
}

uint32_t * StorageFunctionApp::Snapshot::modelVersion() {
  return &m_modelVersion;
}

uint32_t * StorageFunctionApp::Snapshot::rangeVersion() {
  return &m_rangeVersion;
}

Preferences::AngleUnit * StorageFunctionApp::Snapshot::angleUnitVersion() {
  return &m_angleUnitVersion;
}

Interval * StorageFunctionApp::Snapshot::interval() {
  return &m_interval;
}

int * StorageFunctionApp::Snapshot::indexFunctionSelectedByCursor() {
  return &m_indexFunctionSelectedByCursor;
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

StorageFunctionApp::StorageFunctionApp(Container * container, Snapshot * snapshot, ViewController * rootViewController) :
  ExpressionFieldDelegateApp(container, snapshot, rootViewController)
{
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


bool StorageFunctionApp::isAcceptableExpression(const Expression exp, Responder * responder) {
  if (TextFieldDelegateApp::isAcceptableExpression(exp, responder)) {
    assert(!exp.isUninitialized());
    if (exp.type() == ExpressionNode::Type::Store) {
      // We do not want to allow a function to be "3->a" or "5->f(x)"
      responder->app()->displayWarning(I18n::Message::StoreExpressionNotAcceptedAsFunction);
      return false;
    }
    return true;
  }
  return false;
}

}
