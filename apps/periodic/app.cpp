#include "app.h"
#include "elements_data_base.h"
#include "../apps_container.h"
#include "../settings/settings_icon.h"
#include <apps/i18n.h>
#include <escher/palette.h>

namespace Periodic {

// App::Descriptor

I18n::Message App::Descriptor::name() const {
  return I18n::Message::PeriodicApp;
}

I18n::Message App::Descriptor::upperName() const {
  return I18n::Message::PeriodicAppCapital;
}

const Escher::Image * App::Descriptor::icon() const {
  // TODO
  return ImageStore::SettingsIcon;
}

// App::Snapshot

App * App::Snapshot::unpack(Escher::Container * container) {
  return new (container->currentAppBuffer()) App(this);
}

const App::Descriptor * App::Snapshot::descriptor() const {
  constexpr static Descriptor s_descriptor;
  return &s_descriptor;
}

// App
App::App(Snapshot * snapshot) :
  Shared::TextFieldDelegateApp(snapshot, &m_mainController),
  m_mainController(&m_modalViewController)
{}

}
