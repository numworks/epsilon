#include "layout_cursor.h"

#include <ion/unicode/utf8_decoder.h>
#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/layout/autocompleted_pair.h>
#include <poincare_junior/src/layout/grid.h>
#include <poincare_junior/src/layout/indices.h>
#include <poincare_junior/src/layout/k_tree.h>
#include <poincare_junior/src/layout/layout_cursor.h>
#include <poincare_junior/src/layout/rack_layout.h>
#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/n_ary.h>

#include <algorithm>

namespace PoincareJ {

#if 0
void LayoutCursor::safeSetLayout(Layout layout,
                                 OMG::HorizontalDirection sideOfLayout) {
  LayoutCursor previousCursor = *this;
  setLayout(layout, sideOfLayout);
  // Invalidate memoized info ?
}

void LayoutCursor::safeSetPosition(int position) {
  assert(position >= 0);
  assert(position <= RightmostPossibleCursorPosition(m_layout));
  assert(!isSelecting());
  LayoutCursor previousCursor = *this;
  m_position = position;
  // Invalidate memoized info ?
}
#endif

KDCoordinate LayoutCursor::cursorHeight(KDFont::Size font) const {
  if (cursorNode()->isGridLayout()) {
    return EmptyRectangle::RectangleSize(font).height();
  }
  LayoutSelection currentSelection = selection();
  int left, right;
  if (currentSelection.isEmpty()) {
    left = std::max(leftmostPosition(), m_position - 1);
    right = std::min(rightmostPosition(), m_position + 1);
  } else {
    left = currentSelection.leftPosition();
    right = currentSelection.rightPosition();
  }
  return RackLayout::SizeBetweenIndexes(cursorNode(), left, right).height();
}

KDPoint LayoutCursor::cursorAbsoluteOrigin(KDFont::Size font) const {
  if (cursorNode()->isGridLayout()) {
    Grid *grid = Grid::From(cursorNode());
    return Render::AbsoluteOrigin(cursorNode(), rootNode())
        .translatedBy(
            grid->positionOfChildAt(grid->columnAtChildIndex(m_position),
                                    grid->rowAtChildIndex(m_position), font))
        .translatedBy(
            KDPoint(EmptyRectangle::RectangleSize(font).width() / 2, 0));
  }
  KDCoordinate cursorBaseline = 0;
  LayoutSelection currentSelection = selection();
  int left, right;
  if (currentSelection.isEmpty()) {
    left = std::max(leftmostPosition(), m_position - 1);
    right = std::min(rightmostPosition(), m_position + 1);
  } else {
    left = currentSelection.leftPosition();
    right = currentSelection.rightPosition();
  }
  cursorBaseline =
      RackLayout::BaselineBetweenIndexes(cursorNode(), left, right);
  KDCoordinate cursorYOriginInLayout =
      Render::Baseline(cursorNode()) - cursorBaseline;
  KDCoordinate cursorXOffset = 0;
  cursorXOffset =
      RackLayout::SizeBetweenIndexes(cursorNode(), 0, m_position).width();
  return Render::AbsoluteOrigin(cursorNode(), rootNode())
      .translatedBy(KDPoint(cursorXOffset, cursorYOriginInLayout));
}

KDPoint LayoutCursor::middleLeftPoint(KDFont::Size font) const {
  KDPoint origin = cursorAbsoluteOrigin(font);
  return KDPoint(origin.x(), origin.y() + cursorHeight(font) / 2);
}

static const Tree *mostNestedGridParent(const Tree *node, const Tree *root) {
  while (node != root) {
    if (node->isGridLayout()) {
      return node;
    }
    node = node->parent(root);
  }
  return nullptr;
}

/* Move */
bool LayoutCursor::move(OMG::Direction direction, bool selecting,
                        bool *shouldRedrawLayout, Context *context) {
  *shouldRedrawLayout = false;
  if (!selecting && isSelecting()) {
    stopSelecting();
    *shouldRedrawLayout = true;
    return true;
  }
  if (selecting && !isSelecting()) {
    privateStartSelecting();
  }
#if 0
  LayoutCursor cloneCursor = *this;
#endif
  bool moved = false;
  bool wasEmpty =
      cursorNode()->isGridLayout() || RackLayout::IsEmpty(cursorNode());
  const Tree *oldGridParent = mostNestedGridParent(cursorNode(), rootNode());
  // Perform the actual move
  if (direction.isVertical()) {
    moved = verticalMove(direction);
  } else {
    moved = horizontalMove(direction);
  }
  assert(!*shouldRedrawLayout || moved);
  if (moved) {
    bool isEmpty =
        cursorNode()->isGridLayout() || RackLayout::IsEmpty(cursorNode());
    *shouldRedrawLayout =
        selecting || wasEmpty || isEmpty || *shouldRedrawLayout ||
        // Redraw to show/hide the empty gray squares of the parent grid
        mostNestedGridParent(cursorNode(), rootNode()) != oldGridParent;
#if 0
    if (cloneCursor.layout() != m_layout) {
      // Beautify the layout that was just left
      LayoutCursor rightmostPositionOfPreviousLayout(cloneCursor.layout(),
                                                     OMG::Direction::Right());
      *shouldRedrawLayout =
          InputBeautification::BeautifyLeftOfCursorBeforeCursorMove(
              &rightmostPositionOfPreviousLayout, context) ||
          *shouldRedrawLayout;
    }
#endif
  }

  if (isSelecting() && selection().isEmpty()) {
    stopSelecting();
  }

#if 0
  if (*shouldRedrawLayout) {
    invalidateSizesAndPositions();
  }
#endif
  return moved;
}

bool LayoutCursor::moveMultipleSteps(OMG::Direction direction, int step,
                                     bool selecting, bool *shouldRedrawLayout,
                                     Context *context) {
  assert(step > 0);
  for (int i = 0; i < step; i++) {
    if (!move(direction, selecting, shouldRedrawLayout, context)) {
      return i > 0;
    }
  }
  return true;
}

static bool IsTemporaryAutocompletedBracketPair(const Tree *l, Side tempSide) {
  return l->isAutocompletedPair() &&
         AutocompletedPair::IsTemporary(l, tempSide);
}

// Return leftParenthesisIndex
static int ReplaceCollapsableLayoutsLeftOfIndexWithParenthesis(
    EditionReference rack, int index) {
  int leftParenthesisIndex = index;
  // TODO : Use Iterator
  while (leftParenthesisIndex > 0 &&
         CursorMotion::IsCollapsable(rack->child(leftParenthesisIndex), rack,
                                     OMG::Direction::Left())) {
    leftParenthesisIndex--;
  }
  EditionReference parenthesis =
      SharedEditionPool->push(BlockType::ParenthesisLayout);
  SharedEditionPool->push(0);
  EditionReference tempRack = SharedEditionPool->push<BlockType::RackLayout>(0);
  int i = index;
  while (i >= leftParenthesisIndex) {
    EditionReference child = NAry::DetachChildAtIndex(rack, i);
    NAry::AddChildAtIndex(tempRack, child, 0);
    i--;
  }
  NAry::AddChildAtIndex(rack, parenthesis, leftParenthesisIndex);
  return leftParenthesisIndex;
}

/* const Tree* insertion */
void LayoutBufferCursor::EditionPoolCursor::insertLayout(Context *context,
                                                         const void *data) {
  const InsertLayoutContext *insertLayoutContext =
      static_cast<const InsertLayoutContext *>(data);
  bool forceRight = insertLayoutContext->m_forceRight;
  bool forceLeft = insertLayoutContext->m_forceLeft;

  const Tree *tree = insertLayoutContext->m_tree;
  Tree *copy = SharedEditionPool->contains(tree)
                   ? const_cast<Tree *>(insertLayoutContext->m_tree)
                   : tree->clone();
  // We need to keep track of the node which must live in the edition pool
  // TODO: do we need ConstReferences on const Nodes in the pool ?
  EditionReference ref(copy);
  if (!copy->isRackLayout()) {
    copy->cloneNodeAtNode(KRackL.node<1>);
  }

  if (Layout::IsEmpty(ref)) {
    return;
  }

  assert(!forceRight || !forceLeft);
  // - Step 1 - Delete selection
  deleteAndResetSelection(context, nullptr);

#if 0
  // - Step 2 - Beautify the current layout if needed.
  InputBeautification::BeautificationMethod beautificationMethod =
      InputBeautification::BeautificationMethodWhenInsertingLayout(layout);
  if (beautificationMethod.beautifyIdentifiersBeforeInserting) {
    InputBeautification::BeautifyLeftOfCursorBeforeCursorMove(this, context);
  }
#endif

  /* - Step 3 - Add empty row to grid layout if needed
   * When an empty child at the bottom or right of the grid is filled,
   * an empty row/column is added below/on the right.
   */
  if (cursorNode()->isGridLayout()) {
    setCursorNode(
        Grid::From(m_cursorReference)->willFillEmptyChildAtIndex(m_position));
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
  Tree *leftL = leftLayout();
  Tree *rightL = rightLayout();
  if (leftL && leftL->isAutocompletedPair()) {
    AutocompletedPair::MakeChildrenPermanent(
        leftL, Side::Right,
        !IsTemporaryAutocompletedBracketPair(ref->child(0), Side::Left));
  }
  if (rightL && rightL->isAutocompletedPair()) {
    AutocompletedPair::MakeChildrenPermanent(
        rightL, Side::Left,
        !IsTemporaryAutocompletedBracketPair(
            ref->child(ref->numberOfChildren() - 1), Side::Right));
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
              cursorNode(), cursorNode()->indexOfChild(leftL));
      m_position = leftParenthesisIndex + 1;
    }

    if (rightL && rightL->isVerticalOffsetLayout() &&
        VerticalOffset::IsSuffixSuperscript(rightL) &&
        cursorNode()->indexOfChild(rightL) > 0) {
      // Insert ^b right of a in a^c -> turn a^c into (a)^c
      int leftParenthesisIndex =
          ReplaceCollapsableLayoutsLeftOfIndexWithParenthesis(
              cursorNode(), cursorNode()->indexOfChild(rightL) - 1);
      setCursorNode(cursorNode()->child(leftParenthesisIndex)->child(0));
      m_position = cursorNode()->numberOfChildren();
    }
  }

