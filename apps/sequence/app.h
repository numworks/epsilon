#ifndef SEQUENCE_APP_H
#define SEQUENCE_APP_H

#include <escher.h>
#include <poincare.h>
#include "sequence_store.h"
#include "sequence_toolbox.h"
#include "list/list_controller.h"
#include "values/values_controller.h"
#include "../shared/text_field_delegate_app.h"

namespace Sequence {

class App : public Shared::TextFieldDelegateApp {
public:
  App(Container * container, Poincare::Context * context);
  InputViewController * inputViewController();
  Poincare::Context * localContext() override;
  const char * XNT() override;
  SequenceToolbox * sequenceToolbox();
  SequenceStore * sequenceStore();
private:
  SequenceToolbox m_sequenceToolbox;
  SequenceStore m_sequenceStore;
  Poincare::VariableContext m_nContext;
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
