#include <poincare/old/context_with_parent.h>
#include <poincare/old/empty_context.h>

namespace Poincare {

const Internal::Tree* EmptyContext::expressionForUserNamed(
    const Internal::Tree* symbol) const {
  return nullptr;  // TODO KUndef ?
}

}  // namespace Poincare
