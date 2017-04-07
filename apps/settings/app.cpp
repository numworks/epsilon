#include "app.h"
#include "settings_icon.h"
#include "../i18n.h"

namespace Settings {

App::App(Container * container) :
  Shared::ResettableApp(container, &m_stackViewController, I18n::Message::SettingsApp, I18n::Message::SettingsAppCapital, ImageStore::SettingsIcon),
  m_mainController(MainController(nullptr)),
  m_stackViewController(StackViewController(&m_modalViewController, &m_mainController))
{
}

}
