#include "cursor_motion.h"

#include <poincare_junior/include/layout.h>

#include "autocompleted_pair.h"
#include "code_point_layout.h"
#include "grid.h"
#include "indices.h"
#include "rack_layout.h"

namespace PoincareJ {

int CursorMotion::IndexAfterHorizontalCursorMove(
    Tree* node, OMG::HorizontalDirection direction, int currentIndex) {
  int nChildren = node->numberOfChildren();
  switch (node->layoutType()) {
    case LayoutType::Point2D:
    case LayoutType::Binomial:
    case LayoutType::Fraction:
      if (currentIndex == k_outsideIndex) {
        return direction.isRight() ? TwoRows::k_upperIndex
                                   : TwoRows::k_lowerIndex;
      }
      return k_outsideIndex;
    case LayoutType::Matrix:
    case LayoutType::Piecewise: {
      const Grid* grid = Grid::From(node);
      if (currentIndex == k_outsideIndex) {
        int row = grid->numberOfRows() - 1 - !grid->numberOfRowsIsFixed();
        int col = grid->numberOfColumns() - 1 - !grid->numberOfColumnsIsFixed();
        return direction.isLeft()
                   // last real child
                   ? grid->indexAtRowColumn(row, col)
                   : 0;
      }
      if ((direction.isLeft() && grid->childIsLeftOfGrid(currentIndex)) ||
          (direction.isRight() && grid->childIsRightOfGrid(currentIndex))) {
        return k_outsideIndex;
      }
      int step = direction.isLeft() ? -1 : 1;
      return currentIndex + step;
    }
    case LayoutType::Derivative:
    case LayoutType::NthDerivative: {
      using namespace Derivative;
      if (node->isDerivativeLayout()) {
        if (currentIndex == k_derivandIndex) {
          SetVariableSlot(node, direction.isRight() ? VariableSlot::Assignment
                                                    : VariableSlot::Fraction);
          return k_variableIndex;
        }
        if (currentIndex == k_variableIndex &&
            GetVariableSlot(node) == VariableSlot::Fraction) {
          return direction.isRight() ? k_derivandIndex : k_outsideIndex;
        }
      } else {
        if (currentIndex == k_derivandIndex) {
          if (direction.isRight()) {
            SetVariableSlot(node, VariableSlot::Assignment);
            return k_variableIndex;
          }
          SetOrderSlot(node, OrderSlot::Denominator);
          return k_orderIndex;
        }
        if (currentIndex == k_variableIndex &&
            GetVariableSlot(node) == VariableSlot::Fraction) {
          if (direction.isRight()) {
            SetOrderSlot(node, OrderSlot::Denominator);
            return k_orderIndex;
          }
          return k_outsideIndex;
        }
        if (currentIndex == k_orderIndex) {
          if (GetOrderSlot(node) == OrderSlot::Denominator) {
            if (direction.isLeft()) {
              SetVariableSlot(node, VariableSlot::Fraction);
              return k_variableIndex;
            }
            return k_derivandIndex;
          }
          assert(GetOrderSlot(node) == OrderSlot::Numerator);
          return direction.isRight() ? k_derivandIndex : k_outsideIndex;
        }
      }
      if (currentIndex == k_outsideIndex && direction.isRight()) {
        SetVariableSlot(node, VariableSlot::Fraction);
        return k_variableIndex;
      }
      if (currentIndex == k_abscissaIndex && direction.isLeft()) {
        SetVariableSlot(node, VariableSlot::Assignment);
        return k_variableIndex;
      }
      switch (currentIndex) {
        case k_outsideIndex:
          assert(direction.isLeft());
          return k_abscissaIndex;
        case k_abscissaIndex:
          assert(direction.isRight());
          return k_outsideIndex;
        default: {
          assert(currentIndex == k_variableIndex &&
                 GetVariableSlot(node) == VariableSlot::Assignment);
          return direction.isRight() ? k_abscissaIndex : k_derivandIndex;
        }
      }
    }
    case LayoutType::Integral:
      switch (currentIndex) {
        using namespace Integral;
        case k_outsideIndex:
          return direction.isRight() ? k_upperBoundIndex : k_differentialIndex;
        case k_upperBoundIndex:
        case k_lowerBoundIndex:
          return direction.isRight() ? k_integrandIndex : k_outsideIndex;
        case k_integrandIndex:
          return direction.isRight() ? k_differentialIndex : k_lowerBoundIndex;
        case k_differentialIndex:
          return direction.isRight() ? k_outsideIndex : k_integrandIndex;
      }
    case LayoutType::PtBinomial:
    case LayoutType::PtPermute:
      switch (currentIndex) {
        using namespace PtCombinatorics;
        case k_outsideIndex:
          return direction.isRight() ? k_nIndex : k_kIndex;
        case k_nIndex:
          return direction.isRight() ? k_kIndex : k_outsideIndex;
        default:
          assert(currentIndex == k_kIndex);
          return direction.isRight() ? k_outsideIndex : k_nIndex;
      }
    case LayoutType::ListSequence:
      switch (currentIndex) {
        using namespace ListSequence;
        case k_outsideIndex:
          return direction.isRight() ? k_functionIndex : k_upperBoundIndex;
        case k_functionIndex:
          return direction.isRight() ? k_variableIndex : k_outsideIndex;
        case k_variableIndex:
          return direction.isRight() ? k_upperBoundIndex : k_functionIndex;
        default:
          assert(currentIndex == k_upperBoundIndex);
          return direction.isRight() ? k_outsideIndex : k_variableIndex;
      }
    case LayoutType::NthRoot:
      switch (currentIndex) {
        using namespace NthRoot;
        case k_outsideIndex:
          return direction.isRight() ? k_indexIndex : k_radicandIndex;
        case k_indexIndex:
          return direction.isRight() ? k_radicandIndex : k_outsideIndex;
        default:
          assert(currentIndex == k_radicandIndex);
          return direction.isRight() ? k_outsideIndex : k_indexIndex;
      }
    case LayoutType::Product:
    case LayoutType::Sum:
      switch (currentIndex) {
        using namespace Parametric;
        case k_outsideIndex:
          return direction.isRight() ? k_upperBoundIndex : k_argumentIndex;
        case k_upperBoundIndex:
          return direction.isRight() ? k_argumentIndex : k_outsideIndex;
        case k_variableIndex:
          return direction.isRight() ? k_lowerBoundIndex : k_outsideIndex;
        case k_lowerBoundIndex:
          return direction.isRight() ? k_argumentIndex : k_variableIndex;
        default:
          assert(currentIndex == k_argumentIndex);
          return direction.isRight() ? k_outsideIndex : k_lowerBoundIndex;
      }
    default:
      if (nChildren == 0) {
        assert(currentIndex == k_outsideIndex);
        return k_outsideIndex;
      }
      if (nChildren == 1) {
        assert(currentIndex == k_outsideIndex || currentIndex == 0);
        return currentIndex == k_outsideIndex ? 0 : k_outsideIndex;
      }
      assert(false);
      return k_cantMoveIndex;
  }
}

int CursorMotion::IndexAfterVerticalCursorMove(
    Tree* node, OMG::VerticalDirection direction, int currentIndex,
    PositionInLayout positionAtCurrentIndex) {
  switch (node->layoutType()) {
    case LayoutType::Fraction:
      if (currentIndex == k_outsideIndex) {
        return direction.isUp() ? TwoRows::k_upperIndex : TwoRows::k_lowerIndex;
      }
      // continue
    case LayoutType::Point2D:
    case LayoutType::Binomial: {
      using namespace TwoRows;
      if (currentIndex == k_lowerIndex && direction.isUp()) {
        return k_upperIndex;
      }
      if (currentIndex == k_upperIndex && direction.isDown()) {
        return k_lowerIndex;
      }
      return k_cantMoveIndex;
    }
    case LayoutType::Matrix:
    case LayoutType::Piecewise: {
      const Grid* grid = Grid::From(node);
      if (currentIndex == k_outsideIndex) {
        return k_cantMoveIndex;
      }
      if (direction.isUp() && !grid->childIsTopOfGrid(currentIndex)) {
        return currentIndex - grid->numberOfColumns();
      }
      if (direction.isDown() && !grid->childIsBottomOfGrid(currentIndex)) {
        return currentIndex + grid->numberOfColumns();
      }
      return k_cantMoveIndex;
    }
    case LayoutType::Derivative:
    case LayoutType::NthDerivative: {
      using namespace Derivative;
      if (node->isDerivativeLayout()) {
        if (direction.isDown() && currentIndex == k_derivandIndex &&
            positionAtCurrentIndex == PositionInLayout::Left) {
          SetVariableSlot(node, VariableSlot::Fraction);
          return k_variableIndex;
        }
        if (direction.isUp() && currentIndex == k_variableIndex &&
            GetVariableSlot(node) == VariableSlot::Fraction) {
          return k_derivandIndex;
        }
      } else {
        if (direction.isUp() && currentIndex == k_variableIndex &&
            GetVariableSlot(node) == VariableSlot::Fraction) {
          SetOrderSlot(node, positionAtCurrentIndex == PositionInLayout::Right
                                 ? OrderSlot::Denominator
                                 : OrderSlot::Numerator);
          return k_orderIndex;
        }
        if (direction.isUp() &&
            ((currentIndex == k_derivandIndex &&
              positionAtCurrentIndex == PositionInLayout::Left) ||
             (currentIndex == k_orderIndex &&
              GetOrderSlot(node) == OrderSlot::Denominator))) {
          SetOrderSlot(node, OrderSlot::Numerator);
          return k_orderIndex;
        }
        if (direction.isDown() &&
            ((currentIndex == k_derivandIndex &&
              positionAtCurrentIndex == PositionInLayout::Left) ||
             (currentIndex == k_orderIndex &&
              GetOrderSlot(node) == OrderSlot::Numerator))) {
          SetOrderSlot(node, OrderSlot::Denominator);
          return k_orderIndex;
        }
        if (direction.isDown() && currentIndex == k_orderIndex &&
            GetOrderSlot(node) == OrderSlot::Denominator &&
            positionAtCurrentIndex == PositionInLayout::Left) {
          SetVariableSlot(node, VariableSlot::Fraction);
          return k_variableIndex;
        }
      }
      if (direction.isUp() && currentIndex == k_variableIndex &&
          GetVariableSlot(node) == VariableSlot::Assignment) {
        return k_derivandIndex;
      }
      if (direction.isDown() && currentIndex == k_derivandIndex &&
          positionAtCurrentIndex == PositionInLayout::Right) {
        return k_abscissaIndex;
      }
      return k_cantMoveIndex;
    }
    case LayoutType::Integral: {
      using namespace Integral;
      if (currentIndex == k_integrandIndex &&
          positionAtCurrentIndex == PositionInLayout::Left) {
        return direction.isUp() ? k_upperBoundIndex : k_lowerBoundIndex;
      }
      if (currentIndex == k_upperBoundIndex && direction.isDown()) {
        return k_lowerBoundIndex;
      }
      if (currentIndex == k_lowerBoundIndex && direction.isUp()) {
        return k_upperBoundIndex;
      }
      return k_cantMoveIndex;
    }
    case LayoutType::PtBinomial:
    case LayoutType::PtPermute: {
      using namespace PtCombinatorics;
      if (direction.isUp() &&
          (currentIndex == k_kIndex ||
           (currentIndex == k_outsideIndex &&
            positionAtCurrentIndex == PositionInLayout::Left))) {
        return k_nIndex;
      }
      if (direction.isDown() &&
          (currentIndex == k_nIndex ||
           (currentIndex == k_outsideIndex &&
            positionAtCurrentIndex == PositionInLayout::Right))) {
        return k_kIndex;
      }
      return k_cantMoveIndex;
    }
    case LayoutType::NthRoot: {
      using namespace NthRoot;
      if (direction.isUp() &&
          positionAtCurrentIndex == PositionInLayout::Left &&
          (currentIndex == k_outsideIndex || currentIndex == k_radicandIndex)) {
        return k_indexIndex;
      }
      if (direction.isDown() && currentIndex == k_indexIndex &&
          positionAtCurrentIndex != PositionInLayout::Middle) {
        return positionAtCurrentIndex == PositionInLayout::Right
                   ? k_radicandIndex
                   : k_outsideIndex;
      }
      return k_cantMoveIndex;
    }
    case LayoutType::Product:
    case LayoutType::Sum: {
      using namespace Parametric;
      if (direction.isUp() &&
          ((currentIndex == k_variableIndex ||
            currentIndex == k_lowerBoundIndex) ||
           (positionAtCurrentIndex == PositionInLayout::Left &&
            (currentIndex == k_outsideIndex ||
             currentIndex == k_argumentIndex)))) {
        return k_upperBoundIndex;
      }
      if (direction.isDown() &&
          ((currentIndex == k_upperBoundIndex) ||
           (positionAtCurrentIndex == PositionInLayout::Left &&
            (currentIndex == k_outsideIndex ||
             currentIndex == k_argumentIndex)))) {
        return k_lowerBoundIndex;
      }
      return k_cantMoveIndex;
    }
    case LayoutType::VerticalOffset: {
      if (currentIndex == k_outsideIndex &&
          ((direction.isUp() && true
            /*m_verticalPosition == VerticalPosition::Superscript*/) ||
           (direction.isDown() && false
            /*m_verticalPosition == VerticalPosition::Subscript*/))) {
        return 0;
      }
      if (currentIndex == 0 &&
          ((direction.isDown() && true
            /*m_verticalPosition == VerticalPosition::Superscript*/) ||
           (direction.isUp() && false
            /*m_verticalPosition == VerticalPosition::Subscript*/)) &&
          positionAtCurrentIndex != PositionInLayout::Middle) {
        return k_outsideIndex;
      }
      return k_cantMoveIndex;
    }
    default:
      assert(currentIndex < node->numberOfChildren());
      assert(currentIndex != k_outsideIndex ||
             positionAtCurrentIndex != PositionInLayout::Middle);
      return k_cantMoveIndex;
  }
}

int CursorMotion::IndexToPointToWhenInserting(const Tree* node) {
  switch (node->layoutType()) {
    case LayoutType::Product:
    case LayoutType::Sum:
      return Parametric::k_lowerBoundIndex;
    case LayoutType::Integral:
      return Integral::k_lowerBoundIndex;
    case LayoutType::Derivative:
    case LayoutType::NthDerivative:
      return Derivative::k_derivandIndex;
    case LayoutType::ListSequence:
      return ListSequence::k_functionIndex;
    case LayoutType::Fraction:
      return RackLayout::IsEmpty(node->child(Fraction::k_numeratorIndex))
                 ? Fraction::k_numeratorIndex
                 : Fraction::k_denominatorIndex;
    default:
      return node->numberOfChildren() > 0 ? 0 : k_outsideIndex;
  }
}

Tree* CursorMotion::DeepChildToPointToWhenInserting(Tree* node) {
  for (Tree* d : node->descendants()) {
    if (d->isRackLayout() && RackLayout::IsEmpty(d)) {
      return d;
    }
    if (d->isAutocompletedPair()) {
      /* If the inserted bracket is temp on the left, do not put cursor
       * inside it so that the cursor is put right when inserting ")". */
      return AutocompletedPair::IsTemporary(d, Side::Left) ? node : d;
    }
  }
  return node;
}

static bool IsEmpty(const Tree* layout) {
  return layout->isRackLayout() && layout->numberOfChildren() == 0;
}

static DeletionMethod StandardDeletionMethodForLayoutContainingArgument(
    int childIndex, int argumentIndex) {
  return childIndex == argumentIndex ? DeletionMethod::DeleteParent
                                     : DeletionMethod::MoveLeft;
}

DeletionMethod CursorMotion::DeletionMethodForCursorLeftOfChild(
    const Tree* node, int childIndex) {
  switch (node->layoutType()) {
    case LayoutType::Point2D:
    case LayoutType::Binomial:
      using namespace TwoRows;
      if (childIndex == k_upperIndex && IsEmpty(node->child(k_lowerIndex))) {
        return DeletionMethod::DeleteParent;
      }
      if (childIndex == k_lowerIndex) {
        return DeletionMethod::TwoRowsLayoutMoveFromLowertoUpper;
      }
      return DeletionMethod::MoveLeft;
    case LayoutType::Fraction:
      return childIndex == Fraction::k_denominatorIndex
                 ? DeletionMethod::FractionDenominatorDeletion
                 : DeletionMethod::MoveLeft;
    case LayoutType::Parenthesis:
    case LayoutType::CurlyBrace:
      if ((childIndex == k_outsideIndex &&
           AutocompletedPair::IsTemporary(node, Side::Right)) ||
          (childIndex == 0 &&
           AutocompletedPair::IsTemporary(node, Side::Left))) {
        return DeletionMethod::MoveLeft;
      }
      return DeletionMethod::AutocompletedBracketPairMakeTemporary;
    case LayoutType::AbsoluteValue:
    case LayoutType::Floor:
    case LayoutType::Ceiling:
    case LayoutType::VectorNorm:
    case LayoutType::Conjugate:
      return StandardDeletionMethodForLayoutContainingArgument(childIndex, 0);
    case LayoutType::Derivative:
    case LayoutType::NthDerivative:
      return StandardDeletionMethodForLayoutContainingArgument(
          childIndex, Derivative::k_derivandIndex);
    case LayoutType::Integral:
      return StandardDeletionMethodForLayoutContainingArgument(
          childIndex, Integral::k_integrandIndex);
    case LayoutType::ListSequence:
      return StandardDeletionMethodForLayoutContainingArgument(
          childIndex, ListSequence::k_functionIndex);
    case LayoutType::SquareRoot:
    case LayoutType::NthRoot:
      return StandardDeletionMethodForLayoutContainingArgument(
          childIndex, NthRoot::k_radicandIndex);
    case LayoutType::VerticalOffset:
      return childIndex == 0 && IsEmpty(node->child(0))
                 ? DeletionMethod::DeleteLayout
                 : DeletionMethod::MoveLeft;
    case LayoutType::Product:
    case LayoutType::Sum:
      return StandardDeletionMethodForLayoutContainingArgument(
          childIndex, Parametric::k_argumentIndex);
    case LayoutType::Matrix:
    case LayoutType::Piecewise: {
      const Grid* grid = Grid::From(node);
      if (childIndex == k_outsideIndex) {
        return DeletionMethod::MoveLeft;
      }

      assert(grid->isEditing());
      int row = grid->rowAtChildIndex(childIndex);
      int column = grid->columnAtChildIndex(childIndex);
      if (row == 0 && column == 0 &&
          grid->numberOfColumns() ==
              Grid::k_minimalNumberOfRowsAndColumnsWhileEditing &&
          grid->numberOfRows() ==
              Grid::k_minimalNumberOfRowsAndColumnsWhileEditing) {
        /* If the top left child is filled and the cursor is left of it, delete
         * the grid and keep the child. */
        return DeletionMethod::DeleteParent;
      }

      bool deleteWholeRow =
          !grid->numberOfRowsIsFixed() && grid->childIsLeftOfGrid(childIndex) &&
          !grid->childIsBottomOfGrid(childIndex) && grid->isRowEmpty(row);
      bool deleteWholeColumn = !grid->numberOfColumnsIsFixed() &&
                               grid->childIsTopOfGrid(childIndex) &&
                               !grid->childIsRightOfGrid(childIndex) &&
                               grid->isColumnEmpty(column);
      if (deleteWholeRow || deleteWholeColumn) {
        /* Pressing backspace at the top of an empty column or a the left of an
         * empty row deletes the whole column/row. */
        return deleteWholeRow && deleteWholeColumn
                   ? DeletionMethod::GridLayoutDeleteColumnAndRow
                   : (deleteWholeRow ? DeletionMethod::GridLayoutDeleteRow
                                     : DeletionMethod::GridLayoutDeleteColumn);
      }

      if (grid->childIsLeftOfGrid(childIndex) && row != 0) {
        return DeletionMethod::GridLayoutMoveToUpperRow;
      }
      return DeletionMethod::MoveLeft;
    }
    default:
      assert((childIndex >= 0 || childIndex == k_outsideIndex) &&
             childIndex < node->numberOfChildren());
      return childIndex == k_outsideIndex ? DeletionMethod::DeleteLayout
                                          : DeletionMethod::MoveLeft;
  }
}

bool CursorMotion::ShouldCollapseSiblingsOnDirection(
    const Tree* node, OMG::HorizontalDirection direction) {
  if (direction.isLeft()) {
    return node->isFractionLayout();
  } else {
    return node->isConjugateLayout() || node->isFractionLayout() ||
           node->isSquareRootLayout() || node->isNthRootLayout() ||
           node->isSquareBracketPair();
  }
}

int CursorMotion::CollapsingAbsorbingChildIndex(
    const Tree* node, OMG::HorizontalDirection direction) {
  return direction.isRight() && node->isFractionLayout() ? 1 : 0;
}

bool CursorMotion::IsCollapsable(const Tree* node, const Tree* root,
                                 OMG::HorizontalDirection direction) {
  switch (node->layoutType()) {
    case LayoutType::Rack:
      return node->numberOfChildren() > 0;
    case LayoutType::Fraction: {
      /* We do not want to absorb a fraction if something else is already being
       * absorbed. This way, the user can write a product of fractions without
       * typing the × sign. */
      int indexOfThis;
      const Tree* parent = root->parentOfDescendant(root, &indexOfThis);
      assert(parent->numberOfChildren() > 1);
      int indexInParent = parent->indexOfChild(node);
      int indexOfAbsorbingSibling =
          indexInParent + (direction.isLeft() ? 1 : -1);
      assert(indexOfAbsorbingSibling >= 0 &&
             indexOfAbsorbingSibling < parent->numberOfChildren());
      const Tree* absorbingSibling = parent->child(indexOfAbsorbingSibling);
      if (absorbingSibling->numberOfChildren() > 0) {
        absorbingSibling = absorbingSibling->child(
            CollapsingAbsorbingChildIndex(node, direction));
      }
      return absorbingSibling->isRackLayout() &&
             Rack::IsEmpty(absorbingSibling);
    }
    case LayoutType::CodePoint: {
      CodePoint codePoint = CodePointLayout::GetCodePoint(node);
      if (codePoint == '+' || codePoint == UCodePointRightwardsArrow ||
          codePoint.isEquationOperator() || codePoint == ',') {
        return false;
      }
      if (codePoint == '-') {
        /* If the expression is like 3ᴇ-200, we want '-' to be collapsable.
         * Otherwise, '-' is not collapsable. */
        int indexOfThis;
        const Tree* parent = root->parentOfDescendant(root, &indexOfThis);
        if (indexOfThis > 0) {
          const Tree* leftBrother = parent->child(indexOfThis - 1);
          if (leftBrother->isCodePointLayout() &&
              CodePointLayout::GetCodePoint(leftBrother) ==
                  UCodePointLatinLetterSmallCapitalE) {
            return true;
          }
        }
        return false;
      }
      bool isMultiplication = codePoint == '*' ||
                              codePoint == UCodePointMultiplicationSign ||
                              codePoint == UCodePointMiddleDot;
      if (isMultiplication) {
        /* We want '*' to be collapsable only if the following brother is not
         * a fraction, so that the user can write intuitively "1/2 * 3/4". */
        int indexOfThis;
        const Tree* parent = root->parentOfDescendant(root, &indexOfThis);
        const Tree* brother;
        if (indexOfThis > 0 && direction.isLeft()) {
          brother = parent->child(indexOfThis - 1);
        } else if (indexOfThis < parent->numberOfChildren() - 1 &&
                   direction.isRight()) {
          brother = parent->child(indexOfThis + 1);
        } else {
          return true;
        }
        return !brother->isFractionLayout();
      }
      return true;
    }
    default:
      return true;
  }
}

}  // namespace PoincareJ
