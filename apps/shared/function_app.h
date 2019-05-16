#ifndef SHARED_FUNCTION_APP_H
#define SHARED_FUNCTION_APP_H

#include "expression_field_delegate_app.h"
#include "function_store.h"
#include "curve_view_cursor.h"
#include "interval.h"

class AppsContainer;

namespace Shared {

class FunctionApp : public ExpressionFieldDelegateApp {
public:
  class Snapshot : public ::App::Snapshot, public TabViewDataSource {
  public:
    Snapshot();
    CurveViewCursor * cursor() { return &m_cursor; }
    uint32_t * modelVersion() { return &m_modelVersion; }
    uint32_t * rangeVersion() { return &m_rangeVersion; }
    Poincare::Preferences::AngleUnit * angleUnitVersion() { return &m_angleUnitVersion; }
    virtual FunctionStore * functionStore() = 0;
    Interval * interval() { return &m_interval; }
    int * indexFunctionSelectedByCursor() { return &m_indexFunctionSelectedByCursor; }
    void reset() override;
    void storageDidChangeForRecord(const Ion::Storage::Record record) override;
  protected:
    CurveViewCursor m_cursor;
    Interval m_interval;
  private:
    int m_indexFunctionSelectedByCursor;
    uint32_t m_modelVersion;
    uint32_t m_rangeVersion;
    Poincare::Preferences::AngleUnit m_angleUnitVersion;
 };
  virtual ~FunctionApp() = default;
  virtual FunctionStore * functionStore() { return static_cast<FunctionApp::Snapshot *>(snapshot())->functionStore(); }
  virtual InputViewController * inputViewController() = 0;
  void willBecomeInactive() override;

protected:
  FunctionApp(Snapshot * snapshot, ViewController * rootViewController) :
    ExpressionFieldDelegateApp(snapshot, rootViewController)
  {}
  // TextFieldDelegateApp
  bool isAcceptableExpression(const Poincare::Expression expression) override;
};

}

#endif