  // - Step 6 - Find position to point to if layout will me merged
  EditionPoolCursor previousCursor = *this;
  EditionReference childToPoint;
  if (ref->numberOfChildren() != 1) {
    childToPoint = (forceRight || forceLeft)
                       ? nullptr
                       : CursorMotion::DeepChildToPointToWhenInserting(ref);
    // if (!childToPoint.isUninitialized() &&
    // childToPoint->isAutocompletedPair()) {
    // childToPoint = childToPoint->child(0);
    // }
  }

  // - Step 7 - Insert layout
  int numberOfInsertedChildren = ref->numberOfChildren();
  EditionReference toCollapse =
      numberOfInsertedChildren == 1 ? ref->child(0) : nullptr;
  /* AddOrMergeLayoutAtIndex will replace current layout with an
   * HorizontalLayout if needed. With this assert, m_position is guaranteed to
   * be preserved. */
  assert(cursorNode()->isRackLayout() &&
         (cursorNode() == rootNode() ||
          !rootNode()->parentOfDescendant(cursorNode())->isRackLayout()));
  NAry::AddOrMergeChildAtIndex(cursorNode(), ref, m_position);
  assert(cursorNode()->isRackLayout());

  if (!forceLeft) {
    // Move cursor right of inserted children
    m_position += numberOfInsertedChildren;
  }

