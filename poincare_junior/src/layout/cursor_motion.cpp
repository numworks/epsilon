#include "cursor_motion.h"

#include <poincare_junior/include/layout.h>

#include "indices.h"

namespace PoincareJ {

int CursorMotion::IndexAfterHorizontalCursorMove(
    const Tree* node, OMG::HorizontalDirection direction, int currentIndex,
    bool* shouldRedraw) {
  int nChildren = node->numberOfChildren();
  if (nChildren == 0) {
    assert(currentIndex == k_outsideIndex);
    return k_outsideIndex;
  }
  if (Layout::IsHorizontal(node)) {
    nChildren += 1;
  }
  if (nChildren == 1) {
    assert(currentIndex == k_outsideIndex || currentIndex == 0);
    return currentIndex == k_outsideIndex ? 0 : k_outsideIndex;
  }
#if 0
  assert(false);
  return k_cantMoveIndex;
#else
  // TODO Implement other layout's logic instead of this dummy generalization
  currentIndex += (direction.isRight() ? 1 : -1);
  if (currentIndex == nChildren) {
    return k_outsideIndex;
  }
  if (currentIndex == k_outsideIndex - 1) {
    return nChildren - 1;
  }
  return currentIndex;
#endif
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
