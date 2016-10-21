#include "app.h"
#include "calcul_icon.h"

namespace Calcul {

App::App(Context * context) :
  ::App("Calcul", ImageStore::CalculIcon),
  m_globalContext(context),
  m_historyController(HistoryController(&m_editExpressionController)),
  m_editExpressionController(EditExpressionController(this, &m_historyController))
{
}

ViewController * App::rootViewController() {
  return &m_editExpressionController;
}

Context * App::globalContext() {
  return m_globalContext;
}

}