  /* - Step 8 - Collapse siblings and find position to point to if layout was
   * not merged */
  if (numberOfInsertedChildren == 1) {
    // ref is undef
    collapseSiblingsOfLayout(toCollapse);
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
    setLayout(childToPoint, OMG::Direction::Left());
  }

  // - Step 10 - Balance brackets
  balanceAutocompletedBracketsAndKeepAValidCursor();
#if 0
  // - Step 11 - Beautify after insertion if needed
  if (beautificationMethod.beautifyAfterInserting) {
    InputBeautification::BeautifyLeftOfCursorAfterInsertion(this, context);
  }

  // - Step 12 - Invalidate layout sizes and positions
  invalidateSizesAndPositions();
#endif
}

void LayoutBufferCursor::addEmptyExponentialLayout(Context *context) {
  insertLayout(KRackL("e"_cl, KVertOffL(""_l)), context, false, false);
}

void LayoutBufferCursor::addEmptyTenPowerLayout(Context *context) {
  insertLayout(KRackL("1"_cl, "0"_cl, KVertOffL(""_l)), context, false, false);
}

void LayoutBufferCursor::addEmptyMatrixLayout(Context *context) {
  insertLayout(KEmptyMatrixL, context, false, false);
}

void LayoutBufferCursor::addEmptySquareRootLayout(Context *context) {
  insertLayout(KSqrtL(""_l), context, false, false);
}

void LayoutBufferCursor::addEmptyPowerLayout(Context *context) {
  insertLayout(KVertOffL(""_l), context, false, false);
}

void LayoutBufferCursor::addEmptySquarePowerLayout(Context *context) {
  /* Force the cursor right of the layout. */
  insertLayout(KVertOffL("2"_l), context, true, false);
}

void LayoutBufferCursor::addFractionLayoutAndCollapseSiblings(
    Context *context) {
  insertLayout(KFracL(""_l, ""_l), context, false, false);
}

