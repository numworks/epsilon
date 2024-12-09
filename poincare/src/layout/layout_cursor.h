#ifndef POINCARE_LAYOUT_CURSOR_H
#define POINCARE_LAYOUT_CURSOR_H

#include <omg/directions.h>
#include <omg/unreachable.h>
#include <poincare/layout.h>
#include <poincare/old/context.h>
#include <poincare/src/memory/tree.h>
#include <poincare/src/memory/tree_ref.h>

#include "cursor_motion.h"
#include "empty_rectangle.h"
#include "layout_selection.h"
#include "rack_layout.h"
#include "render.h"

namespace Poincare::Internal {

/* The LayoutCursor has 3 main attributes:
 *   - m_rootLayout: the root rack Layout (only in LayoutBufferCursor)
 *   - m_cursorRack: the rack Tree (descendant of the root rack tree) in which
 *                   the cursor is
 *   - m_position: the index of the child of m_cursorRack. The cursor is left of
 *                 that child. If m_position == m_cursorRack->numberOfChildren()
 *                 the cursor is on the right of the last child of m_cursorRack.
 *
 * Ex: l = "01234"_l
 *     -> m_position == 0 -> "|01234"
 *     -> m_position == 2 -> "01|234"
 *     -> m_position == 5 -> "01234|"
 * */

class LayoutCursor {
 public:
  constexpr static KDCoordinate k_cursorWidth = 1;
  // Cursor navigation // TODO: Do not duplicate them everywhere
  constexpr static int k_outsideIndex = -1;
  constexpr static int k_cantMoveIndex = -2;

  LayoutCursor(int position, int startOfSelection)
      : m_position(position), m_startOfSelection(startOfSelection) {}

  // Definition
  bool isUninitialized() const { return cursorRack() == nullptr; }

  // Getters and setters
  virtual Rack* rootRack() const = 0;
  virtual Rack* cursorRack() const = 0;
  void moveCursorToLayout(Tree* l, OMG::HorizontalDirection sideOfLayout);
  int position() const { return m_position; }
  void setPosition(int position) { m_position = position; }
  bool isSelecting() const { return m_startOfSelection >= 0; }

  // Warning: LayoutSelection contains a Tree* and must be used right away
  LayoutSelection selection() const {
    return isSelecting()
               ? LayoutSelection(cursorRack(), m_startOfSelection, m_position)
               : LayoutSelection();
  }

  void safeSetPosition(int position) { setPosition(position); }  // TODO_PCJ

  /* Position and size */
  KDCoordinate cursorHeight(KDFont::Size font) const;
  KDPoint cursorAbsoluteOrigin(KDFont::Size font) const;
  KDPoint middleLeftPoint(KDFont::Size font) const;
  KDCoordinate cursorBaseline(KDFont::Size font) const;

  /* Move */
  // Return false if could not move
  bool move(OMG::Direction direction, bool selecting, bool* shouldRedrawLayout,
            Poincare::Context* context = nullptr);
  bool moveMultipleSteps(OMG::Direction direction, int step, bool selecting,
                         bool* shouldRedrawLayout,
                         Poincare::Context* context = nullptr);

  /* Layout deletion */
  void stopSelecting() { m_startOfSelection = -1; }

  /* This moves the cursor to a location that will stay valid after exiting the
   * field. Currently only used to move the cursor from grid gray squares to
   * grid normal squares when returning from calculation's history. */
  void prepareForExitingPosition();

  bool isAtNumeratorOfEmptyFraction() const;

#if 0
  static int RightmostPossibleCursorPosition(Layout l);
#endif

  SimpleLayoutCursor simpleCursor() const {
    return {.rack = cursorRack(), .position = position()};
  }
  int startOfSelection() const { return m_startOfSelection; }

 protected:
  virtual void setCursorRack(Rack* rack) = 0;
  void setCursorRack(Rack* rack, int childIndex, OMG::HorizontalDirection side);
  int cursorRackOffset() const {
    return cursorRack()->block() - rootRack()->block();
  }

  bool isOnEmptySquare() const;

  Layout* leftLayout() const;
  Layout* rightLayout() const;
  Tree* parentLayout(int* index) const;

