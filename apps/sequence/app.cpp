#include "app.h"
#include "sequence_icon.h"

using namespace Poincare;

namespace Sequence {

App::App(Container * container, Context * context) :
  TextFieldDelegateApp(container, &m_inputViewController, "Suites", "SUITES", ImageStore::SequenceIcon),
  m_sequenceStore(SequenceStore()),
  m_nContext(VariableContext('n', context)),
  m_listController(&m_listHeader, &m_sequenceStore, &m_listHeader),
  m_listHeader(HeaderViewController(nullptr, &m_listController, &m_listController)),
  m_listStackViewController(StackViewController(&m_tabViewController, &m_listHeader)),
  m_graphController(&m_graphAlternateEmptyViewController, &m_sequenceStore, &m_graphHeader),
  m_graphAlternateEmptyViewController(AlternateEmptyViewController(&m_graphHeader, &m_graphController, &m_graphController)),
  m_graphHeader(HeaderViewController(nullptr, &m_graphAlternateEmptyViewController, &m_graphController)),
  m_graphStackViewController(StackViewController(&m_tabViewController, &m_graphHeader)),
  m_valuesController(&m_valuesAlternateEmptyViewController, &m_sequenceStore, &m_valuesHeader),
  m_valuesAlternateEmptyViewController(AlternateEmptyViewController(&m_valuesHeader, &m_valuesController, &m_valuesController)),
  m_valuesHeader(HeaderViewController(nullptr, &m_valuesAlternateEmptyViewController, &m_valuesController)),
  m_valuesStackViewController(StackViewController(&m_tabViewController, &m_valuesHeader)),
  m_tabViewController(&m_inputViewController, &m_listStackViewController, &m_graphStackViewController, &m_valuesStackViewController),
  m_inputViewController(&m_modalViewController, &m_tabViewController, this)
{
}

InputViewController * App::inputViewController() {
  return &m_inputViewController;
}

Context * App::localContext() {
  return &m_nContext;
}

const char * App::XNT() {
  return "n";
}

}
