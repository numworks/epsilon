#include "app.h"
#include "graph_icon.h"

namespace Graph {

App::App(::Context * context) :
  ::App("Graph", ImageStore::GraphIcon),
  m_functionStore(FunctionStore()),
  m_globalContext(context),
  m_evaluateContext(EvaluateContext(context)),
  m_listController(ListController(nullptr, &m_functionStore)),
  m_listStackViewController(StackViewController(&m_tabViewController, &m_listController)),
  m_graphController(GraphController(nullptr, &m_functionStore, &m_evaluateContext)),
  m_valuesController(nullptr, &m_functionStore, &m_evaluateContext),
  m_valuesStackViewController(StackViewController(&m_tabViewController, &m_valuesController)),
  m_tabViewController(&m_inputViewController, &m_listStackViewController, &m_graphController, &m_valuesStackViewController),
  m_inputViewController(this, &m_tabViewController)
{
  Function * function = m_functionStore.addEmptyFunction();
  function->setContent("(x-1)*(x+1)*x");
  function = m_functionStore.addEmptyFunction();
  function->setContent("x*x");
  function = m_functionStore.addEmptyFunction();
  function->setContent("3");
  function = m_functionStore.addEmptyFunction();
  function->setContent("x*x*x");
  function = m_functionStore.addEmptyFunction();
  function->setContent("1/(1+1/x)");
  function = m_functionStore.addEmptyFunction();
  function->setContent("1/(1+1/(1/x))");
}

ViewController * App::rootViewController() {
  return &m_inputViewController;
}

InputViewController * App::inputViewController() {
  return &m_inputViewController;
}

Context * App::globalContext() {
  return m_globalContext;
}

}
