#ifndef POINCARE_JUNIOR_LAYOUT_CURSOR_H
#define POINCARE_JUNIOR_LAYOUT_CURSOR_H

#include <omg/directions.h>
#include <poincare_junior/src/layout/empty_rectangle.h>
#include <poincare_junior/src/memory/node.h>
#include <poincare_junior/src/layout/render.h>
#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/include/layout.h>
#include <escher/text_field.h>

#include "layout_selection.h"

namespace PoincareJ {

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

// TODO : reimplement Context
class Context {};

class LayoutCursor {
 public:
  constexpr static KDCoordinate k_cursorWidth = 1;
  // Cursor navigation // TODO : Do not duplicate them everywhere
  constexpr static int k_outsideIndex = -1;
  constexpr static int k_cantMoveIndex = -2;
  // TODO : Find a better value and store it at the right place
  constexpr static int k_layoutBufferSize = 220;
  static_assert(k_layoutBufferSize == Escher::TextField::MaxBufferSize(),
                "Maximal number of layouts in a layout field should be equal "
                "to max number of char in text field");

  LayoutCursor(int position, int startOfSelection) : m_position(position), m_startOfSelection(startOfSelection) {}

  // Definition
  bool isUninitialized() const { return cursorNode().isUninitialized(); }
  bool isValid() const {
    return (isUninitialized() || (m_position >= leftMostPosition() &&
                                  m_position <= rightmostPosition()));
  }

  // Getters and setters
  virtual const TypeBlock * rootBlock() const = 0;
  virtual const Node cursorNode() const = 0;
  void setLayout(const Node layout, OMG::HorizontalDirection sideOfLayout);
  int position() const { return m_position; }
  bool isSelecting() const { return m_startOfSelection >= 0; }
  LayoutSelection selection() const {
    return isSelecting()
               ? LayoutSelection(cursorNode(), m_startOfSelection, m_position)
               : LayoutSelection();
  }

#if 0
  // These will call didEnterCurrentPosition
  void safeSetLayout(Layout layout, OMG::HorizontalDirection sideOfLayout);
  void safeSetPosition(int position);
#endif

  /* Position and size */
  KDCoordinate cursorHeight(KDFont::Size font) const;
  KDPoint cursorAbsoluteOrigin(KDFont::Size font) const;
  KDPoint middleLeftPoint(KDFont::Size font) const;

  /* Move */
  // Return false if could not move
  bool move(OMG::Direction direction, bool selecting, bool* shouldRedrawLayout,
            Context* context = nullptr);
  bool moveMultipleSteps(OMG::Direction direction, int step, bool selecting,
                         bool* shouldRedrawLayout, Context* context = nullptr);

  /* Layout deletion */
  void stopSelecting() { m_startOfSelection = -1; }

  /* Set empty rectangle visibility and gray rectangle in grids. */
#if 0
  bool didEnterCurrentPosition(LayoutCursor previousPosition = LayoutCursor());
  // Call this if the cursor is disappearing from the field
  bool didExitPosition();
#endif

  bool isAtNumeratorOfEmptyFraction() const;

#if 0
  static int RightmostPossibleCursorPosition(Layout l);

  void beautifyLeft(Context* context);
#endif

 protected:
  virtual void setCursorNode(const Node node) = 0;
  int cursorNodeOffset() const { return cursorNode().block() - rootBlock(); }

  const Node leftLayout() const;
  const Node rightLayout() const;
  const Node layoutToFit(KDFont::Size font) const;

  int leftMostPosition() const { return 0; }
  int rightmostPosition() const {
    return Layout::IsHorizontal(cursorNode()) ? cursorNode().numberOfChildren() : 1;
  }
  bool horizontalMove(OMG::HorizontalDirection direction,
                      bool* shouldRedrawLayout);
  bool verticalMove(OMG::VerticalDirection direction, bool* shouldRedrawLayout);
  bool verticalMoveWithoutSelection(OMG::VerticalDirection direction,
                                    bool* shouldRedrawLayout);

  void privateStartSelecting() { m_startOfSelection = m_position; }
#if 0
  bool setEmptyRectangleVisibilityAtCurrentPosition(
      EmptyRectangle::State state);
  void removeEmptyRowOrColumnOfGridParentIfNeeded();
  void invalidateSizesAndPositions();

  void collapseSiblingsOfLayout(Layout l);
  void collapseSiblingsOfLayoutOnDirection(Layout l,
                                           OMG::HorizontalDirection direction,
                                           int absorbingChildIndex);

  void balanceAutocompletedBracketsAndKeepAValidCursor();
#endif

