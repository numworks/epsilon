#include "app.h"
#include "apps/apps_container.h"
#include "omega_icon.h"
#include "apps/i18n.h"
#include <assert.h>

namespace Omega {

I18n::Message App::Descriptor::name() {
  return I18n::Message::OmegaApp;
}

I18n::Message App::Descriptor::upperName() {
  return I18n::Message::OmegaAppCapital;
}

const Image * App::Descriptor::icon() {
  return ImageStore::OmegaIcon;
}

App::Snapshot::Snapshot()
{
}

App * App::Snapshot::unpack(Container * container) {
  return new App(this);
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

void App::Snapshot::reset() {
}

App::App(Snapshot * snapshot) :
  ::App(snapshot, &m_omegaController),
  m_omegaController(this)
{
}

}