  int leftmostPosition() const { return 0; }
  int rightmostPosition() const { return cursorRack()->numberOfChildren(); }
  bool horizontalMove(OMG::HorizontalDirection direction);
  bool verticalMove(OMG::VerticalDirection direction);
  bool verticalMoveWithoutSelection(OMG::VerticalDirection direction);

  void privateStartSelecting() { m_startOfSelection = m_position; }
  virtual void invalidateSizesAndPositions() {}
  void removeEmptyRowOrColumnOfGridParentIfNeeded();

  void collapseSiblingsOfLayout(Layout* l);
  void collapseSiblingsOfLayoutOnDirection(Layout* l,
                                           OMG::HorizontalDirection direction,
                                           int absorbingChildIndex);

  virtual bool beautifyRightOfRack(Rack* rack, Poincare::Context* context) = 0;

  // Cursor's horizontal position
  int m_position;
  /* -1 if no current selection. If m_startOfSelection >= 0, the selection is
   * between m_startOfSelection and m_position */
  int m_startOfSelection;
};

/* Simple TreeCursor when a simple const copy of a cursor is needed. The layout
 * should not be modified. */
class TreeCursor final : public LayoutCursor {
 public:
  TreeCursor(Rack* root, Rack* cursor, int position)
      : LayoutCursor(position, -1), m_root(root), m_cursor(cursor) {}

  TreeCursor(const LayoutCursor& other)
      : LayoutCursor(other.position(), other.startOfSelection()),
        m_root(other.rootRack()),
        m_cursor(other.cursorRack()) {}

  Rack* rootRack() const override { return m_root; }
  Rack* cursorRack() const override { return m_cursor; }
  void setCursorRack(Rack* rack) override { assert(false); };
  bool beautifyRightOfRack(Rack* rack, Poincare::Context* context) override {
    OMG::unreachable();
  }

 private:
  Rack* m_root;
  Rack* m_cursor;
};

class TreeStackCursor final : public LayoutCursor {
  friend class PoolLayoutCursor;
  friend class InputBeautification;

 public:
  TreeStackCursor(int position, int startOfSelection, int cursorOffset)
      : LayoutCursor(position, startOfSelection) {
    if (cursorOffset != -1) {
      setCursorRack(
          Rack::From(Tree::FromBlocks(rootRack()->block() + cursorOffset)));
    }
  }

  /* The private API has structs because it is used through
   * PoolLayoutCursor::execute. The public API is a more convenient wrapper
   * around the private one. */
  void insertText(const char* text, Poincare::Context* context = nullptr,
                  bool forceRight = false, bool forceLeft = false,
                  bool linearMode = false) {
    InsertTextContext insertTextContext{text, forceRight, forceLeft,
                                        linearMode};
    insertText(context, &insertTextContext);
  }
  void insertLayout(const Tree* l, Poincare::Context* context = nullptr,
                    bool forceRight = false, bool forceLeft = false,
                    bool collapseSiblings = true) {
    InsertLayoutContext insertLayoutContext{l, forceRight, forceLeft,
                                            collapseSiblings};
    insertLayout(context, &insertLayoutContext);
  }
  void performBackspace() { performBackspace(nullptr, nullptr); }

  Rack* rootRack() const override {
    return static_cast<Rack*>(Tree::FromBlocks(SharedTreeStack->firstBlock()));
  }
  Rack* cursorRack() const override {
    return static_cast<Rack*>(static_cast<Tree*>(m_cursorRackRef));
  }

 private:
  // TreeStackCursor Actions
  void performBackspace(Poincare::Context* context, const void* nullptrData);
  void deleteAndResetSelection(Poincare::Context* context,
                               const void* nullptrData);
  struct InsertLayoutContext {
    const Tree* m_tree;
    bool m_forceRight = false;
    bool m_forceLeft = false;
    bool m_collapseSiblings = true;
  };
  void insertLayout(Poincare::Context* context,
                    const void* insertLayoutContext);
  struct InsertTextContext {
    const char* m_text;
    bool m_forceRight, m_forceLeft, m_linearMode;
  };
  void insertText(Poincare::Context* context, const void* insertTextContext);
  void balanceAutocompletedBracketsAndKeepAValidCursor();

