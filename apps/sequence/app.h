#ifndef SEQUENCE_APP_H
#define SEQUENCE_APP_H

#include <escher.h>
#include "sequence_context.h"
#include "sequence_store.h"
#include "graph/graph_controller.h"
#include "graph/curve_view_range.h"
#include "list/list_controller.h"
#include "values/values_controller.h"
#include "../shared/function_app.h"
#include "../shared/interval.h"

namespace Sequence {

class App : public Shared::FunctionApp {
public:
  class Descriptor : public ::App::Descriptor {
  public:
    I18n::Message name() override;
    I18n::Message upperName() override;
    const Image * icon() override;
  };
  class Snapshot : public Shared::FunctionApp::Snapshot {
  public:
    Snapshot();
    App * unpack(Container * container) override;
    void reset() override;
    Descriptor * descriptor() override;
    SequenceStore * functionStore() override { return &m_sequenceStore; }
    CurveViewRange * graphRange() { return &m_graphRange; }
    Shared::Interval * interval() { return &m_interval; }
  private:
    void tidy() override;
    SequenceStore m_sequenceStore;
    CurveViewRange m_graphRange;
    Shared::Interval m_interval;
  };
  static App * app() {
    return static_cast<App *>(Container::activeApp());
  }
  Snapshot * snapshot() const {
    return static_cast<Snapshot *>(::App::snapshot());
  }
  TELEMETRY_ID("Sequence");
  // TODO: override variableBoxForInputEventHandler to lock sequence in the variable box once they appear there
  // NestedMenuController * variableBoxForInputEventHandler(InputEventHandler * textInput) override;
  CodePoint XNT() override { return 'n'; }
  SequenceContext * localContext() override;
  SequenceStore * functionStore() override { return snapshot()->functionStore(); }
  Shared::Interval * interval() { return snapshot()->interval(); }
  ValuesController * valuesController() override {
    return &m_valuesController;
  }
  InputViewController * inputViewController() override {
    return &m_inputViewController;
  }
private:
  App(Snapshot * snapshot);
  SequenceContext m_sequenceContext;
  ListController m_listController;
  ButtonRowController m_listFooter;
  ButtonRowController m_listHeader;
  StackViewController m_listStackViewController;
  GraphController m_graphController;
  AlternateEmptyViewController m_graphAlternateEmptyViewController;
  ButtonRowController m_graphHeader;
  StackViewController m_graphStackViewController;
  ValuesController m_valuesController;
  AlternateEmptyViewController m_valuesAlternateEmptyViewController;
  ButtonRowController m_valuesHeader;
  StackViewController m_valuesStackViewController;
  TabViewController m_tabViewController;
  InputViewController m_inputViewController;
};

}

#endif
