#include "store_app.h"

namespace Shared {

StoreApp::Snapshot::Snapshot() {
  for (int i = 0; i < k_numberOfColumns; i++) {
    m_memoizedFormulasSize[i] = 0;
  }
}

bool StoreApp::Snapshot::memoizeFormulaAtColumn(Poincare::Layout formula,
                                                int column) {
  assert(column >= 0 && column < k_numberOfColumns);
  size_t layoutSize = formula.isUninitialized() ? 0 : formula.size();
  if (formula.isUninitialized() || layoutSize > k_maxFormulaSize) {
    m_memoizedFormulasSize[column] = 0;
    return false;
  }
  m_memoizedFormulasSize[column] = layoutSize;
  memcpy(m_memoizedFormulasBuffer + column, formula.addressInPool(),
         layoutSize);
  return true;
}

Poincare::Layout StoreApp::Snapshot::memoizedFormulaAtColumn(int column) const {
  if (m_memoizedFormulasSize[column] == 0) {
    return Poincare::Layout();
  }
  return Poincare::Layout::LayoutFromAddress(m_memoizedFormulasBuffer + column,
                                             m_memoizedFormulasSize[column]);
}

void StoreApp::didBecomeActive(Escher::Window *window) {
  storeController()->loadMemoizedFormulasFromSnapshot();
  ExpressionFieldDelegateApp::didBecomeActive(window);
}

}  // namespace Shared