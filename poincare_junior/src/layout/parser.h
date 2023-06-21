#ifndef POINCARE_LAYOUT_PARSER_H
#define POINCARE_LAYOUT_PARSER_H

#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/memory/node.h>

namespace PoincareJ {

class Parser final {
 public:
  static EditionReference Parse(const Node* node);
};

}  // namespace PoincareJ

#endif
