#ifndef REGRESSION_APP_H
#define REGRESSION_APP_H

#include <escher/tab_view_controller.h>
#include <escher/tab_view_data_source.h>
#include "../shared/expression_field_delegate_app.h"
#include "calculation_controller.h"
#include "graph_controller.h"
#include "regression_controller.h"
#include "store.h"
#include "store_controller.h"
#include "../shared/shared_app.h"

namespace Regression {

class App : public Shared::ExpressionFieldDelegateApp {
public:
  class Descriptor : public Escher::App::Descriptor {
  public:
    I18n::Message name() const override;
    I18n::Message upperName() const override;
    const Escher::Image * icon() const override;
  };
  class Snapshot : public Shared::SharedApp::Snapshot, public Escher::TabViewDataSource {
  public:
    Snapshot();
    App * unpack(Escher::Container * container) override;
    void reset() override;
    const Descriptor * descriptor() const override;
    Shared::CurveViewCursor * cursor() { return &m_cursor; }
    int * graphSelectedDotIndex() { return &m_graphSelectedDotIndex; }
    int * selectedSeriesIndex() { return &m_selectedSeriesIndex; }
    uint32_t * rangeVersion() { return &m_rangeVersion; }
    Model::Type * regressionTypes() { return m_regressionTypes; }
  private:
    Shared::CurveViewCursor m_cursor;
    int m_graphSelectedDotIndex;
    uint32_t m_rangeVersion;
    int m_selectedSeriesIndex;
    Model::Type m_regressionTypes[Store::k_numberOfSeries];
  };
  static App * app() {
    return static_cast<App *>(Escher::Container::activeApp());
  }
  TELEMETRY_ID("Regression");
  RegressionController * regressionController() { return &m_regressionController; }
  Escher::InputViewController * inputViewController() { return &m_inputViewController; }
  Escher::NestedMenuController * variableBox() override;

private:
  App(Snapshot * snapshot, Poincare::Context * parentContext);
  bool storageWillChangeForRecord(Ion::Storage::Record) override;
  Store m_store;
  CalculationController m_calculationController;
  Escher::AlternateEmptyViewController m_calculationAlternateEmptyViewController;
  Escher::ButtonRowController m_calculationHeader;
  GraphController m_graphController;
  Escher::AlternateEmptyViewController m_graphAlternateEmptyViewController;
  Escher::ButtonRowController m_graphHeader;
  Escher::StackViewController m_graphStackViewController;
  StoreController m_storeController;
  Escher::ButtonRowController m_storeHeader;
  Escher::StackViewController m_storeStackViewController;
  Escher::TabViewController m_tabViewController;
  RegressionController m_regressionController;
  Escher::InputViewController m_inputViewController;
};

}

#endif
