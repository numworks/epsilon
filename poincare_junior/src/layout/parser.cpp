#include "parser.h"

#include <poincare_junior/include/expression.h>
#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/layout/parsing/rack_parser.h>

#include "indexes.h"

namespace PoincareJ {

EditionReference Parser::Parse(const Tree* node) {
  switch (node->layoutType()) {
    case LayoutType::Fraction: {
      EditionReference ref = SharedEditionPool->push<BlockType::Division>();
      Parser::Parse(node->childAtIndex(k_numeratorIndex));
      Parser::Parse(node->childAtIndex(k_denominatorIndex));
      return ref;
    }
    case LayoutType::Rack: {
      return RackParser(node).parse();
    }
    case LayoutType::Parenthesis: {
      return Parser::Parse(node->childAtIndex(0));
    }
    case LayoutType::VerticalOffset:
    case LayoutType::CodePoint:
      assert(false);
  }
}

}  // namespace PoincareJ
