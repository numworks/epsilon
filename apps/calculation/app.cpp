#include "app.h"
#include "../apps_container.h"
#include "calculation_icon.h"
#include "../i18n.h"
using namespace Poincare;

using namespace Shared;

namespace Calculation {

App * App::Descriptor::build(Container * container) {
  return new App(container, this);
}

I18n::Message App::Descriptor::name() {
  return I18n::Message::CalculApp;
}

I18n::Message App::Descriptor::upperName() {
  return I18n::Message::CalculAppCapital;
}

const Image * App::Descriptor::icon() {
  return ImageStore::CalculationIcon;
}

App::App(Container * container, Descriptor * descriptor) :
  TextFieldDelegateApp(container, &m_editExpressionController, descriptor),
  m_localContext((GlobalContext *)((AppsContainer *)container)->globalContext(), &m_calculationStore),
  m_calculationStore(),
  m_historyController(&m_editExpressionController, &m_calculationStore),
  m_editExpressionController(&m_modalViewController, &m_historyController, &m_calculationStore)
{
}

App::App::Descriptor * App::buildDescriptor() {
  return new App::Descriptor();
}

Context * App::localContext() {
  return &m_localContext;
}

}
