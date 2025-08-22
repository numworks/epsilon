#pragma once

#include <poincare/src/memory/tree_ref.h>

namespace Poincare::Internal {

class Integration {
 public:
  static bool Reduce(Tree* e);
};

}  // namespace Poincare::Internal
