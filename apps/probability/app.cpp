#include "app.h"
#include "../i18n.h"
#include <new>

using namespace Shared;

namespace Probability {

App::Descriptor App::Snapshot::s_descriptor;

App::Snapshot::Snapshot() :
  m_law{},
  m_calculation{},
  m_activePage(Page::Law)
{
  new(m_law) BinomialLaw();
  new(m_calculation) LeftIntegralCalculation();
  calculation()->setLaw(law());
}

App::Snapshot::~Snapshot() {
  law()->~Law();
  calculation()->~Calculation();
}

void App::Snapshot::reset() {
  law()->~Law();
  new(m_law) BinomialLaw();
  calculation()->~Calculation();
  new(m_calculation) LeftIntegralCalculation();
  m_activePage = Page::Law;
}

App::App(Container * container, Snapshot * snapshot) :
  TextFieldDelegateApp(container, snapshot, &m_stackViewController),
  m_calculationController(&m_stackViewController, snapshot->law(), snapshot->calculation()),
  m_parametersController(&m_stackViewController, snapshot->law(), &m_calculationController),
  m_lawController(&m_stackViewController, snapshot->law(), &m_parametersController),
  m_stackViewController(&m_modalViewController, &m_lawController)
{
    switch (snapshot->activePage()) {
    case Snapshot::Page::Parameters:
      m_stackViewController.push(&m_parametersController, KDColorWhite, Palette::PurpleBright, Palette::PurpleBright);
      break;
    case Snapshot::Page::Calculations:
      m_stackViewController.push(&m_parametersController, KDColorWhite, Palette::PurpleBright, Palette::PurpleBright);
      m_stackViewController.push(&m_calculationController, KDColorWhite, Palette::SubTab, Palette::SubTab);
    default:
      break;
  }
}


}
