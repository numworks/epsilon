#include "app.h"
#include "calculation_icon.h"

namespace Calculation {

App::App(Container * container, Context * context) :
  ::App(container, &m_editExpressionController, "Calcul", ImageStore::CalculationIcon),
  ExpressionTextFieldDelegate(),
  m_evaluateContext(EvaluateContext(context, &m_calculationStore)),
  m_calculationStore(CalculationStore()),
  m_historyController(HistoryController(&m_editExpressionController, &m_calculationStore)),
  m_editExpressionController(EditExpressionController(&m_modalViewController, &m_historyController, &m_calculationStore))
{
}

Context * App::evaluateContext() {
  return &m_evaluateContext;
}

}
