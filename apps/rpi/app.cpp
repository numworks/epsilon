#include "app.h"
#include "../apps_container.h"
#include "rpi_icon.h"
#include "../i18n.h"
#include <ion/console.h>
#include <ion/rpi.h>

namespace Rpi {

I18n::Message App::Descriptor::name() {
  return I18n::Message::RpiApp;
}

I18n::Message App::Descriptor::upperName() {
  return I18n::Message::RpiAppCapital;
}

const Image * App::Descriptor::icon() {
  return ImageStore::RpiIcon;
}

App * App::Snapshot::unpack(Container * container) {
  return new (container->currentAppBuffer()) App(container, this);
}

void App::Snapshot::reset() {
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

void App::didBecomeActive(Window * window) {
  ::App::didBecomeActive(window);
  Ion::Rpi::transferControl();
  m_appsContainer->switchTo(m_appsContainer->appSnapshotAtIndex(0));
}

void App::willBecomeInactive() {
  ::App::willBecomeInactive();
}


bool App::processEvent(Ion::Events::Event e) {
  return ::App::processEvent(e);
}

App::App(Container * container, Snapshot * snapshot) :
  ::App(container, snapshot, &m_rpiController),
  m_appsContainer((AppsContainer*) container)
{
}

}
