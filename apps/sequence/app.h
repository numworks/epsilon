#ifndef SEQUENCE_APP_H
#define SEQUENCE_APP_H

#include <apps/apps_container_helper.h>

#include "../shared/function_app.h"
#include "../shared/global_context.h"
#include "../shared/interval.h"
#include "../shared/sequence_context.h"
#include "../shared/sequence_store.h"
#include "graph/curve_view_range.h"
#include "graph/graph_controller.h"
#include "list/list_controller.h"
#include "values/values_controller.h"

namespace Sequence {

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
    Snapshot();
    App *unpack(Escher::Container *container) override;
    void resetInterval();
    void updateInterval();
    void reset() override;
    const Descriptor *descriptor() const override;
    Shared::SequenceStore *functionStore() override {
      return static_cast<Shared::GlobalContext *>(
                 AppsContainerHelper::sharedAppsContainerGlobalContext())
          ->sequenceStore;
    }
    CurveViewRange *graphRange() { return &m_graphRange; }
    Shared::Interval *interval() { return &m_interval; }
    bool intervalModifiedByUser() { return m_intervalModifiedByUser; }
    void setIntervalModifiedByUser(bool intervalModifiedByUser) {
      m_intervalModifiedByUser = intervalModifiedByUser;
    }

   private:
    void tidy() override;
    CurveViewRange m_graphRange;
    Shared::Interval m_interval;
    bool m_intervalModifiedByUser;
  };
  static App *app() {
    return static_cast<App *>(Escher::Container::activeApp());
  }
  Snapshot *snapshot() const {
    return static_cast<Snapshot *>(Escher::App::snapshot());
  }
  TELEMETRY_ID("Sequence");
  // TODO: override variableBox to lock sequence in the variable box once they
  // appear there NestedMenuController * variableBox(InputEventHandler *
  // textInput) override;
  CodePoint XNT() override { return 'n'; }
  Shared::SequenceContext *localContext() override {
    return static_cast<Shared::GlobalContext *>(
               AppsContainerHelper::sharedAppsContainerGlobalContext())
        ->sequenceContext();
  }
  Shared::SequenceStore *functionStore() const override {
    return static_cast<Shared::GlobalContext *>(
               AppsContainerHelper::sharedAppsContainerGlobalContext())
        ->sequenceStore;
  }
  Shared::Interval *interval() { return snapshot()->interval(); }
  ValuesController *valuesController() override {
    return &m_tabs.tab<ValuesTab>()->m_valuesController;
  }
  bool isAcceptableExpression(const Poincare::Expression expression) override;

 private:
  App(Snapshot *snapshot) : FunctionApp(snapshot, &m_tabs, ListTab::k_title) {}

  struct ListTab : public Shared::FunctionApp::ListTab {
    static constexpr I18n::Message k_title = I18n::Message::SequenceTab;
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

  Escher::TabUnion<ListTab, GraphTab, ValuesTab> m_tabs;
};

}  // namespace Sequence

#endif