void LayoutBufferCursor::EditionPoolCursor::insertText(Context *context,
                                                       const void *data) {
  const InsertTextContext *insertTextContext =
      static_cast<const InsertTextContext *>(data);
  const char *text = insertTextContext->m_text;
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
  EditionReference layoutToInsert = KRackL()->clone();
  EditionReference currentLayout = layoutToInsert;
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
        LayoutBufferCursor::EditionPoolCursor::InsertLayoutContext
            insertLayoutContext{layoutToInsert, forceCursorRightOfText,
                                forceCursorLeftOfText};
        insertLayout(context, &insertLayoutContext);
        layoutToInsert = KRackL()->clone();
        currentLayout = layoutToInsert;
        forceCursorLeftOfText = true;
        setCursorToFirstEmptyCodePoint = false;
      }
      assert(!codePoint.isCombining());
      continue;
    }

    /* TODO: The insertion of subscripts should be replaced with a parser
     * that creates layout. This is a draft of this. */

    /* - Step 1.1 - Handle subscripts
     * Subscripts are serialized as "\x14{...\x14}". When the code points
     * "\x14{" are encountered by the decoder, create a subscript layout
     * and continue insertion in it. When the code points "\x14}" are
     * encountered, leave the subscript and continue the insertion in its
     * parent. */
#if 0
    if (codePoint == UCodePointSystem) {
      // UCodePointSystem should be inserted only for system braces
      assert(nextCodePoint == '{' || nextCodePoint == '}');
      if (linearMode) {
        // Convert u\x14{n\x14} into u(n)
        codePoint = nextCodePoint == '{' ? '(' : ')';
        nextCodePoint = decoder.nextCodePoint();
      } else {
        if (nextCodePoint == '{') {
          // Enter a subscript
          Layout newChild = VerticalOffsetLayout::Builder(
              HorizontalLayout::Builder(),
              VerticalOffsetLayoutNode::VerticalPosition::Subscript);
          currentSubscriptDepth++;
          Layout horizontalChildOfSubscript = newChild.child(0);
          assert(horizontalChildOfSubscript.isEmpty());
          currentLayout =
              static_cast<HorizontalLayout &>(horizontalChildOfSubscript);
          codePoint = decoder.nextCodePoint();
          ;
          continue;
        }
        // UCodePointSystem should be inserted only for system braces
        assert(nextCodePoint == '}' && currentSubscriptDepth > 0);
        // Leave the subscript
        currentSubscriptDepth--;
        Layout subscript = currentLayout;
        while (subscript.type() != LayoutNode::Type::VerticalOffsetLayout) {
          subscript = subscript.parent();
          assert(!subscript.isUninitialized());
        }
        Layout parentOfSubscript = subscript.parent();
        assert(!parentOfSubscript.isUninitialized() &&
               parentOfSubscript.isHorizontal());
        currentLayout = static_cast<HorizontalLayout &>(parentOfSubscript);
        codePoint = decoder.nextCodePoint();
        continue;
      }
    }
#endif
    // - Step 1.2 - Handle code points and brackets
    Tree *newChild;
    TypeBlock bracketType(BlockType{});
    Side bracketSide;
    if (!linearMode && AutocompletedPair::IsAutoCompletedBracketPairCodePoint(
                           codePoint, &bracketType, &bracketSide)) {
      // Brackets will be balanced later in insertLayout
      newChild = AutocompletedPair::BuildFromBracketType(bracketType);
      AutocompletedPair::SetTemporary(newChild, OtherSide(bracketSide), true);
    } else if (nextCodePoint.isCombining()) {
      newChild = SharedEditionPool->push<BlockType::CombinedCodePointsLayout>(
          codePoint, nextCodePoint);
      nextCodePoint = decoder.nextCodePoint();
    } else {
      newChild = SharedEditionPool->push<BlockType::CodePointLayout>(codePoint);
    }

    NAry::AddOrMergeChildAtIndex(currentLayout, newChild,
                                 currentLayout->numberOfChildren());
    codePoint = nextCodePoint;
  }
  assert(currentSubscriptDepth == 0);

  // - Step 2 - Inserted the created layout
  LayoutBufferCursor::EditionPoolCursor::InsertLayoutContext
      insertLayoutContext{layoutToInsert, forceCursorRightOfText,
                          forceCursorLeftOfText};
  insertLayout(context, &insertLayoutContext);

  // TODO: Restore beautification
}

void LayoutBufferCursor::EditionPoolCursor::performBackspace(Context *context,
                                                             const void *data) {
  assert(data == nullptr);
  if (isSelecting()) {
    return deleteAndResetSelection(context, nullptr);
  }

  const Tree *leftL = leftLayout();
  if (leftL) {
    DeletionMethod deletionMethod =
        CursorMotion::DeletionMethodForCursorLeftOfChild(leftL, k_outsideIndex);
    privateDelete(deletionMethod, false);
  } else {
    int index;
    const Tree *p = parentLayout(&index);
    if (!p) {
      return;
    }
    DeletionMethod deletionMethod =
        CursorMotion::DeletionMethodForCursorLeftOfChild(p, index);
    privateDelete(deletionMethod, true);
  }
  removeEmptyRowOrColumnOfGridParentIfNeeded();
}

