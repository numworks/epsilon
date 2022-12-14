#include "function_app.h"

using namespace Escher;
using namespace Poincare;

namespace Shared {

FunctionApp::Snapshot::Snapshot() :
  m_indexFunctionSelectedByCursor(0)
{
}

void FunctionApp::Snapshot::reset() {
  m_indexFunctionSelectedByCursor = 0;
  setActiveTab(0);
}

bool FunctionApp::storageWillChangeForRecordName(const Ion::Storage::Record::Name recordName) {
  /* Prevent functions from being (re)defined from the store menu.
   * Variables can be changed even if they are used in functions.
   * Warning: this has no effect on Sequence yet: we can't define sequences
   * from the store menu.  */
  return !(isVarBoxMenuOpen() || isStoreMenuOpen()) || strcmp(recordName.extension, functionStore()->modelExtension()) != 0;
}

void FunctionApp::storageDidChangeForRecord(Ion::Storage::Record record) {
  // SequenceStore is actually already invalidated by the globalContext (see TODO in GlobalContext)
  functionStore()->storageDidChangeForRecord(record);
  if (record.isNull() || record.hasExtension(functionStore()->modelExtension())) {
    return;
  }
  /* TODO: we could avoid updating the graph/values when the record doesn't
   * affect the store but it's hard to detect indirect dependencies:
   * 3 -> a
   * a -> u(n)
   * u(4) + 2 -> f(x)
   */
#if 0
  /* A variable has been modified via the store menu, we need to compute if one
   * of the functions depends on this variable and update the displayed views if
   * it is the case. */
  bool shouldUpdateFunctions = false;
  for (int i = 0; i < functionStore()->numberOfModels(); i++) {
    /* Do not skip inactive functions since active ones may depend on them. */
    Shared::ExpiringPointer<Function> function = functionStore()->modelForRecord(functionStore()->recordAtIndex(i));
    Symbol symbol = Symbol::Builder(record.name().baseName, record.name().baseNameLength);
    Expression f = function->isNamed() ? function->expressionClone() : function->originalEquation();
    /* TODO this condition has false positives when the expression contains a
     * local context with the modified symbol. An ad-hoc hasSymbol could be
     * implemented but the best solution is to rework recursivelyMatches to make
     * it aware of parametered expressions. */
    if (f.recursivelyMatches([](const Expression e, Context * context, void * symbol) {
      return e.type() == ExpressionNode::Type::Symbol && static_cast<const Symbol&>(e).isIdenticalTo(*static_cast<Symbol*>(symbol));
    }, context(), ExpressionNode::SymbolicComputation::DoNotReplaceAnySymbol, &symbol)) {
      shouldUpdateFunctions = true;
      break;
    }
  }
  if (!shouldUpdateFunctions) {
    return;
  }
#endif
  int tab = m_tabViewController.activeTab();
  StackViewController * tabStacks[] = {&m_listStackViewController, &m_graphStackViewController, &m_valuesStackViewController};
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
}

FunctionApp::FunctionApp(Snapshot * snapshot, Shared::FunctionListController * listController, Shared::FunctionGraphController * graphController, Shared::ValuesController * valuesController) :
  ExpressionFieldDelegateApp(snapshot, &m_inputViewController),
  m_listFooter(&m_listHeader, listController, listController, ButtonRowController::Position::Bottom, ButtonRowController::Style::EmbossedGray),
  m_listHeader(&m_listStackViewController, &m_listFooter, listController),
  m_listStackViewController(&m_tabViewController, &m_listHeader, Escher::StackViewController::Style::WhiteUniform),
  m_graphAlternateEmptyViewController(&m_graphHeader, graphController, graphController),
  m_graphHeader(&m_graphStackViewController, &m_graphAlternateEmptyViewController, graphController),
  m_graphStackViewController(&m_tabViewController, &m_graphHeader, Escher::StackViewController::Style::WhiteUniform),
  m_valuesAlternateEmptyViewController(&m_valuesHeader, valuesController, valuesController),
  m_valuesHeader(&m_valuesStackViewController, &m_valuesAlternateEmptyViewController, valuesController),
  m_valuesStackViewController(&m_tabViewController, &m_valuesHeader, Escher::StackViewController::Style::WhiteUniform),
  m_tabViewController(&m_inputViewController, snapshot, &m_listStackViewController, &m_graphStackViewController, &m_valuesStackViewController),
  m_inputViewController(&m_modalViewController, &m_tabViewController, listController, listController, listController)
{
}

void FunctionApp::didBecomeActive(Escher::Window * windows) {
  Escher::App::didBecomeActive(windows);
  m_tabViewController.enterActiveTab();
}

}
