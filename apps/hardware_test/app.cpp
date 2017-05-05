#include "app.h"
#include "../apps_container.h"

extern "C" {
#include <assert.h>
}

namespace HardwareTest {

App::App(AppsContainer * container) :
  ::App(container, &m_keyboardController),
  m_keyboardController(&m_modalViewController)
{
}

}
