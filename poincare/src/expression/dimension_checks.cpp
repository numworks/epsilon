#include "dimension_checks.h"

#include "dimension.h"

namespace Poincare::Internal::Approximation {

bool DimensionChecks(const Tree* e, const SymbolContext& symbolContext,
                     bool isListAllowed, bool optimize) {
  if (!optimize) {
    if (!Dimension::DeepCheck(e, symbolContext)) {
      return false;
    }
    if (!isListAllowed && Dimension::IsList(e, symbolContext)) {
      return false;
    }
    return true;
  }
  COSTLY_ASSERT(Dimension::DeepCheck(e, symbolContext));
  if (!isListAllowed) {
    COSTLY_ASSERT(!Dimension::IsList(e, symbolContext));
  }
  return true;
}

}  // namespace Poincare::Internal::Approximation
