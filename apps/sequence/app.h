#ifndef SEQUENCE_APP_H
#define SEQUENCE_APP_H

#include <escher.h>
#include <poincare.h>
#include "local_context.h"
#include "sequence_store.h"
#include "list/list_controller.h"
#include "values/values_controller.h"
#include "../shared/text_field_delegate_app.h"

namespace Sequence {

class App : public Shared::TextFieldDelegateApp {
public:
  App(Container * container, Poincare::Context * context);
  InputViewController * inputViewController();
  LocalContext * localContext() override;
  const char * XNT() override;
private:
  SequenceStore m_sequenceStore;
  LocalContext m_nContext;
  ListController m_listController;
  HeaderViewController m_listHeader;
  StackViewController m_listStackViewController;
  ValuesController m_graphController;
  AlternateEmptyViewController m_graphAlternateEmptyViewController;
  HeaderViewController m_graphHeader;
  StackViewController m_graphStackViewController;
  ValuesController m_valuesController;
  AlternateEmptyViewController m_valuesAlternateEmptyViewController;
  HeaderViewController m_valuesHeader;
  StackViewController m_valuesStackViewController;
  TabViewController m_tabViewController;
  InputViewController m_inputViewController;
};

}

#endif
