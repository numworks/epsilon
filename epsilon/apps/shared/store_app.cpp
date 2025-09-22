#include "store_app.h"

#include <ion/storage/record.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>
#include <poincare/src/memory/block.h>
#include <poincare/symbol_context.h>
#include <poincare/tree.h>

using namespace Poincare;

namespace Shared {

bool StoreApp::memoizeFormula(Poincare::Layout formula, int index) {
  // Name memoized formulas from A to Z.
  static_assert('Z' - 'A' >= k_numberOfMemoizedFormulas);
  assert(index >= 0 && index < k_numberOfMemoizedFormulas);
  const char baseName[2] = {static_cast<char>('A' + index), 0};
  Ion::Storage::Record(baseName, memoizedFormulaExtension()).tryToDestroy();
  if (formula.isUninitialized()) {
    return false;
  }
  Ion::Storage::Record::ErrorStatus err =
      Ion::Storage::FileSystem::sharedFileSystem->createRecordWithExtension(
          baseName, memoizedFormulaExtension(), formula.tree(),
          formula.tree()->treeSize(), true);
  return err == Ion::Storage::Record::ErrorStatus::None;
}

Layout StoreApp::memoizedFormula(int index) const {
  const char baseName[2] = {static_cast<char>('A' + index), 0};
  Ion::Storage::Record r(baseName, memoizedFormulaExtension());
  if (r.isNull()) {
    return Poincare::Layout();
  }
  return Poincare::Layout::Builder(
      static_cast<const Poincare::Internal::Tree*>(r.value().buffer));
}

}  // namespace Shared
