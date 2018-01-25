#include "function_app.h"
#include "../apps_container.h"

using namespace Poincare;

namespace Shared {

FunctionApp::Snapshot::Snapshot() :
  m_cursor(),
  m_interval(),
  m_indexFunctionSelectedByCursor(0),
  m_modelVersion(0),
  m_rangeVersion(0),
  m_angleUnitVersion(Expression::AngleUnit::Radian)
{
  m_interval.setStart(0);
  m_interval.setEnd(10);
  m_interval.setStep(1);
}

CurveViewCursor * FunctionApp::Snapshot::cursor() {
  return &m_cursor;
}

uint32_t * FunctionApp::Snapshot::modelVersion() {
  return &m_modelVersion;
}

uint32_t * FunctionApp::Snapshot::rangeVersion() {
  return &m_rangeVersion;
}

Expression::AngleUnit * FunctionApp::Snapshot::angleUnitVersion() {
  return &m_angleUnitVersion;
}

Interval * FunctionApp::Snapshot::interval() {
  return &m_interval;
}

int * FunctionApp::Snapshot::indexFunctionSelectedByCursor() {
  return &m_indexFunctionSelectedByCursor;
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

FunctionApp::FunctionApp(Container * container, Snapshot * snapshot, ViewController * rootViewController) :
  TextFieldDelegateApp(container, snapshot, rootViewController)
{
}

void FunctionApp::willBecomeInactive() {
  if (m_modalViewController.isDisplayingModal()) {
    m_modalViewController.dismissModalViewController();
  }
  if (inputViewController()->isDisplayingModal()) {
    inputViewController()->abortTextFieldEditionAndDismiss();
  }
  ::App::willBecomeInactive();
}

}
