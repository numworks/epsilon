#include "app.h"
#include "probability_icon.h"

Probability::App::App(Container * container) :
  ::App(container, &m_stackViewController, "Probability", ImageStore::ProbabilityIcon),
  m_lawController(LawController(nullptr)),
  m_stackViewController(&m_modalViewController, &m_lawController, true)
{
}
