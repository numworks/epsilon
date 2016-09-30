#include "app.h"

namespace Home {

App::App() :
  ::App(),
  m_controller(Controller(this))
{
}

ViewController * App::rootViewController() {
  return &m_controller;
}

}
