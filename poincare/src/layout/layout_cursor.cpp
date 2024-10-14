#include "layout_cursor.h"

#include <omg/utf8_decoder.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/tree_ref.h>

#include <algorithm>

#include "autocompleted_pair.h"
#include "code_point_layout.h"
#include "grid.h"
#include "indices.h"
#include "input_beautification.h"
#include "k_tree.h"
#include "layout_cursor.h"
#include "poincare/src/memory/tree_stack.h"
#include "rack_layout.h"
#include "render.h"

namespace Poincare::Internal {

KDCoordinate LayoutCursor::cursorHeight(KDFont::Size font) const {
  LayoutSelection currentSelection = selection();
  int left, right;
  if (currentSelection.isEmpty()) {
    left = std::max(leftmostPosition(), m_position - 1);
    right = std::min(rightmostPosition(), m_position + 1);
  } else {
    left = currentSelection.leftPosition();
    right = currentSelection.rightPosition();
  }
  return Render::SizeBetweenIndexes(cursorRack(), font, simpleCursor(), left,
                                    right)
      .height();
}

KDPoint LayoutCursor::cursorAbsoluteOrigin(KDFont::Size font) const {
  KDCoordinate cursorBaseline = 0;
  LayoutSelection currentSelection = selection();
  /* TODO: perf: this method and the related ones cursorHeight, cursorBaseline,
   * middleLeftPoint call Render methods with the same arguments several
   * times. We should add a CursorRect on render or expose the
   * clonedWithRackMemoized tree to build it only once. */
  int left, right;
  if (currentSelection.isEmpty()) {
    left = std::max(leftmostPosition(), m_position - 1);
    right = std::min(rightmostPosition(), m_position + 1);
  } else {
    left = currentSelection.leftPosition();
    right = currentSelection.rightPosition();
  }
  cursorBaseline = Render::BaselineBetweenIndexes(cursorRack(), font,
                                                  simpleCursor(), left, right);
  KDCoordinate cursorYOriginInLayout =
      Render::Baseline(cursorRack(), font, simpleCursor()) - cursorBaseline;
  KDCoordinate cursorXOffset = 0;
  cursorXOffset = Render::SizeBetweenIndexes(cursorRack(), font, simpleCursor(),
                                             0, m_position)
                      .width();
  return Render::AbsoluteOrigin(cursorRack(), rootRack(), font, simpleCursor())
      .translatedBy(KDPoint(cursorXOffset, cursorYOriginInLayout));
}

KDPoint LayoutCursor::middleLeftPoint(KDFont::Size font) const {
  KDPoint origin = cursorAbsoluteOrigin(font);
  return KDPoint(origin.x(), origin.y() + cursorHeight(font) / 2);
}

KDCoordinate LayoutCursor::cursorBaseline(KDFont::Size font) const {
  return Render::Baseline(cursorRack(), font, simpleCursor());
}

static const Tree* mostNestedGridParent(const Tree* l, const Tree* root) {
  const Tree* ancestorGrid = nullptr;
  for (const Tree* ancestor : l->ancestors(root)) {
    if (ancestor->isGridLayout()) {
      ancestorGrid = ancestor;
    }
  }
  return ancestorGrid;
}

bool LayoutCursor::isOnEmptySquare() const {
  return RackLayout::IsEmpty(cursorRack()) ||
         (leftLayout() && leftLayout()->isVerticalOffsetLayout() &&
          VerticalOffset::IsPrefix(leftLayout()) &&
          RackLayout::FindBase(cursorRack(), leftLayout(), position() - 1) ==
              nullptr) ||
         (rightLayout() && rightLayout()->isVerticalOffsetLayout() &&
          VerticalOffset::IsSuffix(rightLayout()) &&
          RackLayout::FindBase(cursorRack(), rightLayout(), position()) ==
              nullptr);
}

/* Move */
bool LayoutCursor::move(OMG::Direction direction, bool selecting,
                        bool* shouldRedrawLayout, Poincare::Context* context) {
  *shouldRedrawLayout = false;
  if (!selecting && isSelecting()) {
    stopSelecting();
    *shouldRedrawLayout = true;
    return true;
  }
  if (selecting && !isSelecting()) {
    privateStartSelecting();
  }
  Rack* oldRack = cursorRack();
  bool moved = false;
  bool wasEmpty = isOnEmptySquare();
  const Tree* oldGridParent = mostNestedGridParent(cursorRack(), rootRack());
  // Perform the actual move
  if (direction.isVertical()) {
    moved = verticalMove(direction);
  } else {
    moved = horizontalMove(direction);
  }
  assert(!*shouldRedrawLayout || moved);
  if (moved) {
    *shouldRedrawLayout =
        selecting || wasEmpty || *shouldRedrawLayout || isOnEmptySquare() ||
        // Redraw to show/hide the empty gray squares of the parent grid
        mostNestedGridParent(cursorRack(), rootRack()) != oldGridParent;
    if (cursorRack() != oldRack) {
      // Beautify the layout that was just left
      *shouldRedrawLayout =
          beautifyRightOfRack(oldRack, context) || *shouldRedrawLayout;
    }
  }

  if (isSelecting() && selection().isEmpty()) {
    stopSelecting();
  }

  if (*shouldRedrawLayout) {
    invalidateSizesAndPositions();
  }
  return moved;
}

void LayoutBufferCursor::beautifyLeft(Poincare::Context* context) {
  execute(&TreeStackCursor::beautifyLeftAction, context, nullptr);
  if (position() > cursorRack()->numberOfChildren() + 1) {
    /* Beautification does not preserve the cursor so its position may be
     * invalid. The other calls to beaufication happen just after we move the
     * cursor to another rack than the one beautified so it only matters
     * here. */
    setPosition(cursorRack()->numberOfChildren() + 1);
  }
  // TODO invalidate only if shouldRedrawLayout
  invalidateSizesAndPositions();
  // TODO factorize with beautifyRightOfRack
}

void LayoutBufferCursor::TreeStackCursor::beautifyLeftAction(
    Poincare::Context* context, const void*) {
  InputBeautification::BeautifyLeftOfCursorBeforeCursorMove(this, context);
}

bool LayoutBufferCursor::beautifyRightOfRack(Rack* rack,
                                             Poincare::Context* context) {
  TreeStackCursor::BeautifyContext ctx{static_cast<int>(rack - cursorRack()),
                                       false};
  execute(&TreeStackCursor::beautifyRightOfRackAction, context, &ctx);
  return ctx.m_shouldRedraw;
}

bool LayoutBufferCursor::TreeStackCursor::beautifyRightOfRack(
    Rack* targetRack, Poincare::Context* context) {
  LayoutBufferCursor::TreeStackCursor tempCursor = *this;
  tempCursor.moveCursorToLayout(targetRack, OMG::Direction::Right());
  return InputBeautification::BeautifyLeftOfCursorBeforeCursorMove(&tempCursor,
                                                                   context);
}

void LayoutBufferCursor::TreeStackCursor::beautifyRightOfRackAction(
    Poincare::Context* context, const void* rackOffset) {
  const BeautifyContext* ctx = static_cast<const BeautifyContext*>(rackOffset);
  Rack* targetRack = cursorRack() + ctx->m_rackOffset;
  ctx->m_shouldRedraw = beautifyRightOfRack(targetRack, context);
}

bool LayoutCursor::moveMultipleSteps(OMG::Direction direction, int step,
                                     bool selecting, bool* shouldRedrawLayout,
                                     Poincare::Context* context) {
  assert(step > 0);
  *shouldRedrawLayout = false;
  for (int i = 0; i < step; i++) {
    bool shouldRedrawLayoutStep;
    bool canMove = move(direction, selecting, &shouldRedrawLayoutStep, context);
    *shouldRedrawLayout |= shouldRedrawLayoutStep;
    if (!canMove) {
      return i > 0;
    }
  }
  return true;
}

static bool IsTemporaryAutocompletedBracketPair(const Tree* l, Side tempSide) {
  return l->isAutocompletedPair() &&
         AutocompletedPair::IsTemporary(l, tempSide);
}

// Return leftParenthesisIndex
static int ReplaceCollapsableLayoutsLeftOfIndexWithParenthesis(Rack* rack,
                                                               int index) {
  int leftParenthesisIndex = index + 1;
  // TODO: Use Iterator
  while (leftParenthesisIndex > 0 &&
         CursorMotion::IsCollapsable(rack->child(leftParenthesisIndex - 1),
                                     rack, OMG::Direction::Left())) {
    leftParenthesisIndex--;
  }
  TreeRef parenthesis = SharedTreeStack->pushParenthesisLayout(false, false);
  TreeRef tempRack = SharedTreeStack->pushRackLayout(0);
  int i = index;
  while (i >= leftParenthesisIndex) {
    TreeRef child = NAry::DetachChildAtIndex(rack, i);
    NAry::AddChildAtIndex(tempRack, child, 0);
    i--;
  }
  NAry::AddChildAtIndex(rack, parenthesis, leftParenthesisIndex);
  return leftParenthesisIndex;
}

/* const Tree* insertion */
void LayoutBufferCursor::TreeStackCursor::insertLayout(
    Poincare::Context* context, const void* data) {
  const InsertLayoutContext* insertLayoutContext =
      static_cast<const InsertLayoutContext*>(data);
  bool forceRight = insertLayoutContext->m_forceRight;
  bool forceLeft = insertLayoutContext->m_forceLeft;

  const Tree* tree = insertLayoutContext->m_tree;

  if (tree->isRackLayout() && Rack::IsEmpty(tree)) {
    return;
  }

  Tree* copy = SharedTreeStack->contains(tree)
                   ? const_cast<Tree*>(insertLayoutContext->m_tree)
                   : tree->cloneTree();
  // We need to keep track of the node which must live in the edition pool
  // TODO: do we need ConstReferences on const Nodes in the pool ?
  TreeRef ref(copy);
  if (!copy->isRackLayout()) {
    copy->cloneNodeAtNode(KRackL.node<1>);
  }

  assert(!forceRight || !forceLeft);
  // - Step 1 - Delete selection
  deleteAndResetSelection(context, nullptr);

  // - Step 2 - Beautify the current layout if needed.
  InputBeautification::BeautificationMethod beautificationMethod =
      InputBeautification::BeautificationMethodWhenInsertingLayout(ref);
  if (beautificationMethod.beautifyIdentifiersBeforeInserting) {
    InputBeautification::BeautifyLeftOfCursorBeforeCursorMove(this, context);
  }

  /* - Step 3 - Add empty row to grid layout if needed
   * When an empty child at the bottom or right of the grid is filled,
   * an empty row/column is added below/on the right.
   */
  int index;
  Tree* parent = parentLayout(&index);
  if (parent && parent->isGridLayout() && RackLayout::IsEmpty(cursorRack())) {
    setCursorRack(Grid::From(parent)->willFillEmptyChildAtIndex(index));
    m_position = 0;
  }

  /* - Step 4 - Close brackets on the left/right
   *
   * For example, if the current layout is "(3+4]|" (where "|"" is the cursor
   * and "]" is a temporary parenthesis), inserting something on the right
   * should make the parenthesis permanent.
   * "(3+4]|" -> insert "x" -> "(3+4)x|"
   *
   * There is an exception to this: If a new parenthesis, temporary on the
   * other side, is inserted, you only want to make the inner brackets
   * permanent.
   *
   * Examples:
   * "(3+4]|" -> insert "[)" -> "(3+4][)|"
   * The newly inserted one is temporary on its other side, so the current
   * bracket is not made permanent.
   * Later at Step 9, balanceAutocompletedBrackets will make it so:
   * "(3+4][)|" -> "(3+4)|"
   *
   * "(1+(3+4]]|" -> insert "[)" -> "(1+(3+4)][)|"
   * The newly inserted one is temporary on its other side, so the current
   * bracket is not made permanent, but its inner bracket is made permanent.
   * Later at Step 9, balanceAutocompletedBrackets will make it so:
   * "(1+(3+4)][)|" -> "(1+3(3+4))|"
   * */
  Tree* leftL = leftLayout();
  Tree* rightL = rightLayout();
  if (leftL && leftL->isAutocompletedPair()) {
    AutocompletedPair::MakeChildrenPermanent(
        leftL, Side::Right,
        leftL->type() != ref->child(0)->type() ||
            !IsTemporaryAutocompletedBracketPair(ref->child(0), Side::Left));
  }
  if (rightL && rightL->isAutocompletedPair()) {
    AutocompletedPair::MakeChildrenPermanent(
        rightL, Side::Left,
        rightL->type() != ref->lastChild()->type() ||
            !IsTemporaryAutocompletedBracketPair(ref->lastChild(),
                                                 Side::Right));
  }

  /* - Step 5 - Add parenthesis around vertical offset
   * To avoid ambiguity between a^(b^c) and (a^b)^c when representing a^b^c,
   * add parentheses to make (a^b)^c. */
  if (ref->child(0)->isVerticalOffsetLayout() &&
      VerticalOffset::IsSuffixSuperscript(ref->child(0))) {
    if (leftL && leftL->isVerticalOffsetLayout() &&
        VerticalOffset::IsSuffixSuperscript(leftL)) {
      // Insert ^c left of a^b -> turn a^b into (a^b)
      int leftParenthesisIndex =
          ReplaceCollapsableLayoutsLeftOfIndexWithParenthesis(
              cursorRack(), cursorRack()->indexOfChild(leftL));
      m_position = leftParenthesisIndex + 1;
    }

    if (rightL && rightL->isVerticalOffsetLayout() &&
        VerticalOffset::IsSuffixSuperscript(rightL) &&
        cursorRack()->indexOfChild(rightL) > 0) {
      // Insert ^b right of a in a^c -> turn a^c into (a)^c
      int leftParenthesisIndex =
          ReplaceCollapsableLayoutsLeftOfIndexWithParenthesis(
              cursorRack(), cursorRack()->indexOfChild(rightL) - 1);
      setCursorRack(cursorRack()->child(leftParenthesisIndex)->child(0));
      m_position = cursorRack()->numberOfChildren();
    }
  }

  // - Step 6 - Find position to point to if layout will be merged
  TreeStackCursor previousCursor = *this;
  TreeRef childToPoint;
  if (ref->numberOfChildren() > 0) {
    childToPoint = (forceRight || forceLeft)
                       ? nullptr
                       : CursorMotion::DeepChildToPointToWhenInserting(ref);
    if (!childToPoint.isUninitialized() &&
        childToPoint->isAutocompletedPair()) {
      childToPoint = childToPoint->child(0);
    }
  }

  // - Step 7 - Insert layout
  int numberOfInsertedChildren = ref->numberOfChildren();
  bool autocompletedPairInserted =
      (numberOfInsertedChildren == 1) && ref->child(0)->isAutocompletedPair();
  TreeRef toCollapse = numberOfInsertedChildren == 1 ? ref->child(0) : nullptr;
  /* AddOrMergeLayoutAtIndex will replace current layout with an
   * HorizontalLayout if needed. With this assert, m_position is guaranteed to
   * be preserved. */
  assert(cursorRack()->isRackLayout() &&
         (cursorRack() == rootRack() ||
          !rootRack()->parentOfDescendant(cursorRack())->isRackLayout()));
  NAry::AddOrMergeChildAtIndex(cursorRack(), ref, m_position);
  assert(cursorRack()->isRackLayout());

  if (!forceLeft) {
    // Move cursor right of inserted children
    m_position += numberOfInsertedChildren;
  }

  /* - Step 8 - Collapse siblings and find position to point to if layout was
   * not merged */
  if (numberOfInsertedChildren == 1 && !autocompletedPairInserted) {
    // ref is undef
    if (insertLayoutContext->m_collapseSiblings) {
      collapseSiblingsOfLayout(Layout::From(toCollapse));
    }
    int indexOfChildToPointTo =
        (forceRight || forceLeft)
            ? k_outsideIndex
            : CursorMotion::IndexToPointToWhenInserting(toCollapse);
    if (indexOfChildToPointTo != k_outsideIndex) {
      childToPoint = toCollapse->child(indexOfChildToPointTo);
    }
  }

  // - Step 9 - Point to required position
  if (!childToPoint.isUninitialized()) {
    moveCursorToLayout(childToPoint, OMG::Direction::Left());
  }

  // - Step 10 - Balance brackets
  balanceAutocompletedBracketsAndKeepAValidCursor();

  // - Step 11 - Beautify after insertion if needed
  if (beautificationMethod.beautifyAfterInserting) {
    InputBeautification::BeautifyLeftOfCursorAfterInsertion(this, context);
  }
}

void LayoutBufferCursor::addEmptyExponentialLayout(Poincare::Context* context) {
  insertLayout("e"_l ^ KSuperscriptL(""_l), context, false, false);
}

void LayoutBufferCursor::addEmptyLogarithmWithBase10Layout(
    Poincare::Context* context) {
  const Tree* l =
      Preferences::SharedPreferences()->logarithmBasePosition() ==
              Preferences::LogarithmBasePosition::TopLeft
          ? KPrefixSuperscriptL("10"_l) ^ "log"_l ^ KParenthesesRightTempL(""_l)
          : "log"_l ^ KSubscriptL("10"_l) ^ KParenthesesRightTempL(""_l);
  insertLayout(l, context, false, false);
}

void LayoutBufferCursor::addEmptyTenPowerLayout(Poincare::Context* context) {
  insertLayout("×10"_l ^ KSuperscriptL(""_l), context, false, false);
}

void LayoutBufferCursor::addEmptyMatrixLayout(Poincare::Context* context) {
  insertLayout(KEmptyMatrixL, context, false, false);
}

void LayoutBufferCursor::addEmptySquareRootLayout(Poincare::Context* context) {
  insertLayout(KSqrtL(""_l), context, false, false);
}

void LayoutBufferCursor::addEmptyPowerLayout(Poincare::Context* context) {
  insertLayout(KSuperscriptL(""_l), context, false, false);
}

void LayoutBufferCursor::addEmptySquarePowerLayout(Poincare::Context* context) {
  /* Force the cursor right of the layout. */
  insertLayout(KSuperscriptL("2"_l), context, true, false);
}

void LayoutBufferCursor::addFractionLayoutAndCollapseSiblings(
    Poincare::Context* context) {
  insertLayout(KFracL(""_l, ""_l), context, false, false);
}

void LayoutBufferCursor::addMixedFractionLayout(Poincare::Context* context) {
  insertLayout(KFracL(""_l, ""_l), context, false, true, false);
}

void LayoutBufferCursor::TreeStackCursor::insertText(Poincare::Context* context,
                                                     const void* data) {
  const InsertTextContext* insertTextContext =
      static_cast<const InsertTextContext*>(data);
  const char* text = insertTextContext->m_text;
  bool forceCursorRightOfText = insertTextContext->m_forceRight;
  bool forceCursorLeftOfText = insertTextContext->m_forceLeft;
  bool linearMode = insertTextContext->m_linearMode;

  UTF8Decoder decoder(text);

  CodePoint codePoint = decoder.nextCodePoint();
  if (codePoint == UCodePointNull) {
    return;
  }

  /* - Step 1 -
   * Read the text from left to right and create an Horizontal layout
   * containing the layouts corresponding to each code point. */
  TreeRef layoutToInsert = KRackL()->cloneTree();
  TreeRef currentLayout = layoutToInsert;
  // This is only used to check if we properly left the last subscript
  int currentSubscriptDepth = 0;

  bool setCursorToFirstEmptyCodePoint =
      linearMode && !forceCursorLeftOfText && !forceCursorRightOfText;

  while (codePoint != UCodePointNull) {
    assert(!codePoint.isCombining());
    CodePoint nextCodePoint = decoder.nextCodePoint();
    if (codePoint == UCodePointEmpty) {
      codePoint = nextCodePoint;
      if (setCursorToFirstEmptyCodePoint) {
        /* To force cursor at first empty code point in linear mode, insert
         * the first half of text now, and then insert the end of the text
         * and force the cursor left of it. */
        assert(currentSubscriptDepth == 0);
        (void)currentSubscriptDepth;
        LayoutBufferCursor::TreeStackCursor::InsertLayoutContext
            insertLayoutContext{layoutToInsert, forceCursorRightOfText,
                                forceCursorLeftOfText};
        insertLayout(context, &insertLayoutContext);
        layoutToInsert = KRackL()->cloneTree();
        currentLayout = layoutToInsert;
        forceCursorLeftOfText = true;
        setCursorToFirstEmptyCodePoint = false;
      }
      if (!linearMode && nextCodePoint == ')') {
        // Skip ) to turn cos(\x11) into cos(] with ] temporary
        codePoint = decoder.nextCodePoint();
        /* TODO: previously cos(\x11) was parsable into a cos with an empty
         * expression argument. It is no more parsable but unlike in the
         * toolbox, we need to keep the textual version as key presses are
         * low-level. As an alternative we may catch keys events that insert a
         * function in the layout field and insert a layout directly. */
      }
      assert(!codePoint.isCombining());
      continue;
    }

    // - Step 1 - Handle code points and brackets
    Tree* newChild;
    TypeBlock bracketType(Type{});
    Side bracketSide;
    if (!linearMode && AutocompletedPair::IsAutoCompletedBracketPairCodePoint(
                           codePoint, &bracketType, &bracketSide)) {
      // Brackets will be balanced later in insertLayout
      newChild = AutocompletedPair::BuildFromBracketType(bracketType);
      AutocompletedPair::SetTemporary(newChild, OtherSide(bracketSide), true);
    } else if (nextCodePoint.isCombining()) {
      newChild = SharedTreeStack->pushCombinedCodePointsLayout(codePoint,
                                                               nextCodePoint);
      nextCodePoint = decoder.nextCodePoint();
    } else {
      newChild = CodePointLayout::Push(codePoint);
    }

    NAry::AddOrMergeChildAtIndex(currentLayout, newChild,
                                 currentLayout->numberOfChildren());
    codePoint = nextCodePoint;
  }
  assert(currentSubscriptDepth == 0);

  // - Step 2 - Inserted the created layout
  LayoutBufferCursor::TreeStackCursor::InsertLayoutContext insertLayoutContext{
      layoutToInsert, forceCursorRightOfText, forceCursorLeftOfText};
  insertLayout(context, &insertLayoutContext);

  // TODO: Restore beautification
}

void LayoutBufferCursor::TreeStackCursor::performBackspace(
    Poincare::Context* context, const void* data) {
  assert(data == nullptr);
  if (isSelecting()) {
    return deleteAndResetSelection(context, nullptr);
  }

  const Tree* leftL = leftLayout();
  if (leftL) {
    DeletionMethod deletionMethod =
        CursorMotion::DeletionMethodForCursorLeftOfChild(leftL, k_outsideIndex);
    privateDelete(deletionMethod, false);
  } else {
    int index;
    const Tree* p = parentLayout(&index);
    if (!p) {
      return;
    }
    DeletionMethod deletionMethod =
        CursorMotion::DeletionMethodForCursorLeftOfChild(p, index);
    privateDelete(deletionMethod, true);
  }
  balanceAutocompletedBracketsAndKeepAValidCursor();
  removeEmptyRowOrColumnOfGridParentIfNeeded();
}

void LayoutBufferCursor::TreeStackCursor::deleteAndResetSelection(
    Poincare::Context* context, const void* data) {
  assert(data == nullptr);
  LayoutSelection selec = selection();
  if (selec.isEmpty()) {
    return;
  }
  int selectionLeftBound = selec.leftPosition();
  int selectionRightBound = selec.rightPosition();
  for (int i = selectionLeftBound; i < selectionRightBound; i++) {
    NAry::RemoveChildAtIndex(m_cursorRackRef, selectionLeftBound);
  }
  m_position = selectionLeftBound;
  stopSelecting();
  removeEmptyRowOrColumnOfGridParentIfNeeded();
}

void LayoutCursor::prepareForExitingPosition() {
  int childIndex;
  Tree* parent = parentLayout(&childIndex);
  if (!parent || !parent->isGridLayout()) {
    return;
  }
  Grid* grid = Grid::From(parent);
  if (!grid->childIsPlaceholder(childIndex)) {
    return;
  }
  /* When exiting a grid, the gray columns and rows will disappear, so
   * before leaving the grid, set the cursor position to a layout that will
   * stay valid when the grid will be re-entered. */
  moveCursorToLayout(grid->child(grid->closestNonGrayIndex(childIndex)),
                     OMG::Direction::Right());
}

bool LayoutCursor::isAtNumeratorOfEmptyFraction() const {
  if (cursorRack()->numberOfChildren() != 0) {
    return false;
  }
  int indexInParent;
  const Tree* parent =
      rootRack()->parentOfDescendant(cursorRack(), &indexInParent);
  return parent && parent->isFractionLayout() && indexInParent == 0 &&
         parent->child(1)->numberOfChildren() == 0;
}

/* Private */

void LayoutCursor::moveCursorToLayout(Tree* cursorNode,
                                      OMG::HorizontalDirection sideOfLayout) {
  stopSelecting();
  if (!cursorNode->isRackLayout()) {
    int indexInParent;
    Rack* parent =
        Rack::From(rootRack()->parentOfDescendant(cursorNode, &indexInParent));
    setCursorRack(parent);
    m_position = indexInParent + (sideOfLayout.isRight());
    return;
  }
  setCursorRack(Rack::From(cursorNode));
  m_position = sideOfLayout.isLeft() ? leftmostPosition() : rightmostPosition();
}

Layout* LayoutCursor::leftLayout() const {
  assert(!isUninitialized());
  return m_position == 0 ? nullptr : cursorRack()->child(m_position - 1);
}

Layout* LayoutCursor::rightLayout() const {
  assert(!isUninitialized());
  if (m_position == cursorRack()->numberOfChildren()) {
    return nullptr;
  }
  return cursorRack()->child(m_position);
}

Tree* LayoutCursor::parentLayout(int* index) const {
  return rootRack()->parentOfDescendant(cursorRack(), index);
}

void LayoutCursor::setCursorRack(Rack* l, int childIndex,
                                 OMG::HorizontalDirection side) {
  moveCursorToLayout(l->child(childIndex), side);
}

bool LayoutCursor::horizontalMove(OMG::HorizontalDirection direction) {
  Tree* nextLayout = nullptr;
  /* Search the nextLayout on the left/right to ask it where
   * the cursor should go when entering from outside.
   *
   * Example in the layout of 3+4/5 :
   * § is the cursor and -> the direction
   *
   *       4
   * 3+§->---
   *       5
   *
   * Here the cursor must move right but should not "jump" over the fraction,
   * so will ask its rightLayout (the fraction), where it should enter
   * (numerator or denominator).
   *
   * Example in the layout of 12+34:
   * § is the cursor and -> the direction
   *
   * 12+§->34
   *
   * Here the cursor will ask its rightLayout (the "3"), where it should go.
   * This will result in the "3" answering "outside", so that the cursor
   * jumps over it.
   * */
  int currentIndexInNextLayout = k_outsideIndex;
  if (direction.isRight()) {
    nextLayout = rightLayout();
  } else {
    nextLayout = leftLayout();
  }

  if (!nextLayout) {
    /* If nextLayout is uninitialized, the cursor is at the left-most or
     * right-most position. It should move to the parent.
     *
     * Example in an integral layout:
     * § is the cursor and -> the direction
     *
     *   / 10§->
     *  /
     *  |
     *  |     1+ln(x) dx
     *  |
     *  /
     * / -10
     *
     * Here the cursor must move right but has no rightLayout. So it will
     * ask its parent what it should do when leaving its upper bound child
     * from the right (go to integrand).
     *
     * Example in a square root layout:
     * § is the cursor and -> the direction
     *  _______
     * √1234§->
     *
     * Here the cursor must move right but has no rightLayout. So it will
     * ask its parent what it should do when leaving its only child from
     * from the right (leave the square root).
     * */
    nextLayout = parentLayout(&currentIndexInNextLayout);
    if (!nextLayout) {
      return false;
    }
  }
  assert(nextLayout && !nextLayout->isRackLayout());

  /* If the cursor is selecting, it should not enter a new layout
   * but select all of it. */
  int newIndex = isSelecting()
                     ? k_outsideIndex
                     : CursorMotion::IndexAfterHorizontalCursorMove(
                           nextLayout, direction, currentIndexInNextLayout);
  assert(newIndex != k_cantMoveIndex);

  if (newIndex != k_outsideIndex) {
    /* Enter the next layout child
     *
     *       4                                        §4
     * 3+§->---          : newIndex = numerator ==> 3+---
     *       5                                         5
     *
     *
     *   / 10§->                                     / 10
     *  /                                           /
     *  |                                           |
     *  |     1+ln(x) dx : newIndex = integrand ==> |     §1+ln(x) dx
     *  |                                           |
     *  /                                           /
     * / -10                                       / -10
     *
     * */
    moveCursorToLayout(
        nextLayout->child(newIndex),
        direction.isLeft() ? OMG::Direction::Right() : OMG::Direction::Left());
    return true;
  }

  /* The new index is outside.
   * If it's not selecting, it can be because there is no child to go into:
   *
   * 12+§->34  : newIndex = outside of the 3    ==> 12+3§4
   *
   *  _______                                        ____ §
   * √1234§->  : newIndex = outside of the sqrt ==> √1234 §
   *
   * If it's selecting, the cursor should always leave the layout and all of
   * it will be selected.
   *
   *   / 10§->                                   / 10          §
   *  /                                         /              §
   *  |                                         |              §
   *  |     1+ln(x) dx : newIndex = outside ==> |   1+ln(x) dx §
   *  |                                         |              §
   *  /                                         /              §
   * / -10                                     / -10           §
   *
   * */
  int nextLayoutIndex;
  Tree* parent = rootRack()->parentOfDescendant(nextLayout, &nextLayoutIndex);
  const Tree* previousLayout = cursorRack();
  setCursorRack(Rack::From(parent));
  m_position = nextLayoutIndex + (direction.isRight());

  if (isSelecting() && cursorRack() != previousLayout) {
    /* If the cursor went into the parent, start the selection before
     * the layout that was just left (or after depending on the direction
     * of the selection). */
    m_startOfSelection = m_position + (direction.isRight() ? -1 : 1);
  }
  return true;
}

bool LayoutCursor::verticalMove(OMG::VerticalDirection direction) {
  Tree* previousLayout = cursorRack();
  bool moved = verticalMoveWithoutSelection(direction);

  // Handle selection (find a common ancestor to previous and current layout)
  if (moved && isSelecting() && previousLayout != cursorRack()) {
    Tree* layoutAncestor =
        rootRack()->commonAncestor(cursorRack(), previousLayout);
    assert(layoutAncestor);
    // Down goes left to right and up goes right to left
    moveCursorToLayout(layoutAncestor, direction.isUp()
                                           ? OMG::Direction::Left()
                                           : OMG::Direction::Right());
    m_startOfSelection = m_position + (direction.isUp() ? 1 : -1);
  }
  return moved;
}

static void ScoreCursorInDescendants(KDPoint p, Tree* rack, KDFont::Size font,
                                     LayoutBufferCursor* result) {
  KDCoordinate currentDistance =
      p.squareDistanceTo(result->middleLeftPoint(font));
  LayoutBufferCursor tempCursor(result->rootLayout(), rack);
  int n = rack->numberOfChildren();
  for (int i = 0; i <= n; i++) {
    /* In order to favor the ends in case of equality, we test the first, the
     * last and then the middle from left to right. */
    tempCursor.setPosition(i == 0 ? 0 : i == 1 ? n : i - 1);
    KDCoordinate distance =
        p.squareDistanceTo(tempCursor.middleLeftPoint(font));
    if (currentDistance > distance) {
      *result = tempCursor;
      currentDistance = distance;
    }
  }
  for (Tree* l : rack->children()) {
    for (Tree* r : l->children()) {
      ScoreCursorInDescendants(p, r, font, result);
    }
  }
}

static LayoutBufferCursor ClosestCursorInDescendantsOfRack(
    LayoutBufferCursor currentCursor, Tree* rack, KDFont::Size font) {
  LayoutBufferCursor result = LayoutBufferCursor(currentCursor.rootLayout(),
                                                 rack, OMG::Direction::Left());
  ScoreCursorInDescendants(currentCursor.middleLeftPoint(font), rack, font,
                           &result);
  return result;
}

bool LayoutCursor::verticalMoveWithoutSelection(
    OMG::VerticalDirection direction) {
  /* Step 1:
   * Try to enter right or left layout if it can be entered through up/down
   * */
  if (!isSelecting()) {
    Layout* nextLayout = rightLayout();
    PositionInLayout positionRelativeToNextLayout = PositionInLayout::Left;
    // Repeat for right and left
    for (int i = 0; i < 2; i++) {
      if (nextLayout) {
        int nextIndex = CursorMotion::IndexAfterVerticalCursorMove(
            nextLayout, direction, k_outsideIndex,
            positionRelativeToNextLayout);
        if (nextIndex != k_cantMoveIndex) {
          assert(nextIndex != k_outsideIndex);
          assert(!nextLayout->isRackLayout());
          setCursorRack(nextLayout->child(nextIndex));
          m_position = positionRelativeToNextLayout == PositionInLayout::Left
                           ? leftmostPosition()
                           : rightmostPosition();
          return true;
        }
      }
      nextLayout = leftLayout();
      positionRelativeToNextLayout = PositionInLayout::Right;
    }
  }

  /* Step 2:
   * Ask ancestor if cursor can move vertically. */
  int childIndex;
  Tree* parentLayout = this->parentLayout(&childIndex);
  PositionInLayout currentPosition =
      m_position == leftmostPosition()
          ? PositionInLayout::Left
          : (m_position == rightmostPosition() ? PositionInLayout::Right
                                               : PositionInLayout::Middle);
  while (parentLayout) {
    int nextIndex = CursorMotion::IndexAfterVerticalCursorMove(
        parentLayout, direction, childIndex, currentPosition);
    if (nextIndex != k_cantMoveIndex) {
      if (nextIndex == k_outsideIndex) {
        assert(currentPosition != PositionInLayout::Middle);
        moveCursorToLayout(parentLayout,
                           currentPosition == PositionInLayout::Left
                               ? OMG::Direction::Left()
                               : OMG::Direction::Right());
      } else {
        assert(!parentLayout->isRackLayout());
        // We assume the new cursor is the same whatever the font
        LayoutBufferCursor newCursor = ClosestCursorInDescendantsOfRack(
            *static_cast<LayoutBufferCursor*>(this),
            parentLayout->child(nextIndex), KDFont::Size::Large);
        setCursorRack(newCursor.cursorRack());
        m_position = newCursor.position();
      }
      return true;
    }
    Tree* rack = rootRack()->parentOfDescendant(parentLayout);
    parentLayout = rootRack()->parentOfDescendant(rack, &childIndex);
    currentPosition = PositionInLayout::Middle;
  }
  return false;
}

void LayoutBufferCursor::TreeStackCursor::privateDelete(
    DeletionMethod deletionMethod, bool deletionAppliedToParent) {
  assert(!deletionAppliedToParent ||
         m_cursorRackRef->block() != rootRack()->block());

  if (deletionMethod == DeletionMethod::MoveLeft) {
    bool dummy = false;
    move(OMG::Direction::Left(), false, &dummy);
    return;
  }
  TreeRef layout = m_cursorRackRef;
  TreeRef parent = rootRack()->parentOfDescendant(layout);

  if (deletionMethod == DeletionMethod::DeleteParent) {
    assert(deletionAppliedToParent);
    assert(parent && !parent->isRackLayout());
    Tree* parentRack = rootRack()->parentOfDescendant(parent, &m_position);
    Tree* detached = NAry::DetachChildAtIndex(parentRack, m_position);
    detached->moveTreeOverTree(layout);
    NAry::AddOrMergeChildAtIndex(parentRack, detached, m_position);
    m_cursorRackRef = parentRack;
    return;
  }
  if (deletionMethod == DeletionMethod::AutocompletedBracketPairMakeTemporary) {
    if (deletionAppliedToParent) {  // Inside bracket
      assert(parent->isAutocompletedPair());
      AutocompletedPair::SetTemporary(parent, Side::Left, true);
    } else {  // Right of bracket
      assert(leftLayout()->isAutocompletedPair());
      AutocompletedPair::SetTemporary(leftLayout(), Side::Right, true);
    }
    bool dummy = false;
    move(OMG::Direction::Left(), false, &dummy);
    balanceAutocompletedBracketsAndKeepAValidCursor();
    return;
  }
  if (deletionMethod == DeletionMethod::FractionDenominatorDeletion) {
    // Merge the numerator and denominator and replace the fraction with it
    assert(deletionAppliedToParent);
    Tree* fraction = parent;
    assert(fraction->isFractionLayout() && fraction->child(1) == layout);
    Tree* numerator = fraction->child(0);
    m_position = numerator->numberOfChildren();
    int indexOfFraction;
    Tree* parentOfFraction =
        rootRack()->parentOfDescendant(fraction, &indexOfFraction);
    m_position += indexOfFraction;
    Tree* detached =
        NAry::DetachChildAtIndex(parentOfFraction, indexOfFraction);
    // Remove Fraction Node
    detached->removeNode();
    // Merge denominator into numerator
    NAry::AddOrMergeChild(detached, layout);
    NAry::AddOrMergeChildAtIndex(parentOfFraction, detached, indexOfFraction);
    m_cursorRackRef = parentOfFraction;
    return;
  }
  if (deletionMethod == DeletionMethod::TwoRowsLayoutMoveFromLowertoUpper) {
    assert(deletionAppliedToParent);
    assert(parent->isBinomialLayout());
    int newIndex = Binomial::k_nIndex;
    m_cursorRackRef = parent->child(newIndex);
    m_position = rightmostPosition();
    return;
  }
  if (deletionMethod == DeletionMethod::GridLayoutMoveToUpperRow) {
    assert(deletionAppliedToParent);
    int currentIndex;
    Grid* grid = Grid::From(parentLayout(&currentIndex));
    int currentRow = grid->rowAtChildIndex(currentIndex);
    assert(currentRow > 0 && grid->numberOfColumns() >= 2);
    int newIndex =
        grid->indexAtRowColumn(currentRow - 1, grid->numberOfColumns() - 2);
    moveCursorToLayout(grid->child(newIndex),
                       OMG::HorizontalDirection::Right());
    // TODO trigger beautification here ?
    return;
  }
  if (deletionMethod == DeletionMethod::GridLayoutDeleteColumn ||
      deletionMethod == DeletionMethod::GridLayoutDeleteRow ||
      deletionMethod == DeletionMethod::GridLayoutDeleteColumnAndRow) {
    assert(deletionAppliedToParent);
    int currentIndex;
    Grid* grid = Grid::From(parentLayout(&currentIndex));
    int currentRow = grid->rowAtChildIndex(currentIndex);
    int currentColumn = grid->columnAtChildIndex(currentIndex);
    if (deletionMethod != DeletionMethod::GridLayoutDeleteColumn) {
      grid->deleteRowAtIndex(currentRow);
    }
    if (deletionMethod != DeletionMethod::GridLayoutDeleteRow) {
      grid->deleteColumnAtIndex(currentColumn);
    }
    int newChildIndex = grid->indexAtRowColumn(currentRow, currentColumn);
    moveCursorToLayout(grid->child(newChildIndex),
                       OMG::HorizontalDirection::Left());
    return;
  }
  assert(deletionMethod == DeletionMethod::DeleteLayout);
  if (deletionAppliedToParent) {
    moveCursorToLayout(rootRack()->parentOfDescendant(m_cursorRackRef),
                       OMG::Direction::Right());
  }
  assert(m_cursorRackRef->isRackLayout() &&
         (m_cursorRackRef == rootRack() ||
          !rootRack()->parentOfDescendant(m_cursorRackRef)->isRackLayout()));
  assert(m_position != 0);
  m_position--;
  NAry::RemoveChildAtIndex(m_cursorRackRef, m_position);
}

void LayoutCursor::removeEmptyRowOrColumnOfGridParentIfNeeded() {
  if (!RackLayout::IsEmpty(cursorRack())) {
    return;
  }
  int currentChildIndex;
  Tree* parent = parentLayout(&currentChildIndex);
  if (!parent || !parent->isGridLayout()) {
    return;
  }
  Grid* grid = Grid::From(parent);
  int newChildIndex =
      grid->removeTrailingEmptyRowOrColumnAtChildIndex(currentChildIndex);
  moveCursorToLayout(grid->child(newChildIndex),
                     OMG::HorizontalDirection::Left());
}

void LayoutCursor::collapseSiblingsOfLayout(Layout* l) {
  using namespace OMG;
  for (HorizontalDirection dir : {Direction::Right(), Direction::Left()}) {
    if (CursorMotion::ShouldCollapseSiblingsOnDirection(l, dir)) {
      collapseSiblingsOfLayoutOnDirection(
          l, dir, CursorMotion::CollapsingAbsorbingChildIndex(l, dir));
    }
  }
}

void LayoutCursor::collapseSiblingsOfLayoutOnDirection(
    Layout* l, OMG::HorizontalDirection direction, int absorbingChildIndex) {
  Rack* rack = cursorRack();
  assert(rack->indexOfChild(l) != -1);
  /* This method absorbs the siblings of a layout when it's inserted.
   *
   * Example:
   * When inserting √() was just inserted in "1 + √()45 + 3 ",
   * the square root should absorb the 45 and this will output
   * "1 + √(45) + 3"
   *
   * Here l = √(), and absorbingChildIndex = 0 (the inside of the sqrt)
   * */
  TreeRef absorbingRack = l->child(absorbingChildIndex);
  if (!RackLayout::IsEmpty(absorbingRack)) {
    return;
  }
  int indexInParent = rack->indexOfChild(l);
  int numberOfSiblings = rack->numberOfChildren();

  Layout* sibling;
  int step = direction.isRight() ? 1 : -1;
  /* Loop through the siblings and add them into l until an uncollapsable
   * layout is encountered. */
  while (true) {
    if (indexInParent == (direction.isRight() ? numberOfSiblings - 1 : 0)) {
      break;
    }
    int siblingIndex = indexInParent + step;
    sibling = rack->child(siblingIndex);
    if (!CursorMotion::IsCollapsable(sibling, rack, direction)) {
      break;
    }
    sibling = Layout::From(NAry::DetachChildAtIndex(rack, siblingIndex));
    int newIndex = direction.isRight() ? absorbingRack->numberOfChildren() : 0;
    assert(!sibling->isRackLayout());
    NAry::AddChildAtIndex(absorbingRack, sibling, newIndex);
    numberOfSiblings--;
    if (direction.isLeft()) {
      indexInParent--;
    }
  }
}

void LayoutBufferCursor::TreeStackCursor::
    balanceAutocompletedBracketsAndKeepAValidCursor() {
  /* Find the top horizontal layout for balancing brackets.
   *
   * This might go again through already balanced brackets but it's safer in
   * order to ensure that all brackets are always balanced after an insertion or
   * a deletion.
   *
   * Stop if the parent of the currentLayout is not horizontal neither
   * a bracket.
   * Ex: When balancing the brackets inside the numerator of a fraction, it's
   * useless to take the parent horizontal layout of the fraction, since
   * brackets outside of the fraction won't impact the ones inside the
   * fraction.
   */
  Tree* currentLayout = cursorRack();
  Tree* currentParent = currentLayout->parent(rootRack());
  while (currentParent && (currentParent->isRackLayout() ||
                           currentParent->isAutocompletedPair())) {
    currentLayout = currentParent;
    currentParent = currentLayout->parent(rootRack());
  }
  // If the top bracket does not have an horizontal parent, create one
  assert(currentLayout->isRackLayout());
  TreeRef ref = cursorRack();
  AutocompletedPair::BalanceBrackets(currentLayout, ref, &m_position);
  m_cursorRackRef = static_cast<Tree*>(ref);
}

void LayoutBufferCursor::applyTreeStackCursor(TreeStackCursor cursor) {
  m_position = cursor.m_position;
  m_startOfSelection = cursor.m_startOfSelection;
  /* We need a rack cast there since the pointed rack is set before the
   * actual content of the buffer is modified. */
  setCursorRack(static_cast<Rack*>(
      Tree::FromBlocks(rootRack()->block() + cursor.cursorRackOffset())));
}

void LayoutBufferCursor::execute(Action action, Poincare::Context* context,
                                 const void* data) {
  ExecutionContext executionContext{this, action, cursorRackOffset(), context};
  assert(SharedTreeStack->numberOfTrees() == 0);

  {
    LayoutBufferCursor* bufferCursor = executionContext.m_cursor;
    // Clone layoutBuffer into the TreeStack
    executionContext.m_cursor->rootRack()->cloneTree();
    // Create a temporary cursor
    TreeStackCursor editionCursor = bufferCursor->createTreeStackCursor();
    // Perform the action
    (editionCursor.*(executionContext.m_action))(executionContext.m_context,
                                                 data);
    // Apply the changes
    /* We need a rack cast there since the pointed rack is set before the
     * actual content of the buffer is modified. */
    bufferCursor->setCursorRack(static_cast<Rack*>(Tree::FromBlocks(
        bufferCursor->rootRack()->block() + editionCursor.cursorRackOffset())));
    bufferCursor->applyTreeStackCursor(editionCursor);

    assert(Tree::FromBlocks(SharedTreeStack->firstBlock())->isRackLayout());
    m_rootLayout = Poincare::Layout::Builder(
        Tree::FromBlocks(SharedTreeStack->firstBlock()));
  }
  SharedTreeStack->flush();
}

}  // namespace Poincare::Internal
