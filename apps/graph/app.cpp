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
  Shared::StorageFunctionApp::Snapshot::Snapshot(),
  m_functionStore(),
  m_graphRange(&m_cursor)
{
}

App * App::Snapshot::unpack(Container * container) {
  return new (container->currentAppBuffer()) App(container, this);
}

void App::Snapshot::reset() {
  StorageFunctionApp::Snapshot::reset();
  *(modelVersion()) = 0;
  *(rangeVersion()) = 0;
}

void App::Snapshot::storageDidChangeForRecord(const Ion::Storage::Record record) {
  m_functionStore.storageDidChangeForRecord(record);
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

StorageCartesianFunctionStore * App::Snapshot::functionStore() {
  return &m_functionStore;
}

InteractiveCurveViewRange * App::Snapshot::graphRange() {
  return &m_graphRange;
}

void App::Snapshot::tidy() {
  m_functionStore.tidy();
  m_graphRange.setDelegate(nullptr);
}

App::App(Container * container, Snapshot * snapshot) :
  StorageFunctionApp(container, snapshot, &m_inputViewController),
  m_listController(&m_listFooter, &m_listHeader, &m_listFooter),
  m_listFooter(&m_listHeader, &m_listController, &m_listController, ButtonRowController::Position::Bottom, ButtonRowController::Style::EmbossedGrey),
  m_listHeader(&m_listStackViewController, &m_listFooter, &m_listController),
  m_listStackViewController(&m_tabViewController, &m_listHeader),
  m_graphController(&m_graphAlternateEmptyViewController, this, snapshot->functionStore(), snapshot->graphRange(), snapshot->cursor(), snapshot->indexFunctionSelectedByCursor(), snapshot->modelVersion(), snapshot->rangeVersion(), snapshot->angleUnitVersion(), &m_graphHeader),
  m_graphAlternateEmptyViewController(&m_graphHeader, &m_graphController, &m_graphController),
  m_graphHeader(&m_graphStackViewController, &m_graphAlternateEmptyViewController, &m_graphController),
  m_graphStackViewController(&m_tabViewController, &m_graphHeader),
  m_valuesController(&m_valuesAlternateEmptyViewController, this, snapshot->interval(), &m_valuesHeader),
  m_valuesAlternateEmptyViewController(&m_valuesHeader, &m_valuesController, &m_valuesController),
  m_valuesHeader(&m_valuesStackViewController, &m_valuesAlternateEmptyViewController, &m_valuesController),
  m_valuesStackViewController(&m_tabViewController, &m_valuesHeader),
  m_tabViewController(&m_inputViewController, snapshot, &m_listStackViewController, &m_graphStackViewController, &m_valuesStackViewController),
  m_inputViewController(&m_modalViewController, &m_tabViewController, this, this, this)
{
}

InputViewController * App::inputViewController() {
  return &m_inputViewController;
}

char App::XNT() {
  return 'x';
}

NestedMenuController * App::variableBoxForInputEventHandler(InputEventHandler * textInput) {
  VariableBoxController * varBox = container()->variableBoxController();
  varBox->setSender(textInput);
  varBox->lockDeleteEvent(VariableBoxController::Page::Function);
  return varBox;
}

}
