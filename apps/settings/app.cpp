#include "app.h"
#include "settings_icon.h"
#include "../i18n.h"

namespace Settings {

App * App::Descriptor::build(Container * container) {
  return new App(container, this);
}

I18n::Message App::Descriptor::name() {
  return I18n::Message::SettingsApp;
}

I18n::Message App::Descriptor::upperName() {
  return I18n::Message::SettingsAppCapital;
}

const Image * App::Descriptor::icon() {
  return ImageStore::SettingsIcon;
}

App::App(Container * container, Descriptor * descriptor) :
  ::App(container, &m_stackViewController, descriptor, I18n::Message::Warning),
  m_mainController(&m_stackViewController),
  m_stackViewController(&m_modalViewController, &m_mainController)
{
}

App::Descriptor * App::buildDescriptor() {
  return new App::Descriptor();
}

}
