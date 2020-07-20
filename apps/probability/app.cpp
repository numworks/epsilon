#include "app.h"
#include <apps/i18n.h>
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

App::Descriptor::ExaminationLevel App::Descriptor::examinationLevel() {
  return App::Descriptor::ExaminationLevel::Strict;
}

const Image * App::Descriptor::icon() {
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

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
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
    switch (snapshot->activePage()) {
    case Snapshot::Page::Parameters:
      m_stackViewController.push(&m_parametersController, Palette::BannerFirstText, Palette::BannerFirstBackground, Palette::BannerFirstBorder);
      break;
    case Snapshot::Page::Calculations:
      m_stackViewController.push(&m_parametersController, Palette::BannerFirstText, Palette::BannerFirstBackground, Palette::BannerFirstBorder);
      m_stackViewController.push(&m_calculationController, Palette::BannerSecondText, Palette::BannerSecondBackground, Palette::BannerSecondBorder);
    default:
      break;
  }
}


}
