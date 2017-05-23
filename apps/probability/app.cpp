#include "app.h"
#include "../i18n.h"
#include "probability_icon.h"
#include <new>

using namespace Shared;

namespace Probability {

I18n::Message App::Descriptor::name() {
  return I18n::Message::ProbaApp;
}

I18n::Message App::Descriptor::upperName() {
  return I18n::Message::ProbaAppCapital;
}

const Image * App::Descriptor::icon() {
  return ImageStore::ProbabilityIcon;
}

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

App * App::Snapshot::unpack(Container * container) {
  return new App(container, this);
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

void App::Snapshot::reset() {
  law()->~Law();
  new(m_law) BinomialLaw();
  calculation()->~Calculation();
  new(m_calculation) LeftIntegralCalculation();
  m_activePage = Page::Law;
}

Law * App::Snapshot::law() {
  return (Law *)m_law;
}

Calculation * App::Snapshot::calculation() {
  return (Calculation *)m_calculation;
}

void App::Snapshot::setActivePage(Page activePage) {
  m_activePage = activePage;
}

App::Snapshot::Page App::Snapshot::activePage() {
  return m_activePage;
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
