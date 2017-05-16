#include "app.h"
#include "../apps_container.h"

extern "C" {
#include <assert.h>
}

namespace HardwareTest {

App * App::Descriptor::build(Container * container) {
  return new App(container, this);
}

App::App(Container * container, Descriptor * descriptor) :
  ::App(container, &m_keyboardController, descriptor),
  m_keyboardController(&m_modalViewController)
{
}

App::Descriptor * App::buildDescriptor() {
  return new App::Descriptor();
}

}
