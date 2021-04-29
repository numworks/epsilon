#include "app.h"
#include <apps/i18n.h>
#include "probability_icon.h"
#include <new>

using namespace Shared;
using namespace Escher;

namespace Probability {

I18n::Message App::Descriptor::name() const {
  return I18n::Message::ProbaApp;
}

I18n::Message App::Descriptor::upperName() const {
  return I18n::Message::ProbaAppCapital;
}

const Image * App::Descriptor::icon() const {
  return ImageStore::ProbabilityIcon;
}

App::Snapshot::Snapshot() :
  m_distribution{},
  m_calculation{},
  m_activePage(Page::Distribution)
{
  initializeDistributionAndCalculation();
}

App::Snapshot::~Snapshot() {
  deleteDistributionAndCalculation();
}

App * App::Snapshot::unpack(Container * container) {
  return new (container->currentAppBuffer()) App(this);
}

static constexpr App::Descriptor sDescriptor;

const App::Descriptor * App::Snapshot::descriptor() const {
  return &sDescriptor;
}

void App::Snapshot::reset() {
  deleteDistributionAndCalculation();
  initializeDistributionAndCalculation();
  m_activePage = Page::Distribution;
}

Distribution * App::Snapshot::distribution() {
  return (Distribution *)m_distribution;
}

Calculation * App::Snapshot::calculation() {
  return (Calculation *)m_calculation;
}

void App::Snapshot::deleteDistributionAndCalculation() {
  distribution()->~Distribution();
  calculation()->~Calculation();
}

void App::Snapshot::initializeDistributionAndCalculation() {
  new(m_distribution) BinomialDistribution();
  new(m_calculation) LeftIntegralCalculation(distribution());
}

void App::Snapshot::setActivePage(Page activePage) {
  m_activePage = activePage;
}

App::Snapshot::Page App::Snapshot::activePage() {
  return m_activePage;
}

App::App(Snapshot * snapshot) :
  TextFieldDelegateApp(snapshot, &m_stackViewController),
  m_calculationController(&m_stackViewController, this, snapshot->distribution(), snapshot->calculation()),
  m_parametersController(&m_stackViewController, this, snapshot->distribution(), &m_calculationController),
  m_distributionController(&m_stackViewController, snapshot->distribution(), &m_parametersController),
  m_stackViewController(&m_modalViewController, &m_distributionController)
{
    m_stackViewController.setupHeadersBorderOverlaping(false, false);
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
