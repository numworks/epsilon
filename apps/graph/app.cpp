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
  for (size_t i = 0; i < ContinuousFunction::k_numberOfSymbolTypes; i++) {
    m_interval[i].reset();
  }
}

constexpr static App::Descriptor sDescriptor;

const App::Descriptor * App::Snapshot::descriptor() const {
  return &sDescriptor;
}

bool App::storageWillChangeForRecord(Ion::Storage::Record record) {
  if (!isStoreMenuOpen()) {
    return true;
  }
  return !record.hasExtension(Ion::Storage::funcExtension);
}

void App::storageDidChangeForRecord(Ion::Storage::Record record) {
  FunctionApp::storageDidChangeForRecord(record);
  if (!isStoreMenuOpen()) {
    return;
  }
  bool shouldUpdateFunctions = false;
  for (int i = 0; i < functionStore()->numberOfModels(); i++) {
    Shared::ExpiringPointer<ContinuousFunction> function = functionStore()->modelForRecord(functionStore()->recordAtIndex(i));
    Symbol symbol = Symbol::Builder(record.name().baseName, record.name().baseNameLength);
    Expression f = function->expressionClone();
    /* TODO this condition has false positives when the expression contains a
     * bound symbol with the same name as the modified symbol. An ad-hoc
     * hasSymbol could be implemented but the best solution is to rework
     * recursivelyMatches to make it aware of parametered expressions. */
    if (f.recursivelyMatches([](const Expression e, Context * context, void * symbol) {
      return (e.type() == ExpressionNode::Type::Symbol && static_cast<const Symbol&>(e).isIdenticalTo(*static_cast<Symbol*>(symbol))) ? TrinaryBoolean::True : TrinaryBoolean::Unknown;
    }, context(), ExpressionNode::SymbolicComputation::DoNotReplaceAnySymbol, &symbol)) {
      shouldUpdateFunctions = true;
      break;
    }
  }
  if (!shouldUpdateFunctions) {
    return;
  }
  if (m_tabViewController.activeTab() == 1) {
    m_graphController.viewDidDisappear();
    m_graphController.viewWillAppear();
  } else if (m_tabViewController.activeTab() == 2) {
    m_valuesController.viewDidDisappear();
    m_valuesController.viewWillAppear();
  }
}

void App::Snapshot::tidy() {
  m_functionStore.tidyDownstreamPoolFrom();
  m_graphRange.setDelegate(nullptr);
}

App::App(Snapshot * snapshot) :
  FunctionApp(snapshot, &m_inputViewController),
  m_listController(&m_listFooter, &m_listHeader, &m_listFooter, &m_graphController, &m_functionParameterController),
  m_listFooter(&m_listHeader, &m_listController, &m_listController, ButtonRowController::Position::Bottom, ButtonRowController::Style::EmbossedGray),
  m_listHeader(&m_listStackViewController, &m_listFooter, &m_listController),
  m_listStackViewController(&m_tabViewController, &m_listHeader, Escher::StackViewController::Style::WhiteUniform),
  m_graphController(&m_graphAlternateEmptyViewController, this, snapshot->graphRange(), snapshot->cursor(), snapshot->indexFunctionSelectedByCursor(), snapshot->rangeVersion(), &m_graphHeader),
  m_graphAlternateEmptyViewController(&m_graphHeader, &m_graphController, &m_graphController),
  m_graphHeader(&m_graphStackViewController, &m_graphAlternateEmptyViewController, &m_graphController),
  m_graphStackViewController(&m_tabViewController, &m_graphHeader, Escher::StackViewController::Style::WhiteUniform),
  m_valuesController(&m_valuesAlternateEmptyViewController, this, &m_valuesHeader, &m_functionParameterController),
  m_functionParameterController(this, I18n::Message::FunctionColor, I18n::Message::DeleteExpression, &m_inputViewController, &m_graphController, &m_valuesController),
  m_valuesAlternateEmptyViewController(&m_valuesHeader, &m_valuesController, &m_valuesController),
  m_valuesHeader(&m_valuesStackViewController, &m_valuesAlternateEmptyViewController, &m_valuesController),
  m_valuesStackViewController(&m_tabViewController, &m_valuesHeader, Escher::StackViewController::Style::WhiteUniform),
  m_tabViewController(&m_inputViewController, snapshot, &m_listStackViewController, &m_graphStackViewController, &m_valuesStackViewController),
  m_inputViewController(&m_modalViewController, &m_tabViewController, &m_listController, &m_listController, &m_listController)
{
}

CodePoint App::XNT() {
  int selectedFunctionIndex = m_listController.selectedRow();
  if (selectedFunctionIndex >= 0) {
    assert(selectedFunctionIndex < functionStore()->numberOfModels());
    Ion::Storage::Record record = functionStore()->recordAtIndex(selectedFunctionIndex);
    return functionStore()->modelForRecord(record)->symbol();
  }
  return ContinuousFunction::k_cartesianSymbol;
}

NestedMenuController * App::variableBoxForInputEventHandler() {
  MathVariableBoxController * varBox = AppsContainer::sharedAppsContainer()->variableBoxController();
  varBox->lockDeleteEvent(MathVariableBoxController::Page::Function);
  return varBox;
}

}