void LayoutBufferCursor::EditionPoolCursor::deleteAndResetSelection(
    Context *context, const void *data) {
  assert(data == nullptr);
  LayoutSelection selec = selection();
  if (selec.isEmpty()) {
    return;
  }
  int selectionLeftBound = selec.leftPosition();
  int selectionRightBound = selec.rightPosition();
  for (int i = selectionLeftBound; i < selectionRightBound; i++) {
    NAry::RemoveChildAtIndex(m_cursorReference, selectionLeftBound);
  }
  m_position = selectionLeftBound;
  stopSelecting();
  removeEmptyRowOrColumnOfGridParentIfNeeded();
}

bool LayoutCursor::isAtNumeratorOfEmptyFraction() const {
  if (cursorNode()->numberOfChildren() != 0) {
    return false;
  }
  int indexInParent;
  const Tree *parent =
      rootNode()->parentOfDescendant(cursorNode(), &indexInParent);
  return parent && parent->isFractionLayout() && indexInParent == 0 &&
         parent->child(1)->numberOfChildren() == 0;
}

#if 0
int LayoutCursor::RightmostPossibleCursorPosition(Layout l) {
  return l.isHorizontal() ? l.numberOfChildren() : 1;
}

void LayoutCursor::beautifyLeft(Context *context) {
  if (InputBeautification::BeautifyLeftOfCursorBeforeCursorMove(this,
                                                                context)) {
    invalidateSizesAndPositions();
  }
}
#endif

/* Private */

void LayoutCursor::setLayout(Tree *l, OMG::HorizontalDirection sideOfLayout) {
  if (!l->isRackLayout()) {
    int indexInParent;
    Tree *parent = rootNode()->parentOfDescendant(l, &indexInParent);
    setCursorNode(parent);
    m_position = indexInParent + (sideOfLayout.isRight());
    return;
  }
  setCursorNode(l);
  m_position = sideOfLayout.isLeft() ? leftmostPosition() : rightmostPosition();
}

void LayoutCursor::setGridPosition(Tree *node, int index,
                                   OMG::HorizontalDirection side) {
  Grid *grid = Grid::From(node);
  if (!grid->isFake(index)) {
    index -= grid->rowAtChildIndex(index);
    setLayout(grid->child(index), side);
  } else {
    setCursorNode(grid);
    m_position = index;
  }
}

Tree *LayoutCursor::leftLayout() const {
  assert(!isUninitialized());
  if (cursorNode()->isGridLayout()) {
    return nullptr;
  }
  return m_position == 0 ? nullptr : cursorNode()->child(m_position - 1);
}

Tree *LayoutCursor::rightLayout() const {
  assert(!isUninitialized());
  if (cursorNode()->isGridLayout()) {
    return nullptr;
  }
  if (m_position == cursorNode()->numberOfChildren()) {
    return nullptr;
  }
  return cursorNode()->child(m_position);
}

Tree *LayoutCursor::parentLayout(int *index) const {
  if (cursorNode()->isGridLayout()) {
    *index = m_position;
    return cursorNode();
  }
  Tree *parent = rootNode()->parentOfDescendant(cursorNode(), index);
  if (parent && parent->isGridLayout()) {
    *index += Grid::From(parent)->rowAtChildRealIndex(*index);
  }
  return parent;
}

void LayoutCursor::setCursorNode(Tree *node, int childIndex,
                                 OMG::HorizontalDirection side) {
  if (node->isGridLayout()) {
    // setCursorNode before calling isFake, it will be set again if not fake
    setCursorNode(node);
    if (Grid::From(node)->isFake(childIndex)) {
      m_position = childIndex;
      return;
    }
    childIndex -= Grid::From(node)->rowAtChildIndex(childIndex);
  }
  setLayout(node->child(childIndex), side);
}

