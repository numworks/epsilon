#ifndef SHARED_STORAGE_FUNCTION_APP_H
#define SHARED_STORAGE_FUNCTION_APP_H

#include "expression_field_delegate_app.h"
#include "storage_function_store.h"
#include "curve_view_cursor.h"
#include "interval.h"

class AppsContainer;

namespace Shared {

class StorageFunctionApp : public ExpressionFieldDelegateApp {
public:
  class Snapshot : public ::App::Snapshot, public TabViewDataSource {
  public:
    Snapshot();
    CurveViewCursor * cursor();
    uint32_t * modelVersion();
    uint32_t * rangeVersion();
    Poincare::Preferences::AngleUnit * angleUnitVersion();
    Interval * interval();
    int * indexFunctionSelectedByCursor();
    void reset() override;
  protected:
    CurveViewCursor m_cursor;
    Interval m_interval;
  private:
    int m_indexFunctionSelectedByCursor;
    uint32_t m_modelVersion;
    uint32_t m_rangeVersion;
    Poincare::Preferences::AngleUnit m_angleUnitVersion;
 };
  virtual ~StorageFunctionApp() = default;
  virtual StorageFunctionStore * functionStore() = 0;
  virtual InputViewController * inputViewController() = 0;
  void willBecomeInactive() override;
protected:
  StorageFunctionApp(Container * container, Snapshot * snapshot, ViewController * rootViewController);
  bool isAcceptableExpression(const Poincare::Expression expression, Responder * responder) override;
};

}

#endif
