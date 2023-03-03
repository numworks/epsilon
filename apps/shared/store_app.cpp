#include "store_app.h"

using namespace Poincare;

namespace Shared {

StoreApp::Snapshot::Snapshot() {
  for (int i = 0; i < k_numberOfColumns; i++) {
    m_memoizedFormulasBuffer[i][0] = 0;
  }
}

bool StoreApp::Snapshot::memoizeFormulaAtColumn(Poincare::Layout formula,
                                                int column) {
  assert(column >= 0 && column < k_numberOfColumns);
  if (formula.isUninitialized()) {
    m_memoizedFormulasBuffer[column][0] = 0;
    return false;
  }
  size_t formulaLength = formula.serializeParsedExpression(
      m_memoizedFormulasBuffer[column], k_bufferSize,
      StoreApp::storeApp()->localContext());
  if (formulaLength == k_bufferSize - 1) {
    // Formula is too long
    m_memoizedFormulasBuffer[column][0] = 0;
    return false;
  }
  return true;
}

Layout StoreApp::Snapshot::memoizedFormulaAtColumn(int column) const {
  if (m_memoizedFormulasBuffer[column][0] == 0) {
    return Layout();
  }
  Expression e = Expression::Parse(m_memoizedFormulasBuffer[column],
                                   StoreApp::storeApp()->localContext());
  return e.createLayout(
      Preferences::sharedPreferences->displayMode(),
      Preferences::sharedPreferences->numberOfSignificantDigits(),
      StoreApp::storeApp()->localContext());
}

void StoreApp::didBecomeActive(Escher::Window *window) {
  storeController()->loadMemoizedFormulasFromSnapshot();
  ExpressionFieldDelegateApp::didBecomeActive(window);
}

}  // namespace Shared
