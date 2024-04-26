#ifndef POINCARE_LAYOUT_CURSOR_H
#define POINCARE_LAYOUT_CURSOR_H

#include <omg/directions.h>

#include "empty_rectangle.h"
#include "layout_selection.h"
#include "old_layout.h"

namespace Poincare {

/* The LayoutCursor has two main attributes: m_layout and m_position
 *
 * If m_layout is an HorizontalLayout, the cursor is left of the child at
 * index m_position. If m_position == layout.numberOfChildren(), the cursor
 * is on the right of the HorizontalLayout.
 * Ex: l = HorizontalLayout("01234")
 *     -> m_position == 0 -> "|01234"
 *     -> m_position == 2 -> "01|234"
 *     -> m_position == 5 -> "01234|"
 *
 * If the layout is not an HorizontalLayout, and its parent is not horizontal
 * either, the cursor is either left or right of the layout.
 * m_position should only be 0 or 1.
 * Ex: l = CodePoint("A")
 *     -> m_position == 0 -> "|A"
 *     -> m_position == 1 -> "A|"
 *
 * WARNING: If a layout has an HorizontalLayout as parent, the cursor must have
 * m_layout = ParentHorizontalLayout.
 *
 * Ex: l = HorizontalLayout("01234") and the cursor is at "012|34"
 * It CAN'T be m_layout = "3" and m_position = 0.
 * It MUST be m_layout = Horizontal("01234") and m_position = 3
 *
 * */

class LayoutCursor final {
 public:
  /* This constructor either set the cursor at the leftMost or rightmost
   * position in the layout. */
  LayoutCursor(OLayout layout,
               OMG::HorizontalDirection sideOfLayout = OMG::Direction::Right())
      : m_position(0), m_startOfSelection(-1) {
    if (!layout.isUninitialized()) {
      setLayout(layout, sideOfLayout);
    }
  }

  LayoutCursor() : LayoutCursor(OLayout()) {}

  // Definition
  bool isUninitialized() const { return m_layout.isUninitialized(); }
  bool isValid() const {
    return !m_layout.deepIsGhost() &&
           (isUninitialized() || (m_position >= leftMostPosition() &&
                                  m_position <= rightmostPosition()));
  }

  // Getters and setters
  OLayout layout() { return m_layout; }
  int position() const { return m_position; }
  bool isSelecting() const { return m_startOfSelection >= 0; }
  LayoutSelection selection() const {
    return isSelecting()
               ? LayoutSelection(m_layout, m_startOfSelection, m_position)
               : LayoutSelection();
  }

  // These will call didEnterCurrentPosition
  void safeSetLayout(OLayout layout, OMG::HorizontalDirection sideOfLayout);
  void safeSetPosition(int position);

  /* Position and size */
  KDCoordinate cursorHeight(KDFont::Size font);
  KDPoint cursorAbsoluteOrigin(KDFont::Size font);
  KDPoint middleLeftPoint(KDFont::Size font);

  /* Move */
  // Return false if could not move
  bool move(OMG::Direction direction, bool selecting, bool* shouldRedrawLayout,
            Context* context = nullptr);
  bool moveMultipleSteps(OMG::Direction direction, int step, bool selecting,
                         bool* shouldRedrawLayout, Context* context = nullptr);

  /* OLayout insertion */
  void insertLayout(OLayout layout, Context* context, bool forceRight = false,
                    bool forceLeft = false);
  void addEmptyExponentialLayout(Context* context);
  void addEmptyMatrixLayout(Context* context);
  void addEmptyPowerLayout(Context* context);
  void addEmptySquareRootLayout(Context* context);
  void addEmptySquarePowerLayout(Context* context);
  void addEmptyTenPowerLayout(Context* context);
  void addFractionLayoutAndCollapseSiblings(Context* context);
  void insertText(const char* text, Context* context,
                  bool forceCursorRightOfText = false,
                  bool forceCursorLeftOfText = false, bool linearMode = false);

  /* OLayout deletion */
  void performBackspace();

  void resetSelection();

  /* Set empty rectangle visibility and gray rectangle in grids. */
  bool didEnterCurrentPosition(LayoutCursor previousPosition = LayoutCursor());

  // Call this if the cursor is disappearing from the field
  bool didExitPosition();
  /* This moves the cursor to a location that will stay valid after exiting the
   * field */
  void prepareForExitingPosition();

  bool isAtNumeratorOfEmptyFraction() const;

  static int RightmostPossibleCursorPosition(OLayout l);

  void beautifyLeft(Context* context);

 private:
  void setLayout(OLayout layout, OMG::HorizontalDirection sideOfLayout);

  OLayout leftLayout();
  OLayout rightLayout();
  OLayout layoutToFit(KDFont::Size font);

  int leftMostPosition() const { return 0; }
  int rightmostPosition() const {
    return m_layout.isHorizontal() ? m_layout.numberOfChildren() : 1;
  }
  bool horizontalMove(OMG::HorizontalDirection direction,
                      bool* shouldRedrawLayout);
  bool verticalMove(OMG::VerticalDirection direction, bool* shouldRedrawLayout);
  bool verticalMoveWithoutSelection(OMG::VerticalDirection direction,
                                    bool* shouldRedrawLayout);

  void privateStartSelecting();

  void deleteAndResetSelection();
  void privateDelete(LayoutNode::DeletionMethod deletionMethod,
                     bool deletionAppliedToParent);

  bool setEmptyRectangleVisibilityAtCurrentPosition(
      EmptyRectangle::State state);
  void removeEmptyRowOrColumnOfGridParentIfNeeded();
  void invalidateSizesAndPositions();

  void collapseSiblingsOfLayout(OLayout l);
  void collapseSiblingsOfLayoutOnDirection(OLayout l,
                                           OMG::HorizontalDirection direction,
                                           int absorbingChildIndex);

  void balanceAutocompletedBracketsAndKeepAValidCursor();

  OLayout m_layout;
  int m_position;
  /* -1 if no current selection. If m_startOfSelection >= 0, the selection is
   * between m_startOfSelection and m_position */
  int m_startOfSelection;
};

}  // namespace Poincare

#endif
