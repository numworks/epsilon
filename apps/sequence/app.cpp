#include "app.h"
#include "sequence_icon.h"

using namespace Poincare;

namespace Sequence {

App::App(Container * container, Context * context) :
  TextFieldDelegateApp(container, &m_inputViewController, I18n::Message::SequenceApp, I18n::Message::SequenceAppCapital, ImageStore::SequenceIcon),
  m_sequenceStore(SequenceStore()),
  m_nContext(LocalContext(context)),
  m_listController(&m_listFooter, &m_sequenceStore, &m_listHeader, &m_listFooter),
  m_listFooter(ButtonRowController(&m_listHeader, &m_listController, &m_listController, ButtonRowController::Position::Bottom, ButtonRowController::Style::EmbossedGrey)),
  m_listHeader(ButtonRowController(nullptr, &m_listFooter, &m_listController)),
  m_listStackViewController(StackViewController(&m_tabViewController, &m_listHeader)),
  m_graphController(&m_graphAlternateEmptyViewController, &m_sequenceStore, &m_graphHeader),
  m_graphAlternateEmptyViewController(AlternateEmptyViewController(&m_graphHeader, &m_graphController, &m_graphController)),
  m_graphHeader(ButtonRowController(&m_graphStackViewController, &m_graphAlternateEmptyViewController, &m_graphController)),
  m_graphStackViewController(StackViewController(&m_tabViewController, &m_graphHeader)),
  m_valuesController(&m_valuesAlternateEmptyViewController, &m_sequenceStore, &m_valuesHeader),
  m_valuesAlternateEmptyViewController(AlternateEmptyViewController(&m_valuesHeader, &m_valuesController, &m_valuesController)),
  m_valuesHeader(ButtonRowController(nullptr, &m_valuesAlternateEmptyViewController, &m_valuesController)),
  m_valuesStackViewController(StackViewController(&m_tabViewController, &m_valuesHeader)),
  m_tabViewController(&m_inputViewController, &m_listStackViewController, &m_graphStackViewController, &m_valuesStackViewController),
  m_inputViewController(&m_modalViewController, &m_tabViewController, this)
{
}

InputViewController * App::inputViewController() {
  return &m_inputViewController;
}

LocalContext * App::localContext() {
  return &m_nContext;
}

const char * App::XNT() {
  return "n";
}

}
