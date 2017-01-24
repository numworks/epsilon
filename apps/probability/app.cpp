#include "app.h"
#include "probability_icon.h"

namespace Probability {

App::App(Container * container) :
  TextFieldDelegateApp(container, &m_stackViewController, "Probability", ImageStore::ProbabilityIcon),
  m_lawController(LawController(nullptr)),
  m_stackViewController(&m_modalViewController, &m_lawController, true)
{
}

}