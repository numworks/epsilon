#ifndef POINCARE_LAYOUT_PARSER_H
#define POINCARE_LAYOUT_PARSER_H

#include <poincare/context.h>
#include <poincare_junior/src/memory/tree.h>
#include <poincare_junior/src/memory/tree_ref.h>

namespace PoincareJ {

class Parser final {
 public:
  static Tree* Parse(const Tree* node, Poincare::Context* context);
};

}  // namespace PoincareJ

#endif
