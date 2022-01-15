#ifndef GRAPH_APP_H
#define GRAPH_APP_H

#include <escher.h>
#include "continuous_function_store.h"
#include "graph/graph_controller.h"
#include "list/list_controller.h"
#include "values/values_controller.h"
#include "../shared/function_app.h"
#include "../shared/interval.h"

namespace Graph {

class App : public Shared::FunctionApp {
public:
  class Descriptor : public ::App::Descriptor {
  public:
    I18n::Message name() override;
    I18n::Message upperName() override;
    App::Descriptor::ExaminationLevel examinationLevel() override;
    const Image * icon() override;
  };
  class Snapshot : public Shared::FunctionApp::Snapshot {
  public:
    Snapshot();
    App * unpack(Container * container) override;
    void reset() override;
    Descriptor * descriptor() override;
    ContinuousFunctionStore * functionStore() override { return &m_functionStore; }
    Shared::InteractiveCurveViewRange * graphRange() { return &m_graphRange; }
    Shared::Interval * intervalForType(Shared::ContinuousFunction::PlotType plotType) {
      return m_interval + static_cast<size_t>(plotType);
    }
    bool * shouldDisplayDerivative() { return &m_shouldDisplayDerivative; }
  private:
    void tidy() override;
    ContinuousFunctionStore m_functionStore;
    Shared::InteractiveCurveViewRange m_graphRange;
    Shared::Interval m_interval[Shared::ContinuousFunction::k_numberOfPlotTypes];
    bool m_shouldDisplayDerivative;
  };
  static App * app() {
    return static_cast<App *>(Container::activeApp());
  }
  Snapshot * snapshot() const {
    return static_cast<Snapshot *>(::App::snapshot());
  }
  TELEMETRY_ID("Graph");
  bool XNTCanBeOverriden() const override { return false; }
  CodePoint XNT() override;
  NestedMenuController * variableBoxForInputEventHandler(InputEventHandler * textInput) override;
  ContinuousFunctionStore * functionStore() override { return snapshot()->functionStore(); }
  Shared::Interval * intervalForType(Shared::ContinuousFunction::PlotType plotType) {
    return snapshot()->intervalForType(plotType);
  }
  ValuesController * valuesController() override {
    return &m_valuesController;
  }
  InputViewController * inputViewController() override {
    return &m_inputViewController;
  }
private:
  App(Snapshot * snapshot);
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
