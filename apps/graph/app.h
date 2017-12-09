#ifndef GRAPH_APP_H
#define GRAPH_APP_H

#include <escher.h>
#include <poincare.h>
#include "graph_icon.h"
#include "cartesian_function_store.h"
#include "graph/graph_controller.h"
#include "list/list_controller.h"
#include "values/values_controller.h"
#include "../shared/function_app.h"

namespace Graph {

class App final : public Shared::FunctionApp {
public:
  class Descriptor final : public ::App::Descriptor {
  public:
    I18n::Message name() override {
      return I18n::Message::FunctionApp;
    }
    I18n::Message upperName() override {
      return I18n::Message::FunctionAppCapital;
    }
    const Image * icon() override {
      return ImageStore::GraphIcon;
    }
  };
  class Snapshot final : public Shared::FunctionApp::Snapshot {
  public:
    Snapshot() :
      Shared::FunctionApp::Snapshot::Snapshot(),
      m_functionStore(),
      m_graphRange(&m_cursor) {}
    App * unpack(Container * container) override {
      return new App(container, this);
    }
    void reset() override;
    Descriptor * descriptor() override { return &s_descriptor; }
    CartesianFunctionStore * functionStore() { return &m_functionStore; }
    Shared::InteractiveCurveViewRange * graphRange() { return &m_graphRange; }
  private:
    void tidy() override;
    CartesianFunctionStore m_functionStore;
    Shared::InteractiveCurveViewRange m_graphRange;
    static Descriptor s_descriptor;
  };
  InputViewController * inputViewController() override { return &m_inputViewController; }
  const char * XNT() override { return "x"; }
private:
  App(Container * container, Snapshot * snapshot);
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
