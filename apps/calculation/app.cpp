#include "app.h"
#include "../apps_container.h"
#include "calculation_icon.h"
#include "../i18n.h"

using namespace Poincare;

using namespace Shared;

namespace Calculation {

I18n::Message App::Descriptor::name() {
  return I18n::Message::CalculApp;
}

I18n::Message App::Descriptor::upperName() {
  return I18n::Message::CalculAppCapital;
}

const Image * App::Descriptor::icon() {
  return ImageStore::CalculationIcon;
}

App * App::Snapshot::unpack(Container * container) {
  return new App(container, this);
}

void App::Snapshot::reset() {
  m_calculationStore.deleteAll();
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

CalculationStore * App::Snapshot::calculationStore() {
  return &m_calculationStore;
}

void App::Snapshot::tidy() {
  m_calculationStore.tidy();
}

App::App(Container * container, Snapshot * snapshot) :
  TextFieldDelegateApp(container, snapshot, &m_editExpressionController),
  m_localContext((GlobalContext *)((AppsContainer *)container)->globalContext(), snapshot->calculationStore()),
  m_historyController(&m_editExpressionController, snapshot->calculationStore()),
  m_editExpressionController(&m_modalViewController, &m_historyController, snapshot->calculationStore())
{
}

Context * App::localContext() {
  return &m_localContext;
}

}
