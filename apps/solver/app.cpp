#include "app.h"
#include "../i18n.h"
#include "solver_icon.h"

using namespace Shared;

namespace Solver {

I18n::Message App::Descriptor::name() {
  return I18n::Message::SolverApp;
}

I18n::Message App::Descriptor::upperName() {
  return I18n::Message::SolverAppCapital;
}

const Image * App::Descriptor::icon() {
  return ImageStore::SolverIcon;
}

App::Snapshot::Snapshot() :
  m_equationStore()
{
}

App * App::Snapshot::unpack(Container * container) {
  return new App(container, this);
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

void App::Snapshot::reset() {
  // Delete all equations
  m_equationStore.removeAll();
}

void App::Snapshot::tidy() {
  // Delete all expressions of equations
  m_equationStore.tidy();
}

App::App(Container * container, Snapshot * snapshot) :
  ExpressionFieldDelegateApp(container, snapshot, &m_inputViewController),
  m_listController(&m_stackViewController, snapshot->equationStore(), &m_listFooter),
  m_listFooter(&m_stackViewController, &m_listController, &m_listController, ButtonRowController::Position::Bottom, ButtonRowController::Style::EmbossedGrey),
  m_stackViewController(&m_inputViewController, &m_listFooter),
  m_inputViewController(&m_modalViewController, &m_stackViewController, &m_listController, &m_listController)
{
}

void App::willBecomeInactive() {
  if (m_modalViewController.isDisplayingModal()) {
    m_modalViewController.dismissModalViewController();
  }
  if (inputViewController()->isDisplayingModal()) {
    inputViewController()->abortEditionAndDismiss();
  }
  ::App::willBecomeInactive();
}

const char * App::XNT() {
  return "x";
}

}
