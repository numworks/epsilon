#ifndef SEQUENCE_APP_H
#define SEQUENCE_APP_H

#include <escher.h>
#include <poincare.h>
#include "local_context.h"
#include "sequence_store.h"
#include "graph/graph_controller.h"
#include "list/list_controller.h"
#include "values/values_controller.h"
#include "../shared/function_app.h"

namespace Sequence {

class App : public Shared::FunctionApp {
public:
  App(Container * container, Poincare::Context * context);
  InputViewController * inputViewController() override;
  Poincare::Context * localContext() override;
  const char * XNT() override;
private:
  SequenceStore m_sequenceStore;
  LocalContext m_nContext;
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
