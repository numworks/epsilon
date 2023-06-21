#include "parser.h"

#include <poincare_junior/include/expression.h>
#include <poincare_junior/include/layout.h>

#include "fraction_layout.h"
#include "parenthesis_layout.h"
#include "rack_layout.h"
#include "vertical_offset_layout.h"

namespace PoincareJ {

EditionReference Parser::Parse(const Node* node) {
  assert(node.block()->isLayout());
  switch (node.type()) {
    case BlockType::FractionLayout:
      return FractionLayout::Parse(node);
    case BlockType::RackLayout:
      return RackLayout::Parse(node);
    case BlockType::ParenthesisLayout:
      return ParenthesisLayout::Parse(node);
    default:
      assert(false);
  }
}

}  // namespace PoincareJ
