#include "parser.h"

#include <omg/unreachable.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/tree_stack_checkpoint.h>

#include "grid.h"
#include "parsing/rack_parser.h"

namespace Poincare::Internal {

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
    case LayoutType::Parentheses:
      return Type::Parentheses;
    default:
      OMG::unreachable();
  }
}

Tree* Parser::Parse(const Tree* l, Poincare::Context* context,
                    ParsingContext::ParsingMethod method) {
  if (l->isRackLayout()) {
    // TODO: should be inlined in the caller
    return RackParser(l, context, -1, method).parse();
  }
  switch (l->layoutType()) {
    case LayoutType::VerticalOffset:
    case LayoutType::AsciiCodePoint:
    case LayoutType::UnicodeCodePoint:
    case LayoutType::CombinedCodePoints:
      assert(false);
    case LayoutType::Parentheses:
    case LayoutType::CurlyBraces: {
      Tree* list = RackParser(l->child(0), context, -1, method, true).parse();
      if (!list) {
        TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
      }
      if (l->layoutType() == LayoutType::Parentheses) {
        int numberOfChildren = list->numberOfChildren();
        if (numberOfChildren == 2) {
          list->cloneNodeOverNode(KPoint);
        } else if (numberOfChildren == 1) {
          list->cloneNodeOverNode(KParentheses);
        } else {
          TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
        }
      }
      return list;
    }
    case LayoutType::Piecewise:
    case LayoutType::Matrix: {
      const Grid* grid = Grid::From(l);
      Tree* expr;
      if (grid->isMatrixLayout()) {
        expr = SharedTreeStack->pushMatrix(grid->numberOfRows() - 1,
                                           grid->numberOfColumns() - 1);
      } else {
        expr = SharedTreeStack->pushPiecewise(0);
      }
      int n = grid->numberOfChildren();
      int actualNumberOfChildren = 0;
      for (int i = 0; i < n; i++) {
        if (grid->childIsPlaceholder(i)) {
          continue;
        }
        if (!Parse(grid->child(i), context)) {
          TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
        }
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
      TreeRef ref = SharedTreeStack->pushBlock(ExpressionType(l->layoutType()));
      int n = l->numberOfChildren();
      for (int i = 0; i < n; i++) {
        if (!Parse(l->child(i), context)) {
          TreeStackCheckpoint::Raise(ExceptionType::ParseFail);
        }
      }
      return ref;
    }
  }
}

}  // namespace Poincare::Internal
