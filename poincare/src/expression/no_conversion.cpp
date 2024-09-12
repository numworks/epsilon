#include <omg/unreachable.h>

#include "conversion.h"

namespace Poincare::Internal {

// Using old Builders with the "no_conversion" flavor will raise

Poincare::OExpression ToPoincareExpression(const Tree* e) {
  assert(false);
  OMG::unreachable();
}

void PushPoincareExpression(Poincare::OExpression exp) {
  assert(false);
  OMG::unreachable();
}

Tree* FromPoincareExpression(Poincare::OExpression exp) {
  assert(false);
  OMG::unreachable();
}

}  // namespace Poincare::Internal
