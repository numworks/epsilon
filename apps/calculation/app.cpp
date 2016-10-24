#include "app.h"
#include "calculation_icon.h"

namespace Calculation {

App::App(Context * context) :
  ::App("Calcul", ImageStore::CalculationIcon),
  m_globalContext(context),
  m_calculationStore(CalculationStore()),
  m_historyController(HistoryController(&m_editExpressionController, &m_calculationStore)),
  m_editExpressionController(EditExpressionController(this, &m_historyController, &m_calculationStore))
{
}

ViewController * App::rootViewController() {
  return &m_editExpressionController;
}

Context * App::globalContext() {
  return m_globalContext;
}

}
