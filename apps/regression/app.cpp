#include "app.h"
#include "regression_icon.h"
#include "../i18n.h"

using namespace Shared;

namespace Regression {

App * App::Descriptor::build(Container * container) {
  return new App(container, this);
}

I18n::Message App::Descriptor::name() {
  return I18n::Message::RegressionApp;
}

I18n::Message App::Descriptor::upperName() {
  return I18n::Message::RegressionAppCapital;
}

const Image * App::Descriptor::icon() {
  return ImageStore::RegressionIcon;
}

App::App(Container * container, Descriptor * descriptor) :
  TextFieldDelegateApp(container, &m_tabViewController, descriptor),
  m_store(),
  m_calculationController(&m_calculationAlternateEmptyViewController, &m_calculationHeader, &m_store),
  m_calculationAlternateEmptyViewController(&m_calculationHeader, &m_calculationController, &m_calculationController),
  m_calculationHeader(&m_tabViewController, &m_calculationAlternateEmptyViewController, &m_calculationController),
  m_graphController(&m_graphAlternateEmptyViewController, &m_graphHeader, &m_store),
  m_graphAlternateEmptyViewController(&m_graphHeader, &m_graphController, &m_graphController),
  m_graphHeader(&m_graphStackViewController, &m_graphAlternateEmptyViewController, &m_graphController),
  m_graphStackViewController(&m_tabViewController, &m_graphHeader),
  m_storeController(&m_storeHeader, &m_store, &m_storeHeader),
  m_storeHeader(&m_storeStackViewController, &m_storeController, &m_storeController),
  m_storeStackViewController(&m_tabViewController, &m_storeHeader),
  m_tabViewController(&m_modalViewController, &m_storeStackViewController, &m_graphStackViewController, &m_calculationHeader)
{
}

App::Descriptor * App::buildDescriptor() {
  return new App::Descriptor();
}

}
