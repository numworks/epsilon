#ifndef POINCARE_LAYOUT_CURSOR_MOTION_H
#define POINCARE_LAYOUT_CURSOR_MOTION_H

#include <omg/directions.h>
#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {

enum class DeletionMethod {
  DeleteLayout,
  DeleteParent,
  MoveLeft,
  FractionDenominatorDeletion,
  TwoRowsLayoutMoveFromLowertoUpper,
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
  static DeletionMethod DeletionMethodForCursorLeftOfChild(const Tree* l,
                                                           int index);
  static int IndexAfterHorizontalCursorMove(Tree* l,
                                            OMG::HorizontalDirection direction,
                                            int currentIndex);
  static int IndexAfterVerticalCursorMove(
      Tree* l, OMG::VerticalDirection direction, int currentIndex,
      PositionInLayout positionAtCurrentIndex);

  static int IndexToPointToWhenInserting(const Tree* l);
  static Tree* DeepChildToPointToWhenInserting(Tree* l);

  static bool IsCollapsable(const Tree* l, const Tree* root,
                            OMG::HorizontalDirection direction);

  static bool ShouldCollapseSiblingsOnDirection(
      const Tree* l, OMG::HorizontalDirection direction);
  static int CollapsingAbsorbingChildIndex(const Tree* l,
                                           OMG::HorizontalDirection direction);

 private:
  constexpr static int k_outsideIndex = -1;
  constexpr static int k_cantMoveIndex = -2;
};
}  // namespace Poincare::Internal

#endif
