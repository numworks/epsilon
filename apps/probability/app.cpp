#include "app.h"
#include "probability_icon.h"

using namespace Shared;

namespace Probability {

App::App(Container * container) :
  TextFieldDelegateApp(container, &m_stackViewController, I18n::Message::ProbaApp, I18n::Message::ProbaAppCapital, ImageStore::ProbabilityIcon),
  m_lawController(nullptr),
  m_stackViewController(&m_modalViewController, &m_lawController)
{
}

}
