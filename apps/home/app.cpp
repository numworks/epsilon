#include "app.h"
#include "../apps_container.h"
extern "C" {
#include <assert.h>
}

namespace Home {

App::App(AppsContainer * container) :
  ::App(&m_controller),
  m_controller(Controller(&m_modalViewController, container))
{
  assert(container->appAtIndex(0) == this);
}

}
