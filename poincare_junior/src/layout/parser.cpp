#include "parser.h"

#include <poincare_junior/src/layout/parsing/rack_parser.h>

#include "../n_ary.h"
#include "grid.h"

namespace PoincareJ {

Type ExpressionType(LayoutType type) {
  switch (type) {
    case LayoutType::Fraction:
      return Type::Div;
    case LayoutType::Binomial:
    case LayoutType::PtBinomial:
      return Type::Binomial;
    case LayoutType::PtPermute:
      return Type::Permute;
    case LayoutType::Abs:
      return Type::Abs;
    case LayoutType::Ceil:
      return Type::Ceil;
    case LayoutType::Floor:
      return Type::Floor;
    case LayoutType::VectorNorm:
      return Type::Norm;
    case LayoutType::Diff:
      return Type::Diff;
    case LayoutType::Integral:
      return Type::Integral;
    case LayoutType::Product:
      return Type::Product;
    case LayoutType::Sum:
      return Type::Sum;
    case LayoutType::ListSequence:
      return Type::ListSequence;
    case LayoutType::Conj:
      return Type::Conj;
    case LayoutType::Sqrt:
      return Type::Sqrt;
    case LayoutType::Root:
      return Type::Root;
    case LayoutType::Parenthesis:
      return Type::Parenthesis;
    default:
      assert(false);
  }
}

Tree* Parser::Parse(const Tree* node, Poincare::Context* context) {
  switch (node->layoutType()) {
    case LayoutType::Rack:
      return RackParser(node, context).parse();
    // case LayoutType::Parenthesis:
    // return Parse(node->child(0), context);
    case LayoutType::VerticalOffset:
    case LayoutType::AsciiCodePoint:
    case LayoutType::UnicodeCodePoint:
    case LayoutType::CombinedCodePoints:
    case LayoutType::CurlyBrace:
    case LayoutType::NthDiff:
      assert(false);
    case LayoutType::Piecewise:
    case LayoutType::Matrix: {
      const Grid* grid = Grid::From(node);
      Tree* expr;
      if (grid->isMatrixLayout()) {
        expr = SharedTreeStack->push<Type::Matrix>(grid->numberOfRows() - 1,
                                                   grid->numberOfColumns() - 1);
      } else {
        expr = SharedTreeStack->push<Type::Piecewise>(0);
      }
      int n = grid->numberOfChildren();
      int actualNumberOfChildren = 0;
      for (int i = 0; i < n; i++) {
        if (grid->childIsPlaceholder(i)) {
          continue;
        }
        Parse(grid->child(i), context);
        actualNumberOfChildren++;
      }
      if (expr->isPiecewise()) {
        /* Update number of children to take the bottom right condition only if
         * it is not a placeholder */
        NAry::SetNumberOfChildren(expr, actualNumberOfChildren);
      }
      return expr;
    }
    default: {
      // The layout children map one-to-one to the expression
      TreeRef ref = SharedTreeStack->push(ExpressionType(node->layoutType()));
      int n = node->numberOfChildren();
      for (int i = 0; i < n; i++) {
        Parse(node->child(i), context);
      }
      return ref;
    }
  }
}

}  // namespace PoincareJ
