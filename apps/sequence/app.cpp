#include "app.h"
#include "../apps_container.h"
#include "sequence_icon.h"

using namespace Poincare;

namespace Sequence {

App * App::Descriptor::build(Container * container) {
  return new App(container, this);
}

I18n::Message App::Descriptor::name() {
  return I18n::Message::SequenceApp;
}

I18n::Message App::Descriptor::upperName() {
  return I18n::Message::SequenceAppCapital;
}

const Image * App::Descriptor::icon() {
  return ImageStore::SequenceIcon;
}

App::App(Container * container, Descriptor * descriptor) :
  FunctionApp(container, &m_inputViewController, descriptor),
  m_sequenceStore(),
  m_nContext(((AppsContainer *)container)->globalContext()),
  m_listController(&m_listFooter, &m_sequenceStore, &m_listHeader, &m_listFooter),
  m_listFooter(&m_listHeader, &m_listController, &m_listController, ButtonRowController::Position::Bottom, ButtonRowController::Style::EmbossedGrey),
  m_listHeader(nullptr, &m_listFooter, &m_listController),
  m_listStackViewController(&m_tabViewController, &m_listHeader),
  m_graphController(&m_graphAlternateEmptyViewController, &m_sequenceStore, &m_graphHeader),
  m_graphAlternateEmptyViewController(&m_graphHeader, &m_graphController, &m_graphController),
  m_graphHeader(&m_graphStackViewController, &m_graphAlternateEmptyViewController, &m_graphController),
  m_graphStackViewController(&m_tabViewController, &m_graphHeader),
  m_valuesController(&m_valuesAlternateEmptyViewController, &m_sequenceStore, &m_valuesHeader),
  m_valuesAlternateEmptyViewController(&m_valuesHeader, &m_valuesController, &m_valuesController),
  m_valuesHeader(nullptr, &m_valuesAlternateEmptyViewController, &m_valuesController),
  m_valuesStackViewController(&m_tabViewController, &m_valuesHeader),
  m_tabViewController(&m_inputViewController, &m_listStackViewController, &m_graphStackViewController, &m_valuesStackViewController),
  m_inputViewController(&m_modalViewController, &m_tabViewController, &m_listController)
{
}

App::Descriptor * App::buildDescriptor() {
  return new App::Descriptor();
}

InputViewController * App::inputViewController() {
  return &m_inputViewController;
}

Context * App::localContext() {
  if (m_tabViewController.activeTab() == 0) {
    return &m_nContext;
  }
  return TextFieldDelegateApp::localContext();
}

const char * App::XNT() {
  return "n";
}

}
