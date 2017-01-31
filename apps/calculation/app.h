#ifndef CALCULATION_APP_H
#define CALCULATION_APP_H

#include "edit_expression_controller.h"
#include "local_context.h"
#include "history_controller.h"
#include "../text_field_delegate_app.h"
#include "../preferences.h"
#include <escher.h>

namespace Calculation {

class App : public TextFieldDelegateApp {
public:
  App(Container * container, Context * context, Preferences * preferences);
  Context * localContext() override;
  Preferences * preferences();
private:
  LocalContext m_localContext;
  Preferences * m_preferences;
  CalculationStore m_calculationStore;
  HistoryController m_historyController;
  EditExpressionController m_editExpressionController;
};

}

#endif
