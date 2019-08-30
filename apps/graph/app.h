#ifndef GRAPH_APP_H
#define GRAPH_APP_H

#include <escher.h>
#include "cartesian_function_store.h"
#include "graph/graph_controller.h"
#include "list/list_controller.h"
#include "values/values_controller.h"
#include "../shared/function_app.h"

namespace Graph {

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
    Descriptor * descriptor() override;
    CartesianFunctionStore * functionStore() override;
    Shared::InteractiveCurveViewRange * graphRange();
  private:
    void tidy() override;
    CartesianFunctionStore m_functionStore;
    Shared::InteractiveCurveViewRange m_graphRange;
  };
  static App * app() {
    return static_cast<App *>(Container::activeApp());
  }
  CodePoint XNT() override;
  NestedMenuController * variableBoxForInputEventHandler(InputEventHandler * textInput) override;
  CartesianFunctionStore * functionStore() override { return static_cast<CartesianFunctionStore *>(Shared::FunctionApp::functionStore()); }
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
