#include "app.h"
#include "calcul_icon.h"

namespace Calcul {

App::App() :
  ::App("Calcul", ImageStore::CalculIcon),
  m_calculController(CalculController(this))
{
}

ViewController * App::rootViewController() {
  return &m_calculController;
}

}
