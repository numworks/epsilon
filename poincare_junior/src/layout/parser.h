#ifndef POINCARE_LAYOUT_PARSER_H
#define POINCARE_LAYOUT_PARSER_H

#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/memory/tree.h>

namespace PoincareJ {

class Parser final {
 public:
  static Tree* Parse(Tree* node);
};

}  // namespace PoincareJ

#endif
