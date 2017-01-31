#include "app.h"
#include "calculation_icon.h"

namespace Calculation {

App::App(Container * container, Context * context, Preferences * preferences) :
  TextFieldDelegateApp(container, &m_editExpressionController, "Calculs", "CALCULS", ImageStore::CalculationIcon),
  m_localContext(LocalContext((GlobalContext *)context, &m_calculationStore)),
  m_preferences(preferences),
  m_calculationStore(CalculationStore()),
  m_historyController(HistoryController(&m_editExpressionController, &m_calculationStore)),
  m_editExpressionController(EditExpressionController(&m_modalViewController, &m_historyController, &m_calculationStore))
{
}

Context * App::localContext() {
  return &m_localContext;
}

Preferences * App::preferences() {
  return m_preferences;
}

}
