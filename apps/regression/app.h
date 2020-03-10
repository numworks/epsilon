#ifndef REGRESSION_APP_H
#define REGRESSION_APP_H

#include <escher.h>
#include "../shared/text_field_delegate_app.h"
#include "calculation_controller.h"
#include "graph_controller.h"
#include "regression_controller.h"
#include "store.h"
#include "store_controller.h"

namespace Regression {

class App : public Shared::TextFieldDelegateApp {
public:
  class Descriptor : public ::App::Descriptor {
  public:
    I18n::Message name() override;
    I18n::Message upperName() override;
    const Image * icon() override;
  };
  class Snapshot : public ::App::Snapshot, public TabViewDataSource {
  public:
    Snapshot();
    App * unpack(Container * container) override;
    void reset() override;
    Descriptor * descriptor() override;
    Store * store() { return &m_store; }
    Shared::CurveViewCursor * cursor() { return &m_cursor; }
    int * graphSelectedDotIndex() { return &m_graphSelectedDotIndex; }
    int * selectedSeriesIndex() { return &m_selectedSeriesIndex; }
    uint32_t * modelVersion() { return &m_modelVersion; }
    uint32_t * previousModelsVersions() { return m_store.seriesChecksum(); }
    uint32_t * rangeVersion() { return &m_rangeVersion; }
  private:
    void tidy() override;
    Store m_store;
    Shared::CurveViewCursor m_cursor;
    int m_graphSelectedDotIndex;
    uint32_t m_modelVersion;
    uint32_t m_rangeVersion;
    int m_selectedSeriesIndex;
  };
  static App * app() {
    return static_cast<App *>(Container::activeApp());
  }
  TELEMETRY_ID("Regression");
  RegressionController * regressionController() { return &m_regressionController; }
private:
  App(Snapshot * snapshot, Poincare::Context * parentContext);
  CalculationController m_calculationController;
  AlternateEmptyViewController m_calculationAlternateEmptyViewController;
  ButtonRowController m_calculationHeader;
  GraphController m_graphController;
  AlternateEmptyViewController m_graphAlternateEmptyViewController;
  ButtonRowController m_graphHeader;
  StackViewController m_graphStackViewController;
  StoreController m_storeController;
  ButtonRowController m_storeHeader;
  StackViewController m_storeStackViewController;
  TabViewController m_tabViewController;
  RegressionController m_regressionController;
};

}

#endif
