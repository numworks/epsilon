#include "app.h"
#include "settings_icon.h"

namespace Settings {

App::App(Container * container, Preferences * preferences) :
  ::App(container, &m_stackViewController, "Parametre", "PARAMETRE", ImageStore::SettingsIcon),
  m_mainController(MainController(nullptr, preferences)),
  m_stackViewController(StackViewController(&m_modalViewController, &m_mainController))
{
}

}
