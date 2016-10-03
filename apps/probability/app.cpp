#include "app.h"
#include "probability_icon.h"

Probability::App::App() :
  ::App("Probability", ImageStore::ProbabilityIcon),
  m_lawController(LawController(nullptr)),
  m_parametersController(ParametersController(nullptr)),
  m_stackViewController(this, &m_lawController, true)
{
}

void Probability::App::setLaw(Law l) {
  //m_parametersController.setLaw(thePickedLaw);
  m_stackViewController.push(&m_parametersController);
}

ViewController * Probability::App::rootViewController() {
  return &m_stackViewController;
}
