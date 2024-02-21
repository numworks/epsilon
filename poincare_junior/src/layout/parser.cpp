#include "parser.h"

#include <poincare_junior/include/expression.h>
#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/layout/parsing/rack_parser.h>

#include "grid.h"

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
    case LayoutType::ListSequence:
      return BlockType::ListSequence;
    case LayoutType::Conjugate:
      return BlockType::Conjugate;
    case LayoutType::SquareRoot:
      return BlockType::SquareRoot;
    case LayoutType::NthRoot:
      return BlockType::NthRoot;
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
    case LayoutType::CurlyBrace:
    case LayoutType::NthDerivative:
      assert(false);
    case LayoutType::Piecewise:
    case LayoutType::Matrix: {
      const Grid* grid = Grid::From(node);
      Tree* expr;
      if (grid->isMatrixLayout()) {
        expr = SharedEditionPool->push<BlockType::Matrix>(
            grid->numberOfRows() - 1, grid->numberOfColumns() - 1);
      } else {
        // TODO PCJ
        // expr = SharedEditionPool->push<BlockType::Piecewise>(
        // (grid->numberOfRows() - 1) * 2);
      }
      int n = grid->numberOfChildren();
      for (int i = 0; i < n; i++) {
        if (grid->childIsPlaceholder(i)) {
          continue;
        }
        Parse(grid->child(i));
      }
      return expr;
    }
    default: {
      // The layout children map one-to-one to the expression
      EditionReference ref =
          SharedEditionPool->push(ExpressionType(node->layoutType()));
      int n = node->numberOfChildren();
      for (int i = 0; i < n; i++) {
        Parse(node->child(i));
      }
      return ref;
    }
  }
}

}  // namespace PoincareJ
