#include "store_app.h"

using namespace Poincare;

namespace Shared {

StoreApp::Snapshot::Snapshot() {
  for (int i = 0; i < k_numberOfMemoizedFormulas; i++) {
    m_memoizedFormulasBuffer[i][0] = 0;
  }
}

bool StoreApp::Snapshot::memoizeFormula(Poincare::Layout formula, int index) {
  assert(index >= 0 && index < k_numberOfMemoizedFormulas);
  if (formula.isUninitialized()) {
    m_memoizedFormulasBuffer[index][0] = 0;
    return false;
  }
  size_t formulaLength = formula.serializeParsedExpression(
      m_memoizedFormulasBuffer[index], k_bufferSize,
      StoreApp::storeApp()->localContext());
  if (formulaLength == k_bufferSize - 1) {
    // Formula is too long
    m_memoizedFormulasBuffer[index][0] = 0;
    return false;
  }
  return true;
}

Layout StoreApp::Snapshot::memoizedFormula(int index) const {
  if (m_memoizedFormulasBuffer[index][0] == 0) {
    return Layout();
  }
  UserExpression e = UserExpression::Parse(
      m_memoizedFormulasBuffer[index], StoreApp::storeApp()->localContext());
  return e.createLayout(
      Preferences::SharedPreferences()->displayMode(),
      Preferences::SharedPreferences()->numberOfSignificantDigits(),
      StoreApp::storeApp()->localContext());
}

}  // namespace Shared
