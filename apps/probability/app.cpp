#include "app.h"

Probability::App::App() :
  ::App(),
  m_lawController(LawController(nullptr)),
  m_stackViewController(this)
{
  m_stackViewController.push(&m_lawController);
}

ViewController * Probability::App::rootViewController() {
  return &m_stackViewController;
}
