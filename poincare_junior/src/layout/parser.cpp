#include "parser.h"

#include <poincare_junior/include/expression.h>
#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/layout/parsing/rack_parser.h>

namespace PoincareJ {

BlockType ExpressionType(LayoutType type) {
  switch (type) {
    case LayoutType::Fraction:
      return BlockType::Division;
    case LayoutType::Binomial:
    case LayoutType::PtBinomial:
      return BlockType::Binomial;
    case LayoutType::PtPermute:
      return BlockType::Permute;
    case LayoutType::AbsoluteValue:
      return BlockType::Abs;
    case LayoutType::Ceiling:
      return BlockType::Ceiling;
    case LayoutType::Floor:
      return BlockType::Floor;
    case LayoutType::VectorNorm:
      return BlockType::Norm;
    case LayoutType::Derivative:
      return BlockType::Derivative;
    case LayoutType::Integral:
      return BlockType::Integral;
    case LayoutType::Product:
      return BlockType::Product;
    case LayoutType::Sum:
      return BlockType::Sum;
    case LayoutType::Matrix:
      return BlockType::Matrix;
    case LayoutType::ListSequence:
      return BlockType::ListSequence;
    case LayoutType::Conjugate:
      return BlockType::Conjugate;
    case LayoutType::SquareRoot:
      return BlockType::SquareRoot;
    case LayoutType::NthRoot:
      // TODO
    default:
      assert(false);
  }
}

Tree* Parser::Parse(const Tree* node) {
  switch (node->layoutType()) {
    case LayoutType::Rack:
      return RackParser(node).parse();
    case LayoutType::Parenthesis:
      return Parse(node->child(0));
    case LayoutType::VerticalOffset:
    case LayoutType::CodePoint:
    case LayoutType::CombinedCodePoints:
    case LayoutType::String:
    case LayoutType::CurlyBrace:
    case LayoutType::CondensedSum:
    case LayoutType::Sequence:
    case LayoutType::Piecewise:
      assert(false);
    default: {
      // The layout children map one-to-one to the expression
      EditionReference ref =
          SharedEditionPool->push(ExpressionType(node->layoutType()));
      int n = node->numberOfChildren();
      for (int i = 0; i < n; i++) {
        Parse(node->child(0));
      }
      return ref;
    }
  }
}

}  // namespace PoincareJ
