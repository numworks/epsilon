#include "store_app.h"

#include <poincare/k_tree.h>
#include <poincare/layout.h>
#include <poincare/src/memory/block.h>
#include <poincare/symbol_context.h>
#include <poincare/tree.h>

using namespace Poincare;

namespace Shared {

StoreApp::Snapshot::Snapshot() {
  for (int i = 0; i < k_numberOfMemoizedFormulas; i++) {
    m_memoizedFormulasBuffer[i][0] = 0;
  }
}

bool StoreApp::Snapshot::memoizeFormula(Poincare::Layout formula, int index) {
  assert(index >= 0 && index < k_numberOfMemoizedFormulas);
  static_assert(!static_cast<Internal::Tree>(Internal::Type(0)).isLayout());
  /* We use 0 as error
   * TODO: Create an error block type ? */
  if (formula.isUninitialized()) {
    m_memoizedFormulasBuffer[index][0] = 0;
    return false;
  }
  size_t size = formula.tree()->treeSize();
  if (size > k_bufferSize - 1) {
    // Formula is too long
    m_memoizedFormulasBuffer[index][0] = 0;
    return false;
  }
  memcpy(m_memoizedFormulasBuffer[index], formula.tree(), size);
  return true;
}

Layout StoreApp::Snapshot::memoizedFormula(int index) const {
  /* We use 0 as error
   * TODO: Create an error block type ? */
  if (m_memoizedFormulasBuffer[index][0] == static_cast<Internal::Block>(0)) {
    return Poincare::Layout();
  }
  const Internal::Tree* layoutTree =
      Internal::Tree::FromBlocks(m_memoizedFormulasBuffer[index]);
  return Poincare::Layout::Builder(layoutTree);
}

}  // namespace Shared
