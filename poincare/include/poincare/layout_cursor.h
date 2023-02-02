#ifndef POINCARE_LAYOUT_CURSOR_H
#define POINCARE_LAYOUT_CURSOR_H

#include <omg/enums.h>
#include <poincare/empty_rectangle.h>
#include <poincare/layout.h>
#include <poincare/layout_selection.h>

namespace Poincare {

class LayoutCursor final {
public:
  constexpr static KDCoordinate k_cursorWidth = 1;

  LayoutCursor(Layout layoutR, int position = 0) :
    m_layout(layoutR),
    m_position(position),
    m_startOfSelection(-1)
  {}

  LayoutCursor() : LayoutCursor(Layout()) {}

  // Definition
  bool isUninitialized() const { return m_layout.isUninitialized(); }
  bool isValid() const { return m_layout.isUninitialized() || (m_position >= 0 && m_position <= m_layout.numberOfChildren()); }

  // Getters and setters
  Layout layout() { return m_layout; }
  int position() const { return m_position; }
  bool isSelecting() const { return m_startOfSelection >= 0; }
  LayoutSelection selection() const { return isSelecting() ? LayoutSelection(m_layout, m_startOfSelection, m_position) : LayoutSelection(); }

  void setTo(LayoutCursor * other) {
     m_layout = other->m_layout;
     m_position = other->m_position;
     m_startOfSelection = other->m_startOfSelection;
  }

  /* Position and size */
  KDCoordinate cursorHeight(KDFont::Size font);
  KDPoint cursorAbsoluteOrigin(KDFont::Size font);

  /* Move */
  // Return false if could not move
  bool move(OMG::Direction direction, bool selecting, bool * shouldRedrawLayout);
  bool moveMultipleSteps(OMG::Direction direction, int step, bool selecting, bool * shouldRedrawLayout);

  /* Layout insertion */
  void insertLayoutAtCursor(Layout layout, Context * context, bool forceRight = false);
  void addEmptyExponentialLayout(Context * context);
  void addEmptyMatrixLayout(Context * context);
  void addEmptyPowerLayout(Context * context);
  void addEmptySquareRootLayout(Context * context);
  void addEmptySquarePowerLayout(Context * context);
  void addEmptyTenPowerLayout(Context * context);
  void addFractionLayoutAndCollapseSiblings(Context * context);
  void insertText(const char * text, Context * context, bool forceCursorRightOfText = false, bool forceCursorLeftOfText = false);

  /* Layout deletion */
  void performBackspace();
  void deleteAndResetSelection();

  void stopSelecting();

  /* Set empty rectangle visibility and gray rectangle in grids */
  bool willExitCurrentPosition();
  bool didEnterCurrentPosition();

  bool isAtNumeratorOfEmptyFraction() const;

private:
  Layout leftLayout();
  Layout rightLayout();
  Layout layoutToFit(KDFont::Size font);

  bool privateHorizontalMove(OMG::HorizontalDirection direction, bool * shouldRedrawLayout);
  bool privateVerticalMove(OMG::VerticalDirection direction, bool * shouldRedrawLayout);

  void privateStartSelecting();

  bool setEmptyRectangleVisibility(EmptyRectangle::State state);
  void invalidateSizesAndPositions();

  Layout m_layout;
  /* If the layout is horizontal, the cursor is left of the child at index
   * m_position. If m_position == layout.numberOfChildren(), the cursor is
   * on the right of the horizontal layout.
   * Ex: l = HorizontalLayout("01234")
   *     -> m_position == 0 -> "|01234"
   *     -> m_position == 2 -> "01|234"
   *     -> m_position == 5 -> "01234|"
   *
   * If the layout is not horizontal, and its parent is not horizontal either,
   * the cursor is either left or right of the layout.
   * m_position should only be 0 or 1.
   * Ex: l = CodePoint("A")
   *     -> m_position == 0 -> "|A"
   *     -> m_position == 1 -> "A|"
   * */
  int m_position;
  /* -1 if no current selection. If m_startOfSelection >= 0, the selection is
   * between m_startOfSelection and m_position */
  int m_startOfSelection;
};

}

#endif
