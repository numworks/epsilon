#include "app.h"
#include "../apps_container.h"
#include "graph_icon.h"
#include "../i18n.h"

using namespace Poincare;
using namespace Shared;

namespace Graph {

I18n::Message App::Descriptor::name() {
  return I18n::Message::FunctionApp;
}

I18n::Message App::Descriptor::upperName() {
  return I18n::Message::FunctionAppCapital;
}

const Image * App::Descriptor::icon() {
  return ImageStore::GraphIcon;
}

App::Snapshot::Snapshot() :
  m_functionStore(),
  m_graphRange(&m_cursor),
  m_cursor()
{
}

App * App::Snapshot::unpack(Container * container) {
  return new App(container, this);
}

void App::Snapshot::reset() {
  m_functionStore.removeAll();
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

CartesianFunctionStore * App::Snapshot::functionStore() {
  return &m_functionStore;
}

InteractiveCurveViewRange * App::Snapshot::graphRange() {
  return &m_graphRange;
}

CurveViewCursor * App::Snapshot::cursor() {
  return &m_cursor;
}

void App::Snapshot::tidy() {
  m_functionStore.tidy();
}

App::App(Container * container, Snapshot * snapshot) :
  FunctionApp(container, snapshot, &m_inputViewController),
  m_xContext('x',((AppsContainer *)container)->globalContext()),
  m_listController(&m_listFooter, snapshot->functionStore(), &m_listHeader, &m_listFooter),
  m_listFooter(&m_listHeader, &m_listController, &m_listController, ButtonRowController::Position::Bottom, ButtonRowController::Style::EmbossedGrey),
  m_listHeader(&m_listStackViewController, &m_listFooter, &m_listController),
  m_listStackViewController(&m_tabViewController, &m_listHeader),
  m_graphController(&m_graphAlternateEmptyViewController, snapshot->functionStore(), snapshot->graphRange(), snapshot->cursor(), &m_graphHeader),
  m_graphAlternateEmptyViewController(&m_graphHeader, &m_graphController, &m_graphController),
  m_graphHeader(&m_graphStackViewController, &m_graphAlternateEmptyViewController, &m_graphController),
  m_graphStackViewController(&m_tabViewController, &m_graphHeader),
  m_valuesController(&m_valuesAlternateEmptyViewController, snapshot->functionStore(), &m_valuesHeader),
  m_valuesAlternateEmptyViewController(&m_valuesHeader, &m_valuesController, &m_valuesController),
  m_valuesHeader(&m_valuesStackViewController, &m_valuesAlternateEmptyViewController, &m_valuesController),
  m_valuesStackViewController(&m_tabViewController, &m_valuesHeader),
  m_tabViewController(&m_inputViewController, &m_listStackViewController, &m_graphStackViewController, &m_valuesStackViewController),
  m_inputViewController(&m_modalViewController, &m_tabViewController, this)
{
}

InputViewController * App::inputViewController() {
  return &m_inputViewController;
}

Context * App::localContext() {
  if (m_tabViewController.activeTab() == 0) {
    return &m_xContext;
  }
  return TextFieldDelegateApp::localContext();
}

}
