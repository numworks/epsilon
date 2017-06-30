#include "app.h"
#include "../apps_container.h"

extern "C" {
#include <assert.h>
}

namespace HardwareTest {

App * App::Snapshot::unpack(Container * container) {
  return new App(container, this);
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

App::App(Container * container, Snapshot * snapshot) :
  ::App(container, snapshot, &m_keyboardController),
  m_keyboardController(&m_modalViewController),
  m_USBTestController(nullptr)
{
}

ViewController * App::USBController() {
  return &m_USBTestController;
}

int App::numberOfTimers() {
  return firstResponder() == &m_USBTestController;
}

Timer * App::timerAtIndex(int i) {
  assert(i == 0);
  return &m_USBTestController;
}

bool App::processEvent(Ion::Events::Event e) {
  ::App::processEvent(e);
  return true;
}

}
