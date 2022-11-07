#include "app.h"
#include "../apps_container.h"
#include "graph_icon.h"
#include <apps/i18n.h>

using namespace Poincare;
using namespace Shared;
using namespace Escher;

namespace Graph {

I18n::Message App::Descriptor::name() const {
  return I18n::Message::FunctionApp;
}

I18n::Message App::Descriptor::upperName() const {
  return I18n::Message::FunctionAppCapital;
}

const Image * App::Descriptor::icon() const {
  return ImageStore::GraphIcon;
}

App * App::Snapshot::unpack(Container * container) {
  return new (container->currentAppBuffer()) App(this);
}

void App::Snapshot::reset() {
  Shared::FunctionApp::Snapshot::reset();
  for (size_t i = 0; i < ContinuousFunctionProperties::k_numberOfSymbolTypes; i++) {
    m_interval[i].reset();
  }
}

constexpr static App::Descriptor sDescriptor;

const App::Descriptor * App::Snapshot::descriptor() const {
  return &sDescriptor;
}

void App::Snapshot::tidy() {
  m_functionStore.tidyDownstreamPoolFrom();
  m_graphRange.setDelegate(nullptr);
}

App::App(Snapshot * snapshot) :
  FunctionApp(snapshot, &m_listController, &m_graphController, &m_valuesController),
  m_listController(&m_listFooter, &m_listHeader, &m_listFooter, &m_graphController, &m_functionParameterController),
  m_graphController(&m_graphAlternateEmptyViewController, this, snapshot->graphRange(), snapshot->cursor(), snapshot->indexFunctionSelectedByCursor(), snapshot->rangeVersion(), &m_graphHeader),
  m_valuesController(&m_valuesAlternateEmptyViewController, this, &m_valuesHeader, &m_functionParameterController),
  m_functionParameterController(this, I18n::Message::FunctionColor, I18n::Message::DeleteExpression, &m_inputViewController, &m_graphController, &m_valuesController)
{
}

CodePoint App::XNT() {
  int selectedFunctionIndex = m_listController.selectedRow();
  if (!isStoreMenuOpen() && selectedFunctionIndex >= 0) {
    assert(selectedFunctionIndex < functionStore()->numberOfModels());
    Ion::Storage::Record record = functionStore()->recordAtIndex(selectedFunctionIndex);
    return functionStore()->modelForRecord(record)->symbol();
  }
  return ContinuousFunction::k_cartesianSymbol;
}

}