  // Cursor's horizontal position
  int m_position;
  /* -1 if no current selection. If m_startOfSelection >= 0, the selection is
   * between m_startOfSelection and m_position */
  int m_startOfSelection;
};

class LayoutBufferCursor final : public LayoutCursor {
public:
  /* This constructor either set the cursor at the leftMost or rightmost
   * position in the layout. */
  LayoutBufferCursor(TypeBlock * layoutBuffer, Node layout, OMG::HorizontalDirection sideOfLayout = OMG::Direction::Right()) :
      LayoutCursor(0, -1),
      m_layoutBuffer(layoutBuffer) {
    if (!layout.isUninitialized()) {
      setLayout(layout, sideOfLayout);
    }
  }

  TypeBlock * layoutBuffer() { return m_layoutBuffer; }
  const TypeBlock * rootBlock() const override { return m_layoutBuffer; }
  const Node cursorNode() const override { return m_cursorNode; }

  /* Layout insertion */
  void addEmptyExponentialLayout(const Context* context) {
    execute(&EditionPoolCursor::addEmptyExponentialLayout, context);
  }
  void addEmptyMatrixLayout(const Context* context);
  void addEmptyPowerLayout(const Context* context);
  void addEmptySquareRootLayout(const Context* context);
  void addEmptySquarePowerLayout(const Context* context);
  void addEmptyTenPowerLayout(const Context* context) {
    execute(&EditionPoolCursor::addEmptyTenPowerLayout, context);
  }
  void addFractionLayoutAndCollapseSiblings(const Context* context);
  void insertText(const char* text, const Context* context, bool forceRight = false, bool forceLeft = false, bool linearMode = false) {
    EditionPoolCursor::InsertTextContext insertTextContext{text, context, forceRight, forceLeft, linearMode};
    execute(&EditionPoolCursor::insertText, &insertTextContext);
  }
  void insertLayout(const Node tree, const Context* context, bool forceRight = false, bool forceLeft = false) {
    EditionPoolCursor::InsertLayoutContext insertLayoutContext{tree, context, forceRight, forceLeft};
    execute(&EditionPoolCursor::insertLayout, &insertLayoutContext);
  }
  void deleteAndResetSelection() {
    execute(&EditionPoolCursor::deleteAndResetSelection, nullptr);
  }
  void performBackspace() {
    execute(&EditionPoolCursor::performBackspace, nullptr);
  }

private:
  class EditionPoolCursor final : public LayoutCursor {
  friend class LayoutBufferCursor;
    EditionPoolCursor(int position, int startOfSelection, int cursorOffset) : LayoutCursor(position, startOfSelection) {
      setCursorNode(Node(rootBlock() + cursorOffset));
    }

    const TypeBlock * rootBlock() const override {
      return EditionPool::sharedEditionPool()->firstBlock();
    }
    const Node cursorNode() const override { return m_cursorReference; }

    // EditionPoolCursor Actions
    void performBackspace(const void * nullptrData);
    void deleteAndResetSelection(const void * nullptrData);
    void addEmptyExponentialLayout(const void * context);
    void addEmptyTenPowerLayout(const void * context);
    struct InsertLayoutContext {
      const Node m_tree;
      const Context* m_context;
      bool m_forceRight, m_forceLeft;
    };
    void insertLayout(const void * insertLayoutContext);
    struct InsertTextContext {
      const char* m_text;
      const Context* m_context;
      bool m_forceRight, m_forceLeft, m_linearMode;
    };
    void insertText(const void * insertTextContext);
    void privateDelete(Render::DeletionMethod deletionMethod, bool deletionAppliedToParent);
    void setCursorNode(const Node node) override {
      m_cursorReference = EditionReference(node);
      assert(cursorNodeOffset() >= 0 && cursorNodeOffset() < k_layoutBufferSize);
    }

    EditionReference m_cursorReference;
  };
  EditionPoolCursor createEditionPoolCursor() const {
    return EditionPoolCursor(m_position, m_startOfSelection, cursorNodeOffset());
  }
  void applyEditionPoolCursor(EditionPoolCursor cursor);
  typedef void (EditionPoolCursor::*Action)(const void * data);
  struct ExecutionContext {
    LayoutBufferCursor * m_cursor;
    Action m_action;
    int m_cursorOffset;
  };
  bool execute(Action action, const void * data);
  void setCursorNode(const Node node) override {
    m_cursorNode = node;
    assert(cursorNodeOffset() >= 0 && cursorNodeOffset() < k_layoutBufferSize);
  }

  // Buffer of cursor's layout
  TypeBlock * m_layoutBuffer;
  // Cursor's node
  Node m_cursorNode;
};

}  // namespace PoincareJ

#endif
