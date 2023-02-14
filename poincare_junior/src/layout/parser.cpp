#include <poincare_junior/include/layout.h>
#include <poincare_junior/include/expression.h>
#include "parser.h"
#include "fraction_layout.h"
#include "rack_layout.h"

namespace PoincareJ {

EditionReference Parser::EditionPoolLayoutToExpression(const Node node) {
  assert(node.block()->isLayout());
  switch (node.type()) {
  case BlockType::FractionLayout:
    return FractionLayout::Parse(node);
  case BlockType::RackLayout:
    return RackLayout::Parse(node);
  }
}

}
