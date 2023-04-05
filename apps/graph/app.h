#ifndef GRAPH_APP_H
#define GRAPH_APP_H

#include <apps/apps_container_helper.h>
#include <apps/shared/continuous_function.h>
#include <apps/shared/continuous_function_store.h>
#include <apps/shared/function_app.h>
#include <apps/shared/interval.h>
#include <escher/alternate_empty_view_controller.h>

#include "graph/graph_controller.h"
#include "list/list_controller.h"
#include "shared/function_toolbox.h"
#include "values/values_controller.h"

namespace Graph {

class App : public Shared::FunctionApp {
 public:
  class Descriptor : public Escher::App::Descriptor {
   public:
    I18n::Message name() const override;
    I18n::Message upperName() const override;
    const Escher::Image *icon() const override;
  };

  class Snapshot : public Shared::FunctionApp::Snapshot {
   public:
    using Shared::FunctionApp::Snapshot::Snapshot;
    App *unpack(Escher::Container *container) override;
    void reset() override;
    const Descriptor *descriptor() const override;
    Shared::ContinuousFunctionStore *functionStore() override {
      return static_cast<Shared::GlobalContext *>(
                 AppsContainerHelper::sharedAppsContainerGlobalContext())
          ->continuousFunctionStore;
    }
    Shared::InteractiveCurveViewRange *graphRange() { return &m_graphRange; }
    Shared::Interval *intervalForSymbolType(
        Shared::ContinuousFunctionProperties::SymbolType symbolType) {
      return m_interval + static_cast<size_t>(symbolType);
    }

   private:
    void tidy() override;
    Shared::InteractiveCurveViewRange m_graphRange;
    Shared::Interval
        m_interval[Shared::ContinuousFunctionProperties::k_numberOfSymbolTypes];
  };
  static App *app() {
    return static_cast<App *>(Escher::Container::activeApp());
  }
  Snapshot *snapshot() const {
    return static_cast<Snapshot *>(Escher::App::snapshot());
  }

  TELEMETRY_ID("Graph");
  CodePoint XNT() override;
  Shared::ContinuousFunctionStore *functionStore() const override {
    return snapshot()->functionStore();
  }
  Shared::Interval *intervalForSymbolType(
      Shared::ContinuousFunctionProperties::SymbolType symbolType) {
    return snapshot()->intervalForSymbolType(symbolType);
  }
  ValuesController *valuesController() override {
    return &m_tabs.tab<ValuesTab>()->m_valuesController;
  }
  ListController *listController() {
    return &m_tabs.tab<ListTab>()->m_listController;
  }
  GraphController *graphController() {
    return &m_tabs.tab<GraphTab>()->m_graphController;
  }
  FunctionParameterController *parameterController() {
    return &m_functionParameterController;
  }
  FunctionToolbox *functionToolbox() { return &m_functionToolbox; }

 private:
  App(Snapshot *snapshot);

  struct ListTab : public Shared::FunctionApp::ListTab {
    static constexpr I18n::Message k_title = I18n::Message::FunctionTab;
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
  FunctionToolbox m_functionToolbox;
  Escher::TabUnion<ListTab, GraphTab, ValuesTab> m_tabs;
};

}  // namespace Graph

#endif
