#include "app.h"
#include "images/probability_icon.h"

namespace Probability {

App::App(Container * container, Context * context) :
  ::App(container, &m_stackViewController, "Probability", ImageStore::ProbabilityIcon),
  m_localContext(LocalContext(context)),
  m_lawController(LawController(nullptr, &m_localContext)),
  m_stackViewController(&m_modalViewController, &m_lawController, true)
{
}

Context * App::localContext() {
  return &m_localContext;
}

}