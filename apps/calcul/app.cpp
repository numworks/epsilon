#include "app.h"
#include "calcul_icon.h"

namespace Calcul {

App::App(Context * context) :
  ::App("Calcul", ImageStore::CalculIcon),
  m_globalContext(context),
  m_historyController(HistoryController(this))
{
}

ViewController * App::rootViewController() {
  return &m_historyController;
}

Context * App::globalContext() {
  return m_globalContext;
}

}
