#ifndef POINCARE_JUNIOR_LAYOUT_CURSOR_MOTION_H
#define POINCARE_JUNIOR_LAYOUT_CURSOR_MOTION_H

#include <omg/directions.h>
#include <poincare_junior/src/memory/tree.h>

namespace PoincareJ {

enum class DeletionMethod {
  DeleteLayout,
  DeleteParent,
  MoveLeft,
  FractionDenominatorDeletion,
  BinomialCoefficientMoveFromKtoN,
  GridLayoutMoveToUpperRow,
  GridLayoutDeleteColumnAndRow,
  GridLayoutDeleteColumn,
  GridLayoutDeleteRow,
  AutocompletedBracketPairMakeTemporary
};

enum class PositionInLayout : uint8_t { Left, Middle, Right };

class CursorMotion {
 public:
  // TODO: Finish these methods implementation.
  static DeletionMethod DeletionMethodForCursorLeftOfChild(const Tree* node,
                                                           int index);
  static int IndexAfterHorizontalCursorMove(const Tree* node,
                                            OMG::HorizontalDirection direction,
                                            int currentIndex,
                                            bool* shouldRedrawLayout);
  static int IndexAfterVerticalCursorMove(
      const Tree* node, OMG::VerticalDirection direction, int currentIndex,
      PositionInLayout positionAtCurrentIndex, bool* shouldRedrawLayout);

  static bool IsCollapsable(const Tree* node, const Tree* root,
                            int* numberOfOpenParenthesis,
                            OMG::HorizontalDirection direction);

  static bool ShouldCollapseSiblingsOnDirection(
      const Tree* node, OMG::HorizontalDirection direction);
  static int CollapsingAbsorbingChildIndex(const Tree* node,
                                           OMG::HorizontalDirection direction);

 private:
  constexpr static int k_outsideIndex = -1;
  constexpr static int k_cantMoveIndex = -2;
};
}  // namespace PoincareJ

#endif
