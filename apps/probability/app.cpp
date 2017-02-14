#include "app.h"
#include "probability_icon.h"

using namespace Shared;

namespace Probability {

App::App(Container * container) :
  TextFieldDelegateApp(container, &m_stackViewController, "Probabilites", "PROBABILITES", ImageStore::ProbabilityIcon),
  m_lawController(LawController(nullptr)),
  m_stackViewController(&m_modalViewController, &m_lawController)
{
}

}