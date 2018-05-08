#include "app.h"
#include "settings_icon.h"
#include "../i18n.h"

namespace Settings {

I18n::Message App::Descriptor::name() {
  return I18n::Message::SettingsApp;
}

I18n::Message App::Descriptor::upperName() {
  return I18n::Message::SettingsAppCapital;
}

const Image * App::Descriptor::icon() {
  return ImageStore::SettingsIcon;
}

App * App::Snapshot::unpack(Container * container) {
  return new App(container, this);
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

App::App(Container * container, Snapshot * snapshot) :
  Shared::TextFieldDelegateApp(container, snapshot, &m_stackViewController),
  m_mainController(&m_stackViewController),
  m_stackViewController(&m_modalViewController, &m_mainController)
{
}

}
