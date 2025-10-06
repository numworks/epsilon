#pragma once

#include <omg/directions.h>
#include <omg/unreachable.h>
#include <poincare/preferences.h>
#include <poincare/src/memory/tree.h>
#include <poincare/src/memory/tree_ref.h>
#include <poincare/symbol_context.h>

#include "cursor_motion.h"
#include "empty_rectangle.h"
#include "k_tree.h"
#include "layout_selection.h"
#include "rack_layout.h"
#include "render.h"

namespace Poincare::Internal {

/* There are two main cursor classes:
 *  - LayoutCursor has all the method that doesn't need to modify the underlying
 *    Layout. This underlying layout may leave anywhere.
 *  - TreeStackCursor must point to a layout in the TreeStack and is able to
 *    alter it to perform insertions and backspace.
 *    The usual cursor motion move is in TreeStackCursor since it can trigger a
 *    beautification and may alter the layout.
 */

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
  const Rack* rootRack() const { return protectedRootRack(); }
  const Rack* cursorRack() const { return protectedCursorRack(); }
  Rack* rootRack() {
    return const_cast<Rack*>(
        const_cast<const LayoutCursor*>(this)->protectedRootRack());
  }
  Rack* cursorRack() {
    return const_cast<Rack*>(
        const_cast<const LayoutCursor*>(this)->protectedCursorRack());
  }

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

  void stopSelecting() { m_startOfSelection = -1; }

  /* This moves the cursor to a location that will stay valid after exiting the
   * field. Currently only used to move the cursor from grid gray squares to
   * grid normal squares when returning from calculation's history. */
  void prepareForExitingPosition();

  bool isAtNumeratorOfEmptyFraction() const;

  SimpleLayoutCursor simpleCursor() const {
    return {.rack = cursorRack(), .position = position()};
  }
  int startOfSelection() const { return m_startOfSelection; }

 protected:
  virtual const Rack* protectedRootRack() const = 0;
  virtual const Rack* protectedCursorRack() const = 0;
  virtual void setCursorRack(Rack* rack) = 0;
  void setCursorRack(Rack* rack, int childIndex, OMG::HorizontalDirection side);
  int cursorRackOffset() const {
    return cursorRack()->block() - rootRack()->block();
  }

  bool isOnEmptySquare() const;

  Layout* leftLayout();
  const Layout* leftLayout() const;
  Layout* rightLayout();
  const Layout* rightLayout() const;
  Tree* parentLayout(int* index);
  const Tree* parentLayout(int* index) const;

  int leftmostPosition() const { return 0; }
  int rightmostPosition() const { return cursorRack()->numberOfChildren(); }

  void privateStartSelecting() { m_startOfSelection = m_position; }

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

  void setCursorRack(Rack* rack) override { assert(false); };

 private:
  const Rack* protectedRootRack() const override { return m_root; }
  const Rack* protectedCursorRack() const override { return m_cursor; }

  Rack* m_root;
  Rack* m_cursor;
};

template <class Cursor>
class AddEmptyLayoutHelpers {
  void insertLayout(const Tree* l, const Poincare::SymbolContext& symbolContext,
                    bool forceRight, bool forceLeft,
                    bool collapseSiblings = true) {
    // Up cast to the template class that should be a cursor
    static_cast<Cursor&>(*this).insertLayout(l, symbolContext, forceRight,
                                             forceLeft, collapseSiblings);
  }

 public:
  void addEmptyMatrixLayout(const Poincare::SymbolContext& symbolContext) {
    insertLayout(KEmptyMatrixL, symbolContext, false, false);
  }
  void addEmptyPowerLayout(const Poincare::SymbolContext& symbolContext) {
    insertLayout(KSuperscriptL(""_l), symbolContext, false, false);
  }
  void addEmptySquareRootLayout(const Poincare::SymbolContext& symbolContext) {
    insertLayout(KSqrtL(""_l), symbolContext, false, false);
  }
  void addEmptySquarePowerLayout(const Poincare::SymbolContext& symbolContext) {
    /* Force the cursor right of the layout. */
    insertLayout(KSuperscriptL("2"_l), symbolContext, true, false);
  }
  void addEmptyNthRootLayout(const Poincare::SymbolContext& symbolContext) {
    insertLayout(KRootL(""_l, ""_l), symbolContext, false, false);
  }
  void addEmptyExponentialLayout(const Poincare::SymbolContext& symbolContext) {
    insertLayout("e"_l ^ KSuperscriptL(""_l), symbolContext, false, false);
  }
  void addEmptyLogarithmWithBase10Layout(
      const Poincare::SymbolContext& symbolContext) {
    const Tree* l =
        SharedPreferences->logarithmBasePosition() ==
                Preferences::LogarithmBasePosition::TopLeft
            ? KPrefixSuperscriptL("10"_l) ^ "log"_l ^
                  KParenthesesRightTempL(""_l)
            : "log"_l ^ KSubscriptL("10"_l) ^ KParenthesesRightTempL(""_l);
    insertLayout(l, symbolContext, false, false);
  }
  void addEmptyTenPowerLayout(const Poincare::SymbolContext& symbolContext) {
    insertLayout("×10"_l ^ KSuperscriptL(""_l), symbolContext, false, false);
  }
  void addFractionLayoutAndCollapseSiblings(
      const Poincare::SymbolContext& symbolContext) {
    insertLayout(KFracL(""_l, ""_l), symbolContext, false, false);
  }
  void addMixedFractionLayout(const Poincare::SymbolContext& symbolContext) {
    insertLayout(KFracL(""_l, ""_l), symbolContext, false, true, false);
  }
};

