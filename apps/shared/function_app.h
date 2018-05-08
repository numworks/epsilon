#ifndef SHARED_FUNCTION_APP_H
#define SHARED_FUNCTION_APP_H

#include <poincare.h>
#include "text_field_delegate_app.h"
#include "curve_view_cursor.h"
#include "interval.h"

class AppsContainer;

namespace Shared {

class FunctionApp : public TextFieldDelegateApp {
public:
  class Snapshot : public ::App::Snapshot, public TabViewDataSource {
  public:
    Snapshot();
    CurveViewCursor * cursor();
    uint32_t * modelVersion();
    uint32_t * rangeVersion();
    Poincare::Expression::AngleUnit * angleUnitVersion();
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
    Poincare::Expression::AngleUnit m_angleUnitVersion;
 };
  virtual ~FunctionApp() = default;
  virtual InputViewController * inputViewController() = 0;
  void willBecomeInactive() override;
protected:
  FunctionApp(Container * container, Snapshot * snapshot, ViewController * rootViewController);
};

}

#endif
