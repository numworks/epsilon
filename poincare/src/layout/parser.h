#ifndef POINCARE_LAYOUT_PARSER_H
#define POINCARE_LAYOUT_PARSER_H

#include <poincare/old/context.h>
#include <poincare/src/memory/tree.h>
#include <poincare/src/memory/tree_ref.h>

namespace Poincare::Internal {

class Parser final {
 public:
  static Tree* Parse(const Tree* node, Poincare::Context* context);
};

}  // namespace Poincare::Internal

#endif
