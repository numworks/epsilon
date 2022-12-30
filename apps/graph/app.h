#ifndef GRAPH_APP_H
#define GRAPH_APP_H

#include <escher/alternate_empty_view_controller.h>
#include <apps/shared/continuous_function_store.h>
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
    Shared::ContinuousFunctionStore * functionStore() override { return static_cast<Shared::GlobalContext *>(AppsContainerHelper::sharedAppsContainerGlobalContext())->continuousFunctionStore(); }
    Shared::InteractiveCurveViewRange * graphRange() { return &m_graphRange; }
    Shared::Interval * intervalForSymbolType(Shared::ContinuousFunctionProperties::SymbolType symbolType) {
      return m_interval + static_cast<size_t>(symbolType);
    }
  private:
    void tidy() override;
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
  Shared::ContinuousFunctionStore * functionStore() const override { return snapshot()->functionStore(); }
  Shared::Interval * intervalForSymbolType(Shared::ContinuousFunctionProperties::SymbolType symbolType) { return snapshot()->intervalForSymbolType(symbolType); }
  ValuesController * valuesController() override { return &m_valuesController; }
  Escher::InputViewController * inputViewController() override { return &m_inputViewController; }
  ListController * listController() { return &m_listController; }
  GraphController * graphController() { return &m_graphController; }

private:
  App(Snapshot * snapshot);
  ListController m_listController;
  GraphController m_graphController;
  ValuesController m_valuesController;
  FunctionParameterController m_functionParameterController;
};

}

#endif
