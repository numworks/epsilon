#include "app.h"
#include "settings_icon.h"

namespace Settings {

App::App(Container * container) :
  ::App(container, &m_stackViewController, "Parametre", "PARAMETRE", ImageStore::SettingsIcon),
  m_preference(),
  m_mainController(MainController(nullptr)),
  m_stackViewController(StackViewController(&m_modalViewController, &m_mainController))
{
}

}
