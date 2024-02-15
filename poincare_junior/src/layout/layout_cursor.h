#ifndef POINCARE_JUNIOR_LAYOUT_CURSOR_H
#define POINCARE_JUNIOR_LAYOUT_CURSOR_H

#include <escher/text_field.h>
#include <omg/directions.h>
#include <poincare/junior_layout.h>
#include <poincare_junior/src/layout/empty_rectangle.h>
#include <poincare_junior/src/layout/rack_layout.h>
#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/memory/tree.h>

#include "cursor_motion.h"
#include "layout_selection.h"
#include "render.h"

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

// TODO: reimplement Context
class Context {};

class LayoutCursor {
 public:
  constexpr static KDCoordinate k_cursorWidth = 1;
  // Cursor navigation // TODO: Do not duplicate them everywhere
  constexpr static int k_outsideIndex = -1;
  constexpr static int k_cantMoveIndex = -2;
  // TODO: Find a better value and store it at the right place
  constexpr static int k_layoutBufferSize = 220;
  static_assert(k_layoutBufferSize == Escher::TextField::MaxBufferSize(),
                "Maximal number of layouts in a layout field should be equal "
                "to max number of char in text field");

  LayoutCursor(int position, int startOfSelection)
      : m_position(position), m_startOfSelection(startOfSelection) {}

  // Definition
  bool isUninitialized() const { return cursorNode() == nullptr; }

  // Getters and setters
  virtual Rack* rootNode() const = 0;
  virtual Rack* cursorNode() const = 0;
  void setLayout(Tree* layout, OMG::HorizontalDirection sideOfLayout);
  int position() const { return m_position; }
  void setPosition(int position) { m_position = position; }
  bool isSelecting() const { return m_startOfSelection >= 0; }

  // Warning: LayoutSelection contains a Tree* and must be used right away
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
  KDCoordinate cursorBaseline(KDFont::Size font) const;

  /* Move */
  // Return false if could not move
  bool move(OMG::Direction direction, bool selecting, bool* shouldRedrawLayout,
            Context* context = nullptr);
  bool moveMultipleSteps(OMG::Direction direction, int step, bool selecting,
                         bool* shouldRedrawLayout, Context* context = nullptr);

  /* Layout deletion */
  void stopSelecting() { m_startOfSelection = -1; }

  /* This moves the cursor to a location that will stay valid after exiting the
   * field. Currently only used to move the cursor from grid gray squares to
   * grid normal squares when returning from calculation's history. */
  void prepareForExitingPosition();

  bool isAtNumeratorOfEmptyFraction() const;

#if 0
  static int RightmostPossibleCursorPosition(Layout l);

  void beautifyLeft(Context* context);
#endif

 protected:
  virtual void setCursorNode(Rack* node) = 0;
  void setCursorNode(Rack* node, int childIndex, OMG::HorizontalDirection side);
  int cursorNodeOffset() const {
    return cursorNode()->block() - rootNode()->block();
  }

  LayoutT* leftLayout() const;
  LayoutT* rightLayout() const;
  Tree* parentLayout(int* index) const;

  int leftmostPosition() const { return 0; }
  int rightmostPosition() const { return cursorNode()->numberOfChildren(); }
  bool horizontalMove(OMG::HorizontalDirection direction);
  bool verticalMove(OMG::VerticalDirection direction);
  bool verticalMoveWithoutSelection(OMG::VerticalDirection direction);

  void privateStartSelecting() { m_startOfSelection = m_position; }
  virtual void invalidateSizesAndPositions() {}
  void removeEmptyRowOrColumnOfGridParentIfNeeded();

  void collapseSiblingsOfLayout(Tree* l);
  void collapseSiblingsOfLayoutOnDirection(Tree* l,
                                           OMG::HorizontalDirection direction,
                                           int absorbingChildIndex);

  virtual bool beautifyRightOfRack(Rack* rack, Context* context) = 0;

  // Cursor's horizontal position
  int m_position;
  /* -1 if no current selection. If m_startOfSelection >= 0, the selection is
   * between m_startOfSelection and m_position */
  int m_startOfSelection;
};

class LayoutBufferCursor final : public LayoutCursor {
  friend class InputBeautification;

 public:
  /* This constructor either set the cursor at the leftMost or rightmost
   * position in the layout. */
  LayoutBufferCursor(
      Poincare::JuniorLayout rootLayout = Poincare::JuniorLayout(),
      Tree* layout = nullptr,
      OMG::HorizontalDirection sideOfLayout = OMG::Direction::Right())
      : LayoutCursor(0, -1), m_layout(rootLayout) {
    if (layout) {
      setLayout(layout, sideOfLayout);
    }
  }

  bool isUninitialized() const {
    return m_layout.isUninitialized() || LayoutCursor::isUninitialized();
  }

  Poincare::JuniorLayout layoutBuffer() { return m_layout; }
  Rack* rootNode() const override {
    return static_cast<Rack*>(const_cast<Tree*>(m_layout.tree()));
  }
  Rack* cursorNode() const override { return rootNode() + m_cursorNode; }

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
  void invalidateSizesAndPositions() override {
    m_layout.invalidAllSizesPositionsAndBaselines();
  }

 private:
  class EditionPoolCursor final : public LayoutCursor {
    friend class LayoutBufferCursor;
    friend class InputBeautification;

    EditionPoolCursor(int position, int startOfSelection, int cursorOffset)
        : LayoutCursor(position, startOfSelection) {
      setCursorNode(
          Rack::From(Tree::FromBlocks(rootNode()->block() + cursorOffset)));
    }

    Rack* rootNode() const override {
      return static_cast<Rack*>(
          Tree::FromBlocks(SharedEditionPool->firstBlock()));
    }
    Rack* cursorNode() const override {
      return static_cast<Rack*>(static_cast<Tree*>(m_cursorReference));
    }

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
    void setCursorNode(Rack* node) override {
      m_cursorReference = EditionReference(node);
    }
    bool beautifyRightOfRack(Rack* rack, Context* context) override;
    void beautifyRightOfRackAction(Context* context, const void* rack);

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
  void setCursorNode(Rack* node) override {
    // Don't use node here as it may be invalid during execute
    m_cursorNode = node - Rack::From(static_cast<Tree*>(rootNode()));
  }
  bool beautifyRightOfRack(Rack* rack, Context* context) override;

  // Buffer of cursor's layout
  Poincare::JuniorLayout m_layout;
  // Cursor's node
  int m_cursorNode;
};

}  // namespace PoincareJ

#endif
