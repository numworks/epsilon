#include "cursor_motion.h"

#include <poincare_junior/include/layout.h>

#include "indices.h"

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
    case LayoutType::Parenthesis:  // TODO temporary brackets
    case LayoutType::CurlyBrace:
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

}  // namespace PoincareJ
