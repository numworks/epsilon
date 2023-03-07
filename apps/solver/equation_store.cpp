#include "equation_store.h"

#include <apps/constant.h>
#include <apps/global_preferences.h>
#include <apps/shared/expression_display_permissions.h>
#include <apps/shared/poincare_helpers.h>
#include <poincare/matrix.h>
#include <poincare/polynomial.h>
#include <poincare/symbol.h>

#include "app.h"

using namespace Poincare;
using namespace Shared;

namespace Solver {

Ion::Storage::Record::ErrorStatus EquationStore::addEmptyModel() {
  char name[3];
  static_assert(k_maxNumberOfEquations < 9,
                "Equation name record might not fit");
  const char *const extensions[1] = {Ion::Storage::eqExtension};
  name[0] = 'e';
  Ion::Storage::FileSystem::sharedFileSystem->firstAvailableNameFromPrefix(
      name, 1, 3, extensions, 1, k_maxNumberOfEquations);
  return Ion::Storage::FileSystem::sharedFileSystem->createRecordWithExtension(
      name, Ion::Storage::eqExtension, nullptr, 0);
}

Shared::ExpressionModelHandle *EquationStore::setMemoizedModelAtIndex(
    int cacheIndex, Ion::Storage::Record record) const {
  assert(cacheIndex >= 0 && cacheIndex < maxNumberOfMemoizedModels());
  m_equations[cacheIndex] = Equation(record);
  return &m_equations[cacheIndex];
}

ExpressionModelHandle *EquationStore::memoizedModelAtIndex(
    int cacheIndex) const {
  assert(cacheIndex >= 0 && cacheIndex < maxNumberOfMemoizedModels());
  return &m_equations[cacheIndex];
}

}  // namespace Solver
