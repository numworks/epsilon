#ifndef POINCARE_JUNIOR_LAYOUT_CURSOR_H
#define POINCARE_JUNIOR_LAYOUT_CURSOR_H

#include <escher/text_field.h>
#include <omg/directions.h>
#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/layout/empty_rectangle.h>
#include <poincare_junior/src/layout/rack_layout.h>
#include <poincare_junior/src/layout/render.h>
#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/memory/tree.h>

#include "cursor_motion.h"
#include "layout_selection.h"

namespace PoincareJ {

/* The LayoutCursor has two main attributes: m_layout and m_position
 *
 * m_layout is an HorizontalLayout, the cursor is left of the child at index
 * m_position. If m_position == layout.numberOfChildren(), the cursor is on the
 * right of the HorizontalLayout.
 *
 * Ex: l = HorizontalLayout("01234")
 *     -> m_position == 0 -> "|01234"
 *     -> m_position == 2 -> "01|234"
 *     -> m_position == 5 -> "01234|"
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

  LayoutCursor(int position, int startOfSelection)
      : m_position(position), m_startOfSelection(startOfSelection) {}

  // Definition
  bool isUninitialized() const { return cursorNode() == nullptr; }

  // Getters and setters
  virtual Tree* rootNode() const = 0;
  virtual Tree* cursorNode() const = 0;
  void setLayout(Tree* layout, OMG::HorizontalDirection sideOfLayout);
  int position() const { return m_position; }
  void setPosition(int position) { m_position = position; }
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

  bool isAtNumeratorOfEmptyFraction() const;

#if 0
  static int RightmostPossibleCursorPosition(Layout l);

  void beautifyLeft(Context* context);
#endif

 protected:
  virtual void setCursorNode(Tree* node) = 0;
  void setCursorNode(Tree* node, int childIndex, OMG::HorizontalDirection side);
  int cursorNodeOffset() const {
    return cursorNode()->block() - rootNode()->block();
  }

  Tree* leftLayout() const;
  Tree* rightLayout() const;
  Tree* parentLayout(int* index) const;

  int leftmostPosition() const { return 0; }
  int rightmostPosition() const { return cursorNode()->numberOfChildren(); }
  bool horizontalMove(OMG::HorizontalDirection direction);
  bool verticalMove(OMG::VerticalDirection direction);
  bool verticalMoveWithoutSelection(OMG::VerticalDirection direction);

  void privateStartSelecting() { m_startOfSelection = m_position; }
#if 0
  bool setEmptyRectangleVisibilityAtCurrentPosition(
      EmptyRectangle::State state);
  void invalidateSizesAndPositions();
#endif
  void removeEmptyRowOrColumnOfGridParentIfNeeded();

  void collapseSiblingsOfLayout(Tree* l);
  void collapseSiblingsOfLayoutOnDirection(Tree* l,
                                           OMG::HorizontalDirection direction,
                                           int absorbingChildIndex);

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
  LayoutBufferCursor(
      Block* layoutBuffer, Tree* layout,
      OMG::HorizontalDirection sideOfLayout = OMG::Direction::Right())
      : LayoutCursor(0, -1), m_layoutBuffer(layoutBuffer) {
    if (layout) {
      setLayout(layout, sideOfLayout);
    }
  }

  Block* layoutBuffer() { return m_layoutBuffer; }
  Tree* rootNode() const override { return Tree::FromBlocks(m_layoutBuffer); }
  Tree* cursorNode() const override { return m_cursorNode; }

  /* Layout insertion */
  void addEmptyMatrixLayout(Context* context);
  void addEmptyPowerLayout(Context* context);
  void addEmptySquareRootLayout(Context* context);
  void addEmptySquarePowerLayout(Context* context);
  void addEmptyExponentialLayout(Context* context);
  void addEmptyTenPowerLayout(Context* context);
  void addFractionLayoutAndCollapseSiblings(Context* context);
  void insertText(const char* text, Context* context, bool forceRight = false,
                  bool forceLeft = false, bool linearMode = false) {
    EditionPoolCursor::InsertTextContext insertTextContext{
        text, forceRight, forceLeft, linearMode};
    execute(&EditionPoolCursor::insertText, context, &insertTextContext);
  }
  void insertLayout(const Tree* tree, Context* context, bool forceRight = false,
                    bool forceLeft = false) {
    EditionPoolCursor::InsertLayoutContext insertLayoutContext{tree, forceRight,
                                                               forceLeft};
    execute(&EditionPoolCursor::insertLayout, context, &insertLayoutContext);
  }
  void deleteAndResetSelection() {
    execute(&EditionPoolCursor::deleteAndResetSelection);
  }
  void performBackspace() { execute(&EditionPoolCursor::performBackspace); }

 private:
  class EditionPoolCursor final : public LayoutCursor {
    friend class LayoutBufferCursor;
    EditionPoolCursor(int position, int startOfSelection, int cursorOffset)
        : LayoutCursor(position, startOfSelection) {
      setCursorNode(Tree::FromBlocks(rootNode()->block() + cursorOffset));
    }

    Tree* rootNode() const override {
      return Tree::FromBlocks(SharedEditionPool->firstBlock());
    }
    Tree* cursorNode() const override { return m_cursorReference; }

    // EditionPoolCursor Actions
    void performBackspace(Context* context, const void* nullptrData);
    void deleteAndResetSelection(Context* context, const void* nullptrData);
    struct InsertLayoutContext {
      const Tree* m_tree;
      bool m_forceRight, m_forceLeft;
    };
    void insertLayout(Context* context, const void* insertLayoutContext);
    struct InsertTextContext {
      const char* m_text;
      bool m_forceRight, m_forceLeft, m_linearMode;
    };
    void insertText(Context* context, const void* insertTextContext);
    void balanceAutocompletedBracketsAndKeepAValidCursor();

    void privateDelete(DeletionMethod deletionMethod,
                       bool deletionAppliedToParent);
    void setCursorNode(Tree* node) override {
      m_cursorReference = EditionReference(node);
      assert(cursorNodeOffset() >= 0 &&
             cursorNodeOffset() < k_layoutBufferSize);
    }

    EditionReference m_cursorReference;
  };
  EditionPoolCursor createEditionPoolCursor() const {
    return EditionPoolCursor(m_position, m_startOfSelection,
                             cursorNodeOffset());
  }
  void applyEditionPoolCursor(EditionPoolCursor cursor);
  typedef void (EditionPoolCursor::*Action)(Context* context, const void* data);
  struct ExecutionContext {
    LayoutBufferCursor* m_cursor;
    Action m_action;
    int m_cursorOffset;
    Context* m_context;
  };
  void execute(Action action, Context* context = nullptr,
               const void* data = nullptr);
  void setCursorNode(Tree* node) override {
    // Don't use node here is it invalid during execute
    m_cursorNode = node;
    assert(cursorNodeOffset() >= 0 && cursorNodeOffset() < k_layoutBufferSize);
  }

  // Buffer of cursor's layout
  Block* m_layoutBuffer;
  // Cursor's node
  Tree* m_cursorNode;
};

}  // namespace PoincareJ

#endif
