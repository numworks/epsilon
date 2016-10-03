#include "app.h"
#include "../apps_container.h"
extern "C" {
#include <assert.h>
}

namespace Home {

App::App(AppsContainer * container) :
  ::App(),
  m_controller(Controller(this, container))
{
  assert(container->appAtIndex(0) == this);
}

ViewController * App::rootViewController() {
  return &m_controller;
}

}
