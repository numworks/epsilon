#ifndef SHARED_FUNCTION_APP_H
#define SHARED_FUNCTION_APP_H

#include "expression_field_delegate_app.h"
#include "function_graph_controller.h"
#include "function_store.h"
#include "curve_view_cursor.h"
#include "values_controller.h"

namespace Shared {

class FunctionApp : public ExpressionFieldDelegateApp {
public:
  class Snapshot : public ::App::Snapshot, public TabViewDataSource {
  public:
    Snapshot();
    CurveViewCursor * cursor() { return &m_cursor; }
    uint32_t * modelVersion() { return &m_modelVersion; }
    uint32_t * previousModelsVersions() { return m_previousModelsVersions; }
    uint32_t * rangeVersion() { return &m_rangeVersion; }
    Poincare::Preferences::AngleUnit * angleUnitVersion() { return &m_angleUnitVersion; }
    virtual FunctionStore * functionStore() = 0;
    int * indexFunctionSelectedByCursor() { return &m_indexFunctionSelectedByCursor; }
    void reset() override;
    void storageDidChangeForRecord(const Ion::Storage::Record record) override;
  protected:
    CurveViewCursor m_cursor;
  private:
    int m_indexFunctionSelectedByCursor;
    uint32_t m_modelVersion;
    uint32_t m_previousModelsVersions[FunctionGraphController::sNumberOfMemoizedModelVersions];
    uint32_t m_rangeVersion;
    Poincare::Preferences::AngleUnit m_angleUnitVersion;
  };
  static FunctionApp * app() {
    return static_cast<FunctionApp *>(Container::activeApp());
  }
  virtual ~FunctionApp() = default;
  Snapshot * snapshot() const {
    return static_cast<Snapshot *>(::App::snapshot());
  }
  virtual FunctionStore * functionStore() { return snapshot()->functionStore(); }
  virtual ValuesController * valuesController() = 0;
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
