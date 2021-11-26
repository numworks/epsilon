#include "app.h"
#include <apps/i18n.h>
#include <assert.h>

namespace USB {

I18n::Message App::Descriptor::name() {
  return upperName();
}

I18n::Message App::Descriptor::upperName() {
  return I18n::Message::USBAppCapital;
}

App * App::Snapshot::unpack(Container * container) {
  return new (container->currentAppBuffer()) App(this);
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

App::App(Snapshot * snapshot) :
  ::App(snapshot, &m_usbConnectedController)
{
}

bool App::processEvent(Ion::Events::Event e) {
  return false;
}

}