  void privateDelete(DeletionMethod deletionMethod,
                     bool deletionAppliedToParent);
  void setCursorRack(Rack* rack) override { m_cursorRackRef = TreeRef(rack); }
  struct BeautifyContext {
    int m_rackOffset;
    mutable bool m_shouldRedraw;
  };
  bool beautifyRightOfRack(Rack* rack, Poincare::Context* context) override;
  void beautifyRightOfRackAction(Poincare::Context* context, const void* rack);
  void beautifyLeftAction(Poincare::Context* context, const void* /* no arg */);

  TreeRef m_cursorRackRef;
};

class PoolLayoutCursor final : public LayoutCursor {
 public:
  /* This constructor either set the cursor at the leftMost or rightmost
   * position in the cursorNode. */
  PoolLayoutCursor(
      Poincare::Layout rootLayout = Poincare::Layout(),
      Tree* cursorNode = nullptr,
      OMG::HorizontalDirection sideOfLayout = OMG::Direction::Right())
      : LayoutCursor(0, -1), m_rootLayout(rootLayout) {
    if (cursorNode) {
      moveCursorToLayout(cursorNode, sideOfLayout);
    }
  }

  bool isUninitialized() const {
    return m_rootLayout.isUninitialized() || LayoutCursor::isUninitialized();
  }

  Poincare::Layout rootLayout() { return m_rootLayout; }
  Rack* rootRack() const override {
    return static_cast<Rack*>(const_cast<Tree*>(m_rootLayout.tree()));
  }
  Rack* cursorRack() const override { return rootRack() + m_cursorRack; }

  /* Layout insertion */
  void addEmptyMatrixLayout(Poincare::Context* context);
  void addEmptyPowerLayout(Poincare::Context* context);
  void addEmptySquareRootLayout(Poincare::Context* context);
  void addEmptySquarePowerLayout(Poincare::Context* context);
  void addEmptyExponentialLayout(Poincare::Context* context);
  void addEmptyLogarithmWithBase10Layout(Poincare::Context* context);
  void addEmptyTenPowerLayout(Poincare::Context* context);
  void addFractionLayoutAndCollapseSiblings(Poincare::Context* context);
  void addMixedFractionLayout(Poincare::Context* context);
  void insertText(const char* text, Poincare::Context* context = nullptr,
                  bool forceRight = false, bool forceLeft = false,
                  bool linearMode = false) {
    TreeStackCursor::InsertTextContext insertTextContext{text, forceRight,
                                                         forceLeft, linearMode};
    execute(&TreeStackCursor::insertText, context, &insertTextContext);
  }
  void insertLayout(const Tree* l, Poincare::Context* context = nullptr,
                    bool forceRight = false, bool forceLeft = false,
                    bool collapseSiblings = true) {
    TreeStackCursor::InsertLayoutContext insertLayoutContext{
        l, forceRight, forceLeft, collapseSiblings};
    execute(&TreeStackCursor::insertLayout, context, &insertLayoutContext);
  }
  void deleteAndResetSelection() {
    execute(&TreeStackCursor::deleteAndResetSelection);
  }
  void performBackspace() { execute(&TreeStackCursor::performBackspace); }
  void invalidateSizesAndPositions() override {
    m_rootLayout->invalidAllSizesPositionsAndBaselines();
  }

  void beautifyLeft(Poincare::Context* context);

 private:
  TreeStackCursor createTreeStackCursor() const {
    return TreeStackCursor(m_position, m_startOfSelection, cursorRackOffset());
  }
  void applyTreeStackCursor(TreeStackCursor cursor);
  typedef void (TreeStackCursor::*Action)(Poincare::Context* context,
                                          const void* data);
  void execute(Action action, Poincare::Context* context = nullptr,
               const void* data = nullptr);
  void setCursorRack(Rack* rack) override {
    // Don't use rack here as it may be invalid during execute
    m_cursorRack = rack - Rack::From(static_cast<Tree*>(rootRack()));
  }
  bool beautifyRightOfRack(Rack* rack, Poincare::Context* context) override;

  Poincare::Layout m_rootLayout;
  int m_cursorRack;
};

}  // namespace Poincare::Internal

#endif
