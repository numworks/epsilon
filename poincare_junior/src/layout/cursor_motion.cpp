#include "cursor_motion.h"

#include <poincare_junior/include/layout.h>

#include "autocompleted_pair.h"
#include "indices.h"
#include "code_point_layout.h"

namespace PoincareJ {

int CursorMotion::IndexAfterHorizontalCursorMove(
    const Tree* node, OMG::HorizontalDirection direction, int currentIndex,
    bool* shouldRedraw) {
  int nChildren = node->numberOfChildren();
  switch (node->layoutType()) {
    case LayoutType::Binomial:
    case LayoutType::Fraction:
      static_assert(Fraction::NumeratorIndex == Binomial::nIndex);
      if (currentIndex == k_outsideIndex) {
        return direction.isRight() ? Fraction::NumeratorIndex
                                   : Fraction::DenominatorIndex;
      }
      return k_outsideIndex;
    case LayoutType::Matrix:
    case LayoutType::Piecewise: {
      if (currentIndex == k_outsideIndex) {
        return direction.isLeft() ? nChildren - 1 : 0;
      }
      if ((direction.isLeft() && Grid::childIsLeftOfGrid(node, currentIndex)) ||
          (direction.isRight() &&
           Grid::childIsRightOfGrid(node, currentIndex))) {
        return k_outsideIndex;
      }
      int step = direction.isLeft() ? -1 : 1;
      return currentIndex + step;
    }
    case LayoutType::Derivative:
    case LayoutType::NthDerivative: {
      using namespace Derivative;
      if (node->layoutType() == LayoutType::Derivative) {
        if (currentIndex == DerivandIndex) {
          // setVariableSlot(direction.isRight() ? VariableSlot::Assignment
          // : VariableSlot::Fraction,
          // shouldRedraw);
          return VariableIndex;
        }
        if (currentIndex == VariableIndex /* &&
                                             m_variableSlot == VariableSlot::Fraction */) {
          return direction.isRight() ? DerivandIndex : k_outsideIndex;
        }
      } else {
        if (currentIndex == DerivandIndex) {
          if (direction.isRight()) {
            // setVariableSlot(VariableSlot::Assignment, shouldRedraw);
            return VariableIndex;
          }
          // setOrderSlot(OrderSlot::Denominator, shouldRedraw);
          return OrderIndex;
        }
        if (currentIndex == VariableIndex /*&&
                                            m_variableSlot == VariableSlot::Fraction*/) {
          if (direction.isRight()) {
            // setOrderSlot(OrderSlot::Denominator, shouldRedraw);
            return OrderIndex;
          }
          return k_outsideIndex;
        }
        if (currentIndex == OrderIndex) {
          if (true /* m_orderSlot == OrderSlot::Denominator */) {
            if (direction.isLeft()) {
              // setVariableSlot(VariableSlot::Fraction, shouldRedraw);
              return VariableIndex;
            }
            return DerivandIndex;
          }
          // assert(m_orderSlot == OrderSlot::Numerator);
          return direction.isRight() ? DerivandIndex : k_outsideIndex;
        }
      }
      if (currentIndex == k_outsideIndex && direction.isRight()) {
        // setVariableSlot(VariableSlot::Fraction, shouldRedrawLayout);
        return VariableIndex;
      }
      if (currentIndex == AbscissaIndex && direction.isLeft()) {
        // setVariableSlot(VariableSlot::Assignment, shouldRedrawLayout);
        return VariableIndex;
      }
      switch (currentIndex) {
        case k_outsideIndex:
          assert(direction.isLeft());
          return AbscissaIndex;
        case AbscissaIndex:
          assert(direction.isRight());
          return k_outsideIndex;
        default: {
          // assert(currentIndex == VariableIndex &&
          // m_variableSlot == VariableSlot::Assignment);
          return direction.isRight() ? AbscissaIndex : DerivandIndex;
        }
      }
    }
    case LayoutType::Integral:
      switch (currentIndex) {
        using namespace Integral;
        case k_outsideIndex:
          return direction.isRight() ? UpperBoundIndex : DifferentialIndex;
        case UpperBoundIndex:
        case LowerBoundIndex:
          return direction.isRight() ? IntegrandIndex : k_outsideIndex;
        case IntegrandIndex:
          return direction.isRight() ? DifferentialIndex : LowerBoundIndex;
        case DifferentialIndex:
          return direction.isRight() ? k_outsideIndex : IntegrandIndex;
      }
    case LayoutType::PtBinomial:
    case LayoutType::PtPermute:
      switch (currentIndex) {
        using namespace PtCombinatorics;
        case k_outsideIndex:
          return direction.isRight() ? nIndex : kIndex;
        case nIndex:
          return direction.isRight() ? kIndex : k_outsideIndex;
        default:
          assert(currentIndex == kIndex);
          return direction.isRight() ? k_outsideIndex : nIndex;
      }
    case LayoutType::ListSequence:
      switch (currentIndex) {
        using namespace ListSequence;
        case k_outsideIndex:
          return direction.isRight() ? FunctionIndex : UpperBoundIndex;
        case FunctionIndex:
          return direction.isRight() ? VariableIndex : k_outsideIndex;
        case VariableIndex:
          return direction.isRight() ? UpperBoundIndex : FunctionIndex;
        default:
          assert(currentIndex == UpperBoundIndex);
          return direction.isRight() ? k_outsideIndex : VariableIndex;
      }
    case LayoutType::NthRoot:
      switch (currentIndex) {
        using namespace NthRoot;
        case k_outsideIndex:
          return direction.isRight() ? IndexIndex : RadicandIndex;
        case IndexIndex:
          return direction.isRight() ? RadicandIndex : k_outsideIndex;
        default:
          assert(currentIndex == RadicandIndex);
          return direction.isRight() ? k_outsideIndex : IndexIndex;
      }
    case LayoutType::Product:
    case LayoutType::Sum:
      switch (currentIndex) {
        using namespace Parametric;
        case k_outsideIndex:
          return direction.isRight() ? UpperBoundIndex : ArgumentIndex;
        case UpperBoundIndex:
          return direction.isRight() ? ArgumentIndex : k_outsideIndex;
        case VariableIndex:
          return direction.isRight() ? LowerBoundIndex : k_outsideIndex;
        case LowerBoundIndex:
          return direction.isRight() ? ArgumentIndex : VariableIndex;
        default:
          assert(currentIndex == ArgumentIndex);
          return direction.isRight() ? k_outsideIndex : LowerBoundIndex;
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
    const Tree* node, OMG::VerticalDirection direction, int currentIndex,
    PositionInLayout positionAtCurrentIndex, bool* shouldRedraw) {
  switch (node->layoutType()) {
    case LayoutType::Binomial: {
      using namespace Binomial;
      if (currentIndex == kIndex && direction.isUp()) {
        return nIndex;
      }
      if (currentIndex == nIndex && direction.isDown()) {
        return kIndex;
      }
      return k_cantMoveIndex;
    }
    case LayoutType::Fraction:
      switch (currentIndex) {
        using namespace Fraction;
        case k_outsideIndex:
          return direction.isUp() ? NumeratorIndex : DenominatorIndex;
        case NumeratorIndex:
          return direction.isUp() ? k_cantMoveIndex : DenominatorIndex;
        default:
          assert(currentIndex == DenominatorIndex);
          return direction.isUp() ? NumeratorIndex : k_cantMoveIndex;
      }
    case LayoutType::Matrix:
    case LayoutType::Piecewise: {
      using namespace Grid;
      if (currentIndex == k_outsideIndex) {
        return k_cantMoveIndex;
      }
      if (direction.isUp() && currentIndex >= NumberOfColumns(node)) {
        return currentIndex - NumberOfColumns(node);
      }
      if (direction.isDown() &&
          currentIndex < node->numberOfChildren() - NumberOfColumns(node)) {
        return currentIndex + NumberOfColumns(node);
      }
      return k_cantMoveIndex;
    }
    case LayoutType::Derivative:
    case LayoutType::NthDerivative: {
      using namespace Derivative;
      if (node->isDerivative()) {
        if (direction.isDown() && currentIndex == DerivandIndex &&
            positionAtCurrentIndex == PositionInLayout::Left) {
          // setVariableSlot(VariableSlot::Fraction, shouldRedraw);
          return VariableIndex;
        }
        if (direction.isUp() && currentIndex == VariableIndex && true
            /*m_variableSlot == VariableSlot::Fraction*/) {
          return DerivandIndex;
        }
      } else {
        if (direction.isUp() && currentIndex == VariableIndex && true
            /*m_variableSlot == VariableSlot::Fraction*/) {
          // setOrderSlot(positionAtCurrentIndex == PositionInLayout::Right
          // ? OrderSlot::Denominator
          // : OrderSlot::Numerator,
          // shouldRedraw);
          return OrderIndex;
        }
        if (direction.isUp() &&
            ((currentIndex == DerivandIndex &&
              positionAtCurrentIndex == PositionInLayout::Left) ||
             (currentIndex == OrderIndex && true
              /*m_orderSlot == OrderSlot::Denominator*/))) {
          // setOrderSlot(OrderSlot::Numerator, shouldRedrawLayout);
          return OrderIndex;
        }
        if (direction.isDown() &&
            ((currentIndex == DerivandIndex &&
              positionAtCurrentIndex == PositionInLayout::Left) ||
             (currentIndex == OrderIndex && false
              /*m_orderSlot == OrderSlot::Numerator*/))) {
          // setOrderSlot(OrderSlot::Denominator, shouldRedraw);
          return OrderIndex;
        }
        if (direction.isDown() && currentIndex == OrderIndex &&
            true
            /*m_orderSlot == OrderSlot::Denominator*/
            && positionAtCurrentIndex == PositionInLayout::Left) {
          // setVariableSlot(VariableSlot::Fraction, shouldRedraw);
          return VariableIndex;
        }
      }
      if (direction.isUp() && currentIndex == VariableIndex && false
          /*m_variableSlot == VariableSlot::Assignment*/) {
        return DerivandIndex;
      }
      if (direction.isDown() && currentIndex == DerivandIndex &&
          positionAtCurrentIndex == PositionInLayout::Right) {
        return AbscissaIndex;
      }
      return k_cantMoveIndex;
    }
    case LayoutType::Integral: {
      using namespace Integral;
      if (currentIndex == IntegrandIndex &&
          positionAtCurrentIndex == PositionInLayout::Left) {
        return direction.isUp() ? UpperBoundIndex : LowerBoundIndex;
      }
      if (currentIndex == UpperBoundIndex && direction.isDown()) {
        return LowerBoundIndex;
      }
      if (currentIndex == LowerBoundIndex && direction.isUp()) {
        return UpperBoundIndex;
      }
      return k_cantMoveIndex;
    }
    case LayoutType::PtBinomial:
    case LayoutType::PtPermute: {
      using namespace PtCombinatorics;
      if (direction.isUp() &&
          (currentIndex == kIndex ||
           (currentIndex == k_outsideIndex &&
            positionAtCurrentIndex == PositionInLayout::Left))) {
        return nIndex;
      }
      if (direction.isDown() &&
          (currentIndex == nIndex ||
           (currentIndex == k_outsideIndex &&
            positionAtCurrentIndex == PositionInLayout::Right))) {
        return kIndex;
      }
      return k_cantMoveIndex;
    }
    case LayoutType::NthRoot: {
      using namespace NthRoot;
      if (direction.isUp() &&
          positionAtCurrentIndex == PositionInLayout::Left &&
          (currentIndex == k_outsideIndex || currentIndex == RadicandIndex)) {
        return IndexIndex;
      }
      if (direction.isDown() && currentIndex == IndexIndex &&
          positionAtCurrentIndex != PositionInLayout::Middle) {
        return positionAtCurrentIndex == PositionInLayout::Right
                   ? RadicandIndex
                   : k_outsideIndex;
      }
      return k_cantMoveIndex;
    }
    case LayoutType::Product:
    case LayoutType::Sum: {
      using namespace Parametric;
      if (direction.isUp() &&
          ((currentIndex == VariableIndex || currentIndex == LowerBoundIndex) ||
           (positionAtCurrentIndex == PositionInLayout::Left &&
            (currentIndex == k_outsideIndex ||
             currentIndex == ArgumentIndex)))) {
        return UpperBoundIndex;
      }
      if (direction.isDown() &&
          ((currentIndex == UpperBoundIndex) ||
           (positionAtCurrentIndex == PositionInLayout::Left &&
            (currentIndex == k_outsideIndex ||
             currentIndex == ArgumentIndex)))) {
        return LowerBoundIndex;
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
  }
  assert(currentIndex < node->numberOfChildren());
  assert(currentIndex != k_outsideIndex ||
         positionAtCurrentIndex != PositionInLayout::Middle);
  return k_cantMoveIndex;
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
    case LayoutType::Binomial:
      using namespace Binomial;
      if (childIndex == nIndex && IsEmpty(node->child(kIndex))) {
        return DeletionMethod::DeleteParent;
      }
      if (childIndex == kIndex) {
        return DeletionMethod::BinomialCoefficientMoveFromKtoN;
      }
      return DeletionMethod::MoveLeft;
    case LayoutType::Fraction:
      return childIndex == Fraction::DenominatorIndex
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
          childIndex, Derivative::DerivandIndex);
    case LayoutType::Integral:
      return StandardDeletionMethodForLayoutContainingArgument(
          childIndex, Integral::IntegrandIndex);
    case LayoutType::ListSequence:
      return StandardDeletionMethodForLayoutContainingArgument(
          childIndex, ListSequence::FunctionIndex);
    case LayoutType::SquareRoot:
    case LayoutType::NthRoot:
      return StandardDeletionMethodForLayoutContainingArgument(
          childIndex, NthRoot::RadicandIndex);
    case LayoutType::VerticalOffset:
      return childIndex == 0 && IsEmpty(node->child(0))
                 ? DeletionMethod::DeleteLayout
                 : DeletionMethod::MoveLeft;
    case LayoutType::Product:
    case LayoutType::Sum:
      return StandardDeletionMethodForLayoutContainingArgument(
          childIndex, Parametric::ArgumentIndex);
    case LayoutType::Matrix:
    case LayoutType::Piecewise: {
#if 0
      using namespace Grid;
      if (childIndex == k_outsideIndex) {
        return DeletionMethod::MoveLeft;
      }

      assert(isEditing());
      int row = rowAtChildIndex(node, childIndex);
      int column = columnAtChildIndex(node, childIndex);
      if (row == 0 && column == 0 &&
          NumberOfColumns(node) == k_minimalNumberOfRowsAndColumnsWhileEditing &&
          NumberOfRows(node) == k_minimalNumberOfRowsAndColumnsWhileEditing) {
        /* If the top left child is filled and the cursor is left of it, delete
         * the grid and keep the child. */
        return DeletionMethod::DeleteParent;
      }

      bool deleteWholeRow = !numberOfRowsIsFixed() &&
                            childIsLeftOfGrid(childIndex) &&
                            !childIsBottomOfGrid(childIndex) && isRowEmpty(row);
      bool deleteWholeColumn =
          !numberOfColumnsIsFixed() && childIsTopOfGrid(childIndex) &&
          !childIsRightOfGrid(childIndex) && isColumnEmpty(column);
      if (deleteWholeRow || deleteWholeColumn) {
        /* Pressing backspace at the top of an empty column or a the left of an
         * empty row deletes the whole column/row. */
        return deleteWholeRow && deleteWholeColumn
                   ? DeletionMethod::GridLayoutDeleteColumnAndRow
                   : (deleteWholeRow ? DeletionMethod::GridLayoutDeleteRow
                                     : DeletionMethod::GridLayoutDeleteColumn);
      }

      if (childIsLeftOfGrid(childIndex) && row != 0) {
        return DeletionMethod::GridLayoutMoveToUpperRow;
      }
#endif
      return DeletionMethod::MoveLeft;
    }
    default:
      assert((childIndex >= 0 || childIndex == k_outsideIndex) &&
             childIndex < node->numberOfChildren());
      return childIndex == k_outsideIndex ? DeletionMethod::DeleteLayout
                                          : DeletionMethod::MoveLeft;
  }
}

bool CursorMotion::IsCollapsable(const Tree* node, const Tree* root,
                                 int* numberOfOpenParenthesis,
                                 OMG::HorizontalDirection direction) {
  switch (node->layoutType()) {
    case LayoutType::Rack:
      return node->numberOfChildren() > 0;
    case LayoutType::Fraction: {
      if (*numberOfOpenParenthesis > 0) {
        return true;
      }

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
             Layout::IsEmpty(absorbingSibling);
    }
    case LayoutType::CodePoint: {
      CodePoint codePoint = CodePointLayout::GetCodePoint(node);
      if (*numberOfOpenParenthesis <= 0) {
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
      }
      return true;
    }
    default:
      return true;
  }
}

}  // namespace PoincareJ
