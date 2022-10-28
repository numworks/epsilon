#ifndef GRAPH_APP_H
#define GRAPH_APP_H

#include <escher/alternate_empty_view_controller.h>
#include "continuous_function_store.h"
#include "graph/graph_controller.h"
#include "list/list_controller.h"
#include "values/values_controller.h"
#include "../shared/continuous_function.h"
#include "../shared/function_app.h"
#include "../shared/interval.h"

namespace Graph {

class App : public Shared::FunctionApp {
public:
  class Descriptor : public Escher::App::Descriptor {
  public:
    I18n::Message name() const override;
    I18n::Message upperName() const override;
    const Escher::Image * icon() const override;
  };

  class Snapshot : public Shared::FunctionApp::Snapshot {
  public:
    using Shared::FunctionApp::Snapshot::Snapshot;
    App * unpack(Escher::Container * container) override;
    void reset() override;
    const Descriptor * descriptor() const override;
    ContinuousFunctionStore * functionStore() override { return &m_functionStore; }
    Shared::InteractiveCurveViewRange * graphRange() { return &m_graphRange; }
    Shared::Interval * intervalForSymbolType(Shared::ContinuousFunctionProperties::SymbolType symbolType) {
      return m_interval + static_cast<size_t>(symbolType);
    }
  private:
    void tidy() override;
    ContinuousFunctionStore m_functionStore;
    Shared::InteractiveCurveViewRange m_graphRange;
    Shared::Interval m_interval[Shared::ContinuousFunctionProperties::k_numberOfSymbolTypes];
  };
  static App * app() {
    return static_cast<App *>(Escher::Container::activeApp());
  }
  Snapshot * snapshot() const {
    return static_cast<Snapshot *>(Escher::App::snapshot());
  }

  TELEMETRY_ID("Graph");
  CodePoint XNT() override;
  Escher::NestedMenuController * variableBox() override;
  bool storageWillChangeForRecord(Ion::Storage::Record) override;
  void storageDidChangeForRecord(Ion::Storage::Record) override;
  ContinuousFunctionStore * functionStore() override { return snapshot()->functionStore(); }
  Shared::Interval * intervalForSymbolType(Shared::ContinuousFunctionProperties::SymbolType symbolType) { return snapshot()->intervalForSymbolType(symbolType); }
  ValuesController * valuesController() override { return &m_valuesController; }
  Escher::InputViewController * inputViewController() override { return &m_inputViewController; }
  ListController * listController() { return &m_listController; }

private:
  App(Snapshot * snapshot);
  ListController m_listController;
  Escher::ButtonRowController m_listFooter;
  Escher::ButtonRowController m_listHeader;
  Escher::StackViewController m_listStackViewController;
  GraphController m_graphController;
  Escher::AlternateEmptyViewController m_graphAlternateEmptyViewController;
  Escher::ButtonRowController m_graphHeader;
  Escher::StackViewController m_graphStackViewController;
  ValuesController m_valuesController;
  FunctionColumnParameterController m_functionParameterController;
  Escher::AlternateEmptyViewController m_valuesAlternateEmptyViewController;
  Escher::ButtonRowController m_valuesHeader;
  Escher::StackViewController m_valuesStackViewController;
  Escher::TabViewController m_tabViewController;
  Escher::InputViewController m_inputViewController;
};

}

#endif
