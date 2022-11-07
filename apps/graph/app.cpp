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

bool App::storageWillChangeForRecord(Ion::Storage::Record record) {
  /* Prevent functions from being (re)defined from the store menu.
   * Variables can be changed even if they are used in functions. */
  return !(isVarBoxMenuOpen() || isStoreMenuOpen()) || !record.hasExtension(Ion::Storage::funcExtension);
}

void App::storageDidChangeForRecord(Ion::Storage::Record record) {
  FunctionApp::storageDidChangeForRecord(record);
  if (record.isNull() || record.hasExtension(Ion::Storage::funcExtension)) {
    return;
  }
  /* A variable has been modified via the store menu, we need to compute if one
   * of the functions depends on this variable and update the displayed views if
   * it is the case. */
  bool shouldUpdateFunctions = false;
  for (int i = 0; i < functionStore()->numberOfModels(); i++) {
    /* Do not skip inactive functions since active ones may depend on them. */
    Shared::ExpiringPointer<ContinuousFunction> function = functionStore()->modelForRecord(functionStore()->recordAtIndex(i));
    Symbol symbol = Symbol::Builder(record.name().baseName, record.name().baseNameLength);
    Expression f = function->isNamed() ? function->expressionClone() : function->originalEquation();
    /* TODO this condition has false positives when the expression contains a
     * local context with the modified symbol. An ad-hoc hasSymbol could be
     * implemented but the best solution is to rework recursivelyMatches to make
     * it aware of parametered expressions. */
    if (f.recursivelyMatches([](const Expression e, Context * context, void * symbol) {
      if (e.isUninitialized()) {
      /* A function's expression can be uninitialized if we are storing from a
       * varbox opened while editing a function for the first time. */
        return TrinaryBoolean::False;
      }
      return e.type() == ExpressionNode::Type::Symbol && static_cast<const Symbol&>(e).isIdenticalTo(*static_cast<Symbol*>(symbol)) ? TrinaryBoolean::True : TrinaryBoolean::Unknown;
    }, context(), ExpressionNode::SymbolicComputation::DoNotReplaceAnySymbol, &symbol)) {
      shouldUpdateFunctions = true;
      break;
    }
  }
  if (!shouldUpdateFunctions) {
    return;
  }

  StackViewController * tabStacks[] = {&m_listStackViewController, &m_graphStackViewController, &m_valuesStackViewController};
  int tab = m_tabViewController.activeTab();
  assert(0 <= tab && tab < 3);
  if (tabStacks[tab]->depth() > Shared::InteractiveCurveViewController::k_graphControllerStackDepth) {
    /* Close the details/curve menu/calculation views (minimum...)/column header
     * since they may not make sense with the updated function. */
    tabStacks[tab]->popUntilDepth(Shared::InteractiveCurveViewController::k_graphControllerStackDepth, true);
  }
  /* Changing the storage may have deactivated all active functions. We pop
   * then push to make sur the graph gets updated to its empty counterpart if
   * necessary. */
  ViewController * activeViewController = tabStacks[tab]->topViewController();
  tabStacks[tab]->pop();
  tabStacks[tab]->push(activeViewController);
  /* Pushing the active controller has properly set up the first responder, but
   * it will be overriden by the modal being dismissed.
   * e.g. The store menu is summoned from the graph, its previous responder is
   * the TextField. Then a variable is stored that disables all functions.
   * After a push, the first responder will be the Tab controller, and we want
   * it to stay that way after the modal view dismissal. */
  m_modalViewController.setPreviousResponder(firstResponder());
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
