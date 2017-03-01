#include "app.h"
#include "graph_icon.h"

using namespace Poincare;
using namespace Shared;

namespace Graph {

App::App(Container * container, Context * context) :
  TextFieldDelegateApp(container, &m_inputViewController, "Fonctions", "FONCTIONS", ImageStore::GraphIcon),
  m_functionStore(CartesianFunctionStore()),
  m_xContext(VariableContext('x', context)),
  m_listController(ListController(&m_listFooter, &m_functionStore, &m_listHeader, &m_listFooter)),
  m_listFooter(ButtonRowController(&m_listHeader, &m_listController, &m_listController, ButtonRowController::Position::Bottom, ButtonRowController::Style::EmbossedGrey)),
  m_listHeader(ButtonRowController(&m_listStackViewController, &m_listFooter, &m_listController)),
  m_listStackViewController(StackViewController(&m_tabViewController, &m_listHeader)),
  m_graphController(GraphController(&m_graphAlternateEmptyViewController, &m_functionStore, &m_graphHeader)),
  m_graphAlternateEmptyViewController(AlternateEmptyViewController(&m_graphHeader, &m_graphController, &m_graphController)),
  m_graphHeader(ButtonRowController(&m_graphStackViewController, &m_graphAlternateEmptyViewController, &m_graphController)),
  m_graphStackViewController(StackViewController(&m_tabViewController, &m_graphHeader)),
  m_valuesController(&m_valuesAlternateEmptyViewController, &m_functionStore, &m_valuesHeader),
  m_valuesAlternateEmptyViewController(AlternateEmptyViewController(&m_valuesHeader, &m_valuesController, &m_valuesController)),
  m_valuesHeader(ButtonRowController(&m_valuesStackViewController, &m_valuesAlternateEmptyViewController, &m_valuesController)),
  m_valuesStackViewController(StackViewController(&m_tabViewController, &m_valuesHeader)),
  m_tabViewController(&m_inputViewController, &m_listStackViewController, &m_graphStackViewController, &m_valuesStackViewController),
  m_inputViewController(&m_modalViewController, &m_tabViewController, this)
{
}

InputViewController * App::inputViewController() {
  return &m_inputViewController;
}

Context * App::localContext() {
  return &m_xContext;
}

}
