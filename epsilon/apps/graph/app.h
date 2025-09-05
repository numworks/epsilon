#pragma once

#include <apps/apps_container_helper.h>
#include <apps/shared/continuous_function.h>
#include <apps/shared/continuous_function_store.h>
#include <apps/shared/function_app.h>
#include <apps/shared/interval.h>
#include <escher/alternate_empty_view_controller.h>

#include "graph/graph_controller.h"
#include "list/list_controller.h"
#include "shared/global_context.h"
#include "values/values_controller.h"

namespace Graph {

class App : public Shared::FunctionApp {
 public:
  class Descriptor : public Escher::App::Descriptor {
   public:
    I18n::Message name() const override;
    I18n::Message upperName() const override;
    const Escher::Image* icon() const override;
  };

  class Snapshot : public Shared::FunctionApp::Snapshot {
   public:
    using Shared::FunctionApp::Snapshot::Snapshot;
    App* unpack(Escher::Container* container) override;
    void reset() override;
    void tidy() override;
    const Descriptor* descriptor() const override;
    Shared::InteractiveCurveViewRange* graphRange() { return &m_graphRange; }
    Shared::Interval* intervalForSymbolType(
        Shared::ContinuousFunctionProperties::SymbolType symbolType) {
      return m_interval + static_cast<size_t>(symbolType);
    }

   private:
    Shared::InteractiveCurveViewRange m_graphRange;
    Shared::Interval m_interval
        [Shared::ContinuousFunctionProperties::k_numberOfVariableSymbolTypes];
  };
  static App* app() { return static_cast<App*>(Escher::App::app()); }
  Snapshot* snapshot() {
    return static_cast<Snapshot*>(Escher::App::snapshot());
  }
  const Snapshot* snapshot() const {
    return static_cast<const Snapshot*>(Escher::App::snapshot());
  }

  Shared::ContinuousFunctionStore& functionStore() override {
    return Shared::GlobalContextAccessor::ContinuousFunctionStore();
  }
  const Shared::ContinuousFunctionContext& functionContext() const override {
    return Shared::GlobalContextAccessor::ContinuousFunctionContext();
  };

  Shared::Interval* intervalForSymbolType(
      Shared::ContinuousFunctionProperties::SymbolType symbolType) {
    return snapshot()->intervalForSymbolType(symbolType);
  }
  ValuesController* valuesController() override {
    return &m_tabs.tab<ValuesTab>()->m_valuesController;
  }
  ListController* listController() {
    return &m_tabs.tab<ListTab>()->m_listController;
  }
  GraphController* graphController() {
    return &m_tabs.tab<GraphTab>()->m_graphController;
  }

 private:
  App(Snapshot* snapshot);

  struct ListTab : public Shared::FunctionApp::ListTab {
    constexpr static I18n::Message k_title = I18n::Message::FunctionTab;
    ListTab();
    ListController m_listController;
  };
  struct GraphTab : public Shared::FunctionApp::GraphTab {
    GraphTab();
    GraphController m_graphController;
  };
  struct ValuesTab : public Shared::FunctionApp::ValuesTab {
    ValuesTab();
    ValuesController m_valuesController;
  };

  FunctionParameterController m_functionParameterController;
  DerivativeColumnParameterController m_derivativeColumnParameterController;
  Shared::CachesContainer m_cachesContainer;
  Escher::TabUnion<ListTab, GraphTab, ValuesTab> m_tabs;
};

}  // namespace Graph