bool LayoutCursor::horizontalMove(OMG::HorizontalDirection direction) {
  Tree *nextLayout = nullptr;
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
    setCursorNode(
        nextLayout, newIndex,
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
  Tree *parent = rootNode()->parentOfDescendant(nextLayout, &nextLayoutIndex);
  assert(!parent->isGridLayout());
  const Tree *previousLayout = cursorNode();
  setCursorNode(parent);
  m_position = nextLayoutIndex + (direction.isRight());

  if (isSelecting() && cursorNode() != previousLayout) {
    /* If the cursor went into the parent, start the selection before
     * the layout that was just left (or after depending on the direction
     * of the selection). */
    m_startOfSelection = m_position + (direction.isRight() ? -1 : 1);
  }
  return true;
}

bool LayoutCursor::verticalMove(OMG::VerticalDirection direction) {
  Tree *previousLayout = cursorNode();
  bool moved = verticalMoveWithoutSelection(direction);

  // Handle selection (find a common ancestor to previous and current layout)
  if (moved && isSelecting() && previousLayout != cursorNode()) {
    Tree *layoutAncestor =
        rootNode()->commonAncestor(cursorNode(), previousLayout);
    assert(layoutAncestor);
    // Down goes left to right and up goes right to left
    setLayout(layoutAncestor, direction.isUp() ? OMG::Direction::Left()
                                               : OMG::Direction::Right());
    m_startOfSelection = m_position + (direction.isUp() ? 1 : -1);
  }
  return moved;
}

static void ScoreCursorInDescendants(KDPoint p, Tree *rack, KDFont::Size font,
                                     LayoutBufferCursor *result) {
  KDCoordinate currentDistance =
      p.squareDistanceTo(result->middleLeftPoint(font));
  LayoutBufferCursor tempCursor(result->layoutBuffer(), rack);
  for (int i = 0; i <= rack->numberOfChildren(); i++) {
    tempCursor.setPosition(i);
    KDCoordinate distance =
        p.squareDistanceTo(tempCursor.middleLeftPoint(font));
    if (currentDistance > distance) {
      *result = tempCursor;
      currentDistance = distance;
    }
  }
  for (Tree *l : rack->children()) {
    for (Tree *r : l->children()) {
      ScoreCursorInDescendants(p, r, font, result);
    }
  }
}

static LayoutBufferCursor ClosestCursorInDescendantsOfRack(
    LayoutBufferCursor currentCursor, Tree *rack, KDFont::Size font) {
  LayoutBufferCursor result = LayoutBufferCursor(currentCursor.layoutBuffer(),
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
    Tree *nextLayout = rightLayout();
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
          setCursorNode(nextLayout->child(nextIndex));
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
  Tree *parentLayout = this->parentLayout(&childIndex);
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
        setLayout(parentLayout, currentPosition == PositionInLayout::Left
                                    ? OMG::Direction::Left()
                                    : OMG::Direction::Right());
      } else {
        assert(!parentLayout->isRackLayout());
        if (parentLayout->isGridLayout()) {
          if (Grid::From(parentLayout)->isFake(nextIndex)) {
            // No need to look for the closest place inside the empty
            setGridPosition(parentLayout, nextIndex,
                            OMG::HorizontalDirection::Left());
            return true;
          }
          nextIndex -= Grid::From(parentLayout)->rowAtChildIndex(nextIndex);
        }
        // We assume the new cursor is the same whatever the font
        LayoutBufferCursor newCursor = ClosestCursorInDescendantsOfRack(
            *static_cast<LayoutBufferCursor *>(this),
            parentLayout->child(nextIndex), KDFont::Size::Large);
        setCursorNode(newCursor.cursorNode());
        m_position = newCursor.position();
      }
      return true;
    }
    Tree *rack = rootNode()->parentOfDescendant(parentLayout);
    parentLayout = rootNode()->parentOfDescendant(rack, &childIndex);
    if (parentLayout && parentLayout->isGridLayout()) {
      childIndex += Grid::From(parentLayout)->rowAtChildRealIndex(childIndex);
    }
    currentPosition = PositionInLayout::Middle;
  }
  return false;
}

#if 0
bool LayoutCursor::setEmptyRectangleVisibilityAtCurrentPosition(
    EmptyRectangle::State state) {
  bool result = false;
  if (m_layout.isHorizontal()) {
    result =
        static_cast<HorizontalLayout &>(m_layout).setEmptyVisibility(state);
  }
  Layout leftL = leftLayout();
  if (!leftL.isUninitialized() &&
      leftL.type() == LayoutNode::Type::VerticalOffsetLayout &&
      static_cast<VerticalOffsetLayout &>(leftL).horizontalPosition() ==
          VerticalOffsetLayoutNode::HorizontalPosition::Prefix) {
    result =
        static_cast<VerticalOffsetLayout &>(leftL).setEmptyVisibility(state) ||
        result;
  }
  Layout rightL = rightLayout();
  if (!rightL.isUninitialized() &&
      rightL.type() == LayoutNode::Type::VerticalOffsetLayout &&
      static_cast<VerticalOffsetLayout &>(rightL).horizontalPosition() ==
          VerticalOffsetLayoutNode::HorizontalPosition::Suffix) {
    result =
        static_cast<VerticalOffsetLayout &>(rightL).setEmptyVisibility(state) ||
        result;
  }
  return result;
}
#endif

#if 0
void LayoutCursor::invalidateSizesAndPositions() {
  Layout layoutToInvalidate = m_layout;
  while (!layoutToInvalidate.parent().isUninitialized()) {
    layoutToInvalidate = layoutToInvalidate.parent();
  }
  layoutToInvalidate.invalidAllSizesPositionsAndBaselines();
}
#else
// TODO: Nothing is memoized for now, maybe implement something ?
#endif

