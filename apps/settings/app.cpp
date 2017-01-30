#include "app.h"
#include "settings_icon.h"

namespace Settings {

App::App(Container * container, Preference * preference) :
  ::App(container, &m_stackViewController, "Parametre", "PARAMETRE", ImageStore::SettingsIcon),
  m_mainController(MainController(nullptr, preference)),
  m_stackViewController(StackViewController(&m_modalViewController, &m_mainController))
{
}

}