class TreeStackCursor : public LayoutCursor,
                        public AddEmptyLayoutHelpers<TreeStackCursor> {
  friend class PoolLayoutCursor;

 public:
  TreeStackCursor() : LayoutCursor(0, -1) {}
  TreeStackCursor(int position, int startOfSelection, int cursorOffset)
      : LayoutCursor(position, startOfSelection) {
    if (cursorOffset != -1) {
      setCursorRack(
          Rack::From(Tree::FromBlocks(rootRack()->block() + cursorOffset)));
    }
  }

  /* Motion */
  // Return false if could not move
  bool move(
      OMG::Direction direction, bool selecting, bool* shouldRedrawLayout,
      const Poincare::SymbolContext& symbolContext = EmptySymbolContext{});
  bool moveMultipleSteps(
      OMG::Direction direction, int step, bool selecting,
      bool* shouldRedrawLayout,
      const Poincare::SymbolContext& symbolContext = EmptySymbolContext{});

  /* Insertion */
  void insertText(
      const char* text,
      const Poincare::SymbolContext& symbolContext = EmptySymbolContext{},
      bool forceRight = false, bool forceLeft = false, bool linearMode = false);
  void insertLayout(
      const Tree* l,
      const Poincare::SymbolContext& symbolContext = EmptySymbolContext{},
      bool forceRight = false, bool forceLeft = false,
      bool collapseSiblings = true);

  /* Deletion */
  void performBackspace(
      const Poincare::SymbolContext& symbolContext = EmptySymbolContext{});

 private:
  /* LayoutCursor */
  const Rack* protectedRootRack() const override {
    return static_cast<const Rack*>(
        Tree::FromBlocks(SharedTreeStack->firstBlock()));
  }
  const Rack* protectedCursorRack() const override {
    return static_cast<const Rack*>(static_cast<Tree*>(m_cursorRackRef));
  }

  void balanceAutocompletedBracketsAndKeepAValidCursor();
  void deleteAndResetSelection(const Poincare::SymbolContext& symbolContext,
                               const void* nullptrData);
  void privateDelete(DeletionMethod deletionMethod,
                     bool deletionAppliedToParent);
  void setCursorRack(Rack* rack) override { m_cursorRackRef = TreeRef(rack); }
  bool beautifyRightOfRack(Rack* rack,
                           const Poincare::SymbolContext& symbolContext);

  bool horizontalMove(OMG::HorizontalDirection direction);
  bool verticalMove(OMG::VerticalDirection direction);
  bool verticalMoveWithoutSelection(OMG::VerticalDirection direction);

  void collapseSiblingsOfLayout(Layout* l);
  void collapseSiblingsOfLayoutOnDirection(Layout* l,
                                           OMG::HorizontalDirection direction,
                                           int absorbingChildIndex);
  void removeEmptyRowOrColumnOfGridParentIfNeeded();

  TreeRef m_cursorRackRef;
};

class RootedTreeStackCursor : public TreeStackCursor {
 public:
  RootedTreeStackCursor() : m_rootRack(nullptr) {}
  RootedTreeStackCursor(Tree* root, Tree* cursor, int position = 0)
      : Poincare::Internal::TreeStackCursor(
            position, -1, cursor->block() - SharedTreeStack->firstBlock()),
        m_rootRack(root) {}

 private:
  const Rack* protectedRootRack() const override {
    return static_cast<const Rack*>(m_rootRack);
  }

  Tree* m_rootRack;
};

}  // namespace Poincare::Internal
