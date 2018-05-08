#include "app.h"
#include "../apps_container.h"
#include <assert.h>

namespace USB {

I18n::Message App::Descriptor::name() {
  return upperName();
}

I18n::Message App::Descriptor::upperName() {
  return I18n::Message::USBAppCapital;
}

App * App::Snapshot::unpack(Container * container) {
  return new App(container, this);
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

App::App(Container * container, Snapshot * snapshot) :
  ::App(container, snapshot, &m_usbConnectedController)
{
}

bool App::processEvent(Ion::Events::Event e) {
  return false;
}

}