void LayoutBufferCursor::EditionPoolCursor::privateDelete(
    DeletionMethod deletionMethod, bool deletionAppliedToParent) {
  assert(!deletionAppliedToParent ||
         m_cursorReference->block() != rootNode()->block());

  if (deletionMethod == DeletionMethod::MoveLeft) {
    bool dummy = false;
    move(OMG::Direction::Left(), false, &dummy);
    return;
  }
  EditionReference m_layout = m_cursorReference;
  EditionReference parent = rootNode()->parentOfDescendant(m_layout);

  if (deletionMethod == DeletionMethod::DeleteParent) {
    assert(deletionAppliedToParent);
    assert(parent && !parent->isRackLayout());
    Tree *parentRack = rootNode()->parentOfDescendant(parent, &m_position);
    Tree *detached = NAry::DetachChildAtIndex(parentRack, m_position);
    detached->moveTreeOverTree(m_layout);
    NAry::AddOrMergeChildAtIndex(parentRack, detached, m_position);
    m_cursorReference = parentRack;
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
    Tree *fraction = parent;
    assert(fraction->isFractionLayout() && fraction->child(1) == m_layout);
    Tree *numerator = fraction->child(0);
    m_position = numerator->numberOfChildren();
    int indexOfFraction;
    Tree *parentOfFraction =
        rootNode()->parentOfDescendant(fraction, &indexOfFraction);
    m_position += indexOfFraction;
    Tree *detached =
        NAry::DetachChildAtIndex(parentOfFraction, indexOfFraction);
    // Remove Fraction Node
    detached->removeNode();
    // Merge denominator into numerato
    NAry::AddOrMergeChild(detached, m_layout);
    NAry::AddOrMergeChildAtIndex(parentOfFraction, detached, indexOfFraction);
    m_cursorReference = parentOfFraction;
    return;
  }
  if (deletionMethod == DeletionMethod::BinomialCoefficientMoveFromKtoN) {
    // It is useful at all ?
    assert(deletionAppliedToParent);
    assert(parent->isBinomialLayout());
    int newIndex = Binomial::nIndex;
    m_cursorReference = parent->child(newIndex);
    m_position = rightmostPosition();
    return;
  }
  if (deletionMethod == DeletionMethod::GridLayoutMoveToUpperRow) {
    assert(deletionAppliedToParent);
    int currentIndex;
    Grid *grid = Grid::From(parentLayout(&currentIndex));
    int currentRow = grid->rowAtChildIndex(currentIndex);
    assert(currentRow > 0 && grid->numberOfColumns() >= 2);
    int newIndex =
        grid->indexAtRowColumn(currentRow - 1, grid->numberOfRealColumns() - 1);
    setGridPosition(grid, newIndex, OMG::HorizontalDirection::Right());
    return;
  }
  if (deletionMethod == DeletionMethod::GridLayoutDeleteColumn ||
      deletionMethod == DeletionMethod::GridLayoutDeleteRow ||
      deletionMethod == DeletionMethod::GridLayoutDeleteColumnAndRow) {
    assert(deletionAppliedToParent);
    int currentIndex;
    Grid *grid = Grid::From(parentLayout(&currentIndex));
    int currentRow = grid->rowAtChildIndex(currentIndex);
    int currentColumn = grid->columnAtChildIndex(currentIndex);
    if (deletionMethod != DeletionMethod::GridLayoutDeleteColumn) {
      grid->deleteRowAtIndex(currentRow);
    }
    if (deletionMethod != DeletionMethod::GridLayoutDeleteRow) {
      grid->deleteColumnAtIndex(currentColumn);
    }
    int newChildIndex = grid->indexAtRowColumn(currentRow, currentColumn);
    setGridPosition(grid, newChildIndex, OMG::HorizontalDirection::Left());
    return;
  }
  assert(deletionMethod == DeletionMethod::DeleteLayout);
  if (deletionAppliedToParent) {
    setLayout(rootNode()->parentOfDescendant(m_cursorReference),
              OMG::Direction::Right());
  }
  assert(m_cursorReference->isRackLayout() &&
         (m_cursorReference == rootNode() ||
          !rootNode()->parentOfDescendant(m_cursorReference)->isRackLayout()));
  assert(m_position != 0);
  m_position--;
  NAry::RemoveChildAtIndex(m_cursorReference, m_position);
}

