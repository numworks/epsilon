#ifndef SEQUENCE_APP_H
#define SEQUENCE_APP_H

#include "../shared/sequence_context.h"
#include "../shared/sequence_store.h"
#include "graph/graph_controller.h"
#include "graph/curve_view_range.h"
#include "list/list_controller.h"
#include "values/values_controller.h"
#include "../shared/function_app.h"
#include "../shared/interval.h"
#include "../shared/global_context.h"
#include <apps/apps_container_helper.h>

namespace Sequence {

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
    Snapshot();
    App * unpack(Escher::Container * container) override;
    void reset() override;
    const Descriptor * descriptor() const override;
    Shared::SequenceStore * functionStore() override { return static_cast<Shared::GlobalContext *>(AppsContainerHelper::sharedAppsContainerGlobalContext())->sequenceStore(); }
    CurveViewRange * graphRange() { return &m_graphRange; }
    Shared::Interval * interval() { return &m_interval; }
    bool intervalModifiedByUser() { return m_intervalModifiedByUser; }
    void setIntervalModifiedByUser(bool intervalModifiedByUser) { m_intervalModifiedByUser = intervalModifiedByUser; }
  private:
    void tidy() override;
    CurveViewRange m_graphRange;
    Shared::Interval m_interval;
    bool m_intervalModifiedByUser;
  };
  static App * app() {
    return static_cast<App *>(Escher::Container::activeApp());
  }
  Snapshot * snapshot() const {
    return static_cast<Snapshot *>(Escher::App::snapshot());
  }
  TELEMETRY_ID("Sequence");
  // TODO: override variableBox to lock sequence in the variable box once they appear there
  // NestedMenuController * variableBox(InputEventHandler * textInput) override;
  CodePoint XNT() override { return 'n'; }
  Shared::SequenceContext * localContext() override { return static_cast<Shared::GlobalContext *>(AppsContainerHelper::sharedAppsContainerGlobalContext())->sequenceContext(); }
  Shared::SequenceStore * functionStore() override { return static_cast<Shared::GlobalContext *>(AppsContainerHelper::sharedAppsContainerGlobalContext())->sequenceStore(); }
  Shared::Interval * interval() { return snapshot()->interval(); }
  ValuesController * valuesController() override {
    return &m_valuesController;
  }
  Escher::InputViewController * inputViewController() override {
    return &m_inputViewController;
  }
  bool isAcceptableExpression(const Poincare::Expression expression) override;

private:
  App(Snapshot * snapshot);
  ListController m_listController;
  GraphController m_graphController;
  ValuesController m_valuesController;
};

}

#endif
