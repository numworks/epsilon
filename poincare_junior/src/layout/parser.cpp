#include "parser.h"

#include <poincare_junior/include/expression.h>
#include <poincare_junior/include/layout.h>

#include "fraction_layout.h"
#include "parenthesis_layout.h"
#include "rack_layout.h"
#include "vertical_offset_layout.h"

namespace PoincareJ {

EditionReference Parser::Parse(const Tree* node) {
  switch (node->layoutType()) {
    case LayoutType::Fraction:
      return FractionLayout::Parse(node);
    case LayoutType::Rack:
      return RackLayout::Parse(node);
    case LayoutType::Parenthesis:
      return ParenthesisLayout::Parse(node);
    case LayoutType::VerticalOffset:
    case LayoutType::CodePoint:
      assert(false);
  }
}

}  // namespace PoincareJ