void LayoutCursor::removeEmptyRowOrColumnOfGridParentIfNeeded() {
  if (!cursorNode()->isRackLayout() || !RackLayout::IsEmpty(cursorNode())) {
    return;
  }
  int currentChildIndex;
  Tree *parent = parentLayout(&currentChildIndex);
  if (!parent || !parent->isGridLayout()) {
    return;
  }
  Grid *grid = Grid::From(parent);
  int newChildIndex =
      grid->removeTrailingEmptyRowOrColumnAtChildIndex(currentChildIndex);
  setGridPosition(grid, newChildIndex, OMG::HorizontalDirection::Left());
}

void LayoutCursor::collapseSiblingsOfLayout(Tree *l) {
  using namespace OMG;
  for (HorizontalDirection dir : {Direction::Right(), Direction::Left()}) {
    if (CursorMotion::ShouldCollapseSiblingsOnDirection(l, dir)) {
      collapseSiblingsOfLayoutOnDirection(
          l, dir, CursorMotion::CollapsingAbsorbingChildIndex(l, dir));
    }
  }
}

void LayoutCursor::collapseSiblingsOfLayoutOnDirection(
    Tree *l, OMG::HorizontalDirection direction, int absorbingChildIndex) {
  Tree *rack = cursorNode();
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
  EditionReference absorbingRack = l->child(absorbingChildIndex);
  if (!RackLayout::IsEmpty(absorbingRack)) {
    return;
  }
  int indexInParent = rack->indexOfChild(l);
  int numberOfSiblings = rack->numberOfChildren();

  Tree *sibling;
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
    sibling = NAry::DetachChildAtIndex(rack, siblingIndex);
    int newIndex = direction.isRight() ? absorbingRack->numberOfChildren() : 0;
    assert(!sibling->isRackLayout());
    NAry::AddChildAtIndex(absorbingRack, sibling, newIndex);
    numberOfSiblings--;
    if (direction.isLeft()) {
      indexInParent--;
    }
  }
}

void LayoutBufferCursor::EditionPoolCursor::
    balanceAutocompletedBracketsAndKeepAValidCursor() {
  /* Find the top horizontal layout for balancing brackets.
   *
   * This might go again through already balanced brackets but it's safer
   * in order to ensure that all brackets are always balanced after an
   * insertion or a deletion.
   *
   * Stop if the parent of the currentLayout is not horizontal neither
   * a bracket.
   * Ex: When balancing the brackets inside the numerator of a fraction,
   * it's useless to take the parent horizontal layout of the fraction, since
   * brackets outside of the fraction won't impact the ones inside the
   * fraction
   * */
  Tree *currentLayout = cursorNode();
  Tree *currentParent = currentLayout->parent(rootNode());
  while (currentParent && (currentParent->isRackLayout() ||
                           currentParent->isAutocompletedPair())) {
    currentLayout = currentParent;
    currentParent = currentLayout->parent(rootNode());
  }
  // If the top bracket does not have an horizontal parent, create one
  assert(currentLayout->isRackLayout());
  EditionReference ref = cursorNode();
  AutocompletedPair::BalanceBrackets(currentLayout, ref, &m_position);
  m_cursorReference = static_cast<Tree *>(ref);
}

void LayoutBufferCursor::applyEditionPoolCursor(EditionPoolCursor cursor) {
  m_position = cursor.m_position;
  m_startOfSelection = cursor.m_startOfSelection;
  setCursorNode(
      Tree::FromBlocks(rootNode()->block() + cursor.cursorNodeOffset()));
}

void LayoutBufferCursor::execute(Action action, Context *context,
                                 const void *data) {
  ExecutionContext executionContext{this, action, cursorNodeOffset(), context};
  // Perform Action within an execution
  SharedEditionPool->executeAndDump(
      [](void *context, const void *data) {
        ExecutionContext *executionContext =
            static_cast<ExecutionContext *>(context);
        LayoutBufferCursor *bufferCursor = executionContext->m_cursor;
        // Clone layoutBuffer into the EditionPool
        SharedEditionPool->clone(executionContext->m_cursor->rootNode());
        // Create a temporary cursor
        EditionPoolCursor editionCursor =
            bufferCursor->createEditionPoolCursor();
        // Perform the action
        (editionCursor.*(executionContext->m_action))(
            executionContext->m_context, data);
        // Apply the changes
        bufferCursor->setCursorNode(
            Tree::FromBlocks(bufferCursor->rootNode()->block() +
                             editionCursor.cursorNodeOffset()));
        bufferCursor->applyEditionPoolCursor(editionCursor);
        /* The resulting EditionPool tree will be loaded back into
         * m_layoutBuffer and EditionPool will be flushed. */
      },
      &executionContext, data, m_layoutBuffer, k_layoutBufferSize,
      [](void *context) {
        // Default implementation illustrating how the context could be
        // relaxed ExecutionContext * executionContext =
        // static_cast<ExecutionContext
        // *>(context); Context * context = executionContext->m_context;
        return false;
      });
}

}  // namespace PoincareJ
