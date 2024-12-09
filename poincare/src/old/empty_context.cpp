#include <poincare/old/context_with_parent.h>
#include <poincare/old/empty_context.h>
#include <poincare/old/junior_expression.h>

namespace Poincare {

const Internal::Tree* EmptyContext::expressionForUserNamed(
    const Internal::Tree* symbolt) {
  return nullptr;  // TODO KUndef ?
}

}  // namespace Poincare
