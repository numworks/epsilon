#include "app.h"
#include "../apps_container.h"
#include "../i18n.h"

extern "C" {
#include <assert.h>
}

namespace Home {

App::App(AppsContainer * container) :
  ::App(container, &m_controller, I18n::Message::Apps, I18n::Message::AppsCapital, nullptr, I18n::Message::Warning),
  m_controller(Controller(&m_modalViewController, container))
{
  assert(container->appAtIndex(0) == this);
}

}
