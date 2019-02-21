#include <poincare/layout.h>
#include <poincare/bracket_pair_layout.h>
#include <poincare/char_layout.h>
#include <poincare/empty_layout.h>
#include <poincare/expression.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout_cursor.h>

namespace Poincare {
Layout Layout::clone() const {
  if (isUninitialized()) {
    return Layout();
  }
  TreeHandle c = TreeHandle::clone();
  Layout cast = Layout(static_cast<LayoutNode *>(c.node()));
  cast.invalidAllSizesPositionsAndBaselines();
  return cast;
}

int Layout::serializeParsedExpression(char * buffer, int bufferSize) const {
  /* This method fixes the following problem:
   * Some layouts have a special serialization so they can be parsed afterwards,
   * such has logBase3(2) that serializes as log_{3}(2). When handling the
   * layout text, we want to use log(2,3) because we might paste the text in a
   * LinearEdition textfield, so what we really want is the parsed expression's
   * serialization. */
  if (bufferSize <= 0) {
    return 0;
  }
  serializeForParsing(buffer, bufferSize);
  Poincare::Expression e = Poincare::Expression::Parse(buffer);
  if (e.isUninitialized()) {
    buffer[0] = 0;
    return 0;
  }
  return e.serialize(buffer, bufferSize, Poincare::Preferences::sharedPreferences()->displayMode());
}

Layout Layout::recursivelyMatches(LayoutTest test) const {
  if (test(*this)) {
    return *this;
  }
  for (int i = 0; i < numberOfChildren(); i++) {
    Layout childResult = childAtIndex(i).recursivelyMatches(test);
    if (!childResult.isUninitialized()) {
      return childResult;
    }
  }
  return Layout();
}

// Cursor
LayoutCursor Layout::cursor() const {
  assert(!isUninitialized());
  return LayoutCursor(const_cast<Layout *>(this)->node());
}

LayoutCursor Layout::equivalentCursor(LayoutCursor * cursor) {
  assert(!isUninitialized());
  return node()->equivalentCursor(cursor);
}

Layout Layout::childAtIndex(int i) const {
  TreeHandle c = TreeHandle::childAtIndex(i);
  return static_cast<Layout &>(c);
}

// Tree modification

void Layout::replaceChild(Layout oldChild, Layout newChild, LayoutCursor * cursor, bool force) {
  int childIndex = indexOfChild(oldChild);
  assert(childIndex >= 0);
  if (!node()->willReplaceChild(oldChild.node(), newChild.node(), cursor, force)) {
    return;
  }
  replaceChildInPlace(oldChild, newChild);
  if (cursor != nullptr) {
    cursor->setLayout(newChild);
    cursor->setPosition(LayoutCursor::Position::Right);
  }
  node()->didReplaceChildAtIndex(childIndex, cursor, force);
}

void Layout::replaceChildWithEmpty(Layout oldChild, LayoutCursor * cursor) {
  replaceChild(oldChild, EmptyLayout::Builder(), cursor);
}

void Layout::replaceWith(Layout newChild, LayoutCursor * cursor) {
  Layout p = parent();
  assert(!p.isUninitialized());
  p.replaceChild(*this, newChild, cursor);
}

void Layout::replaceWithJuxtapositionOf(Layout leftChild, Layout rightChild, LayoutCursor * cursor, bool putCursorInTheMiddle) {
  Layout p = parent();
  assert(!p.isUninitialized());
  if (!p.isHorizontal()) {
    /* One of the children to juxtapose might be "this", so we cannot just call
     * replaceWith. */
    HorizontalLayout horizontalLayoutR = HorizontalLayout::Builder();
    p.replaceChild(*this, horizontalLayoutR, cursor);
    horizontalLayoutR.addOrMergeChildAtIndex(leftChild, 0, false);
    if (putCursorInTheMiddle) {
      if (!horizontalLayoutR.isEmpty()) {
        cursor->setLayout(horizontalLayoutR.childAtIndex(horizontalLayoutR.numberOfChildren()-1));
        cursor->setPosition(LayoutCursor::Position::Right);
      } else {
        cursor->setLayout(horizontalLayoutR);
        cursor->setPosition(LayoutCursor::Position::Left);
      }
    }
    horizontalLayoutR.addOrMergeChildAtIndex(rightChild, 1, false);
    return;
  }
  /* The parent is an Horizontal layout, so directly add the two juxtaposition
   * children to the parent. */
  int idxInParent = p.indexOfChild(*this);
  HorizontalLayout castedParent = HorizontalLayout(static_cast<HorizontalLayoutNode *>(p.node()));
  if (putCursorInTheMiddle) {
    if (idxInParent > 0) {
      cursor->setLayout(castedParent.childAtIndex(idxInParent-1));
      cursor->setPosition(LayoutCursor::Position::Right);
    } else {
      cursor->setLayout(castedParent);
      cursor->setPosition(LayoutCursor::Position::Left);
    }
  }
  castedParent.addOrMergeChildAtIndex(rightChild, idxInParent, true);
  castedParent.addOrMergeChildAtIndex(leftChild, idxInParent, true, putCursorInTheMiddle ? cursor : nullptr);
  p.removeChild(*this, cursor->layoutReference() == *this ? cursor : nullptr);
}

void Layout::addChildAtIndex(Layout l, int index, int currentNumberOfChildren, LayoutCursor * cursor) {
  int newIndex = index;
  int newCurrentNumberOfChildren = currentNumberOfChildren;
  if (!node()->willAddChildAtIndex(l.node(), &newIndex, &newCurrentNumberOfChildren, cursor)) {
    return;
  }
  Layout nextPointedLayout;
  LayoutCursor::Position nextPosition = LayoutCursor::Position::Left;
  if (cursor != nullptr) {
    if (newIndex < this->numberOfChildren()) {
      nextPointedLayout = childAtIndex(newIndex);
      nextPosition = LayoutCursor::Position::Left;
    } else {
      nextPointedLayout = *this;
      nextPosition = LayoutCursor::Position::Right;
    }
  }

  addChildAtIndexInPlace(l, newIndex, newCurrentNumberOfChildren);

  if (cursor != nullptr) {
    cursor->setLayout(nextPointedLayout);
    cursor->setPosition(nextPosition);
  }
}

void Layout::addSibling(LayoutCursor * cursor, Layout sibling, bool moveCursor) {
  if (!node()->willAddSibling(cursor, sibling.node(), moveCursor)) {
    return;
  }
  /* The layout must have a parent, because HorizontalLayout's
   * preprocessAddSibling returns false only an HorizontalLayout can be the
   * root layout. */
  Layout rootLayout = root();
  Layout p = parent();
  assert(!p.isUninitialized());
  if (p.isHorizontal()) {
    int indexInParent = p.indexOfChild(*this);
    int siblingIndex = cursor->position() == LayoutCursor::Position::Left ? indexInParent : indexInParent + 1;

    /* Special case: If the neighbour sibling is a VerticalOffsetLayout, let it
     * handle the insertion of the new sibling. Do not enter the special case if
     * "this" is a VerticalOffsetLayout, to avoid an infinite loop. */
    if (!isVerticalOffset()) {
      Layout neighbour;
      if (cursor->position() == LayoutCursor::Position::Left && indexInParent > 0) {
        neighbour = p.childAtIndex(indexInParent - 1);
      } else if (cursor->position() == LayoutCursor::Position::Right && indexInParent < p.numberOfChildren() - 1) {
        neighbour = p.childAtIndex(indexInParent + 1);
      }
      if (!neighbour.isUninitialized() && neighbour.isVerticalOffset()) {
        if (moveCursor) {
          cursor->setLayout(neighbour);
          cursor->setPosition(cursor->position() == LayoutCursor::Position::Left ? LayoutCursor::Position::Right : LayoutCursor::Position::Left);
        }
        neighbour.addSibling(cursor, sibling, moveCursor);
        return;
      }
    }

    // Else, let the parent add the sibling.
    HorizontalLayout(static_cast<HorizontalLayoutNode *>(p.node())).addOrMergeChildAtIndex(sibling, siblingIndex, true, moveCursor ? cursor : nullptr);
    return;
  }
  if (cursor->position() == LayoutCursor::Position::Left) {
    replaceWithJuxtapositionOf(sibling, *this, cursor);
  } else {
    assert(cursor->position() == LayoutCursor::Position::Right);
    replaceWithJuxtapositionOf(*this, sibling, cursor);
  }
}

void Layout::removeChild(Layout l, LayoutCursor * cursor, bool force) {
  if (!node()->willRemoveChild(l.node(), cursor, force)) {
    return;
  }
  assert(hasChild(l));
  int index = indexOfChild(l);
  removeChildInPlace(l, l.numberOfChildren());
  if (cursor) {
    if (index < numberOfChildren()) {
      Layout newCursorRef = childAtIndex(index);
      cursor->setLayout(newCursorRef);
      cursor->setPosition(LayoutCursor::Position::Left);
    } else {
      int newPointedLayoutIndex = index - 1;
      if (newPointedLayoutIndex >= 0 && newPointedLayoutIndex < numberOfChildren()) {
        cursor->setLayout(childAtIndex(newPointedLayoutIndex));
        cursor->setPosition(LayoutCursor::Position::Right);
      } else {
        cursor->setLayout(*this);
        cursor->setPosition(LayoutCursor::Position::Right);
      }
    }
  }
  node()->didRemoveChildAtIndex(index, cursor, force);
}

void  Layout::removeChildAtIndex(int index, LayoutCursor * cursor, bool force) {
  removeChild(childAtIndex(index), cursor, force);
}

void Layout::collapseOnDirection(HorizontalDirection direction, int absorbingChildIndex) {
  Layout p = parent();
  if (p.isUninitialized() || !p.isHorizontal()) {
    return;
  }
  int idxInParent = p.indexOfChild(*this);
  int numberOfSiblings = p.numberOfChildren();
  int numberOfOpenParenthesis = 0;
  bool canCollapse = true;
  Layout absorbingChild = childAtIndex(absorbingChildIndex);
  if (absorbingChild.isUninitialized() || !absorbingChild.isHorizontal()) {
    return;
  }
  HorizontalLayout horizontalAbsorbingChild = HorizontalLayout(static_cast<HorizontalLayoutNode *>(absorbingChild.node()));
  if (direction == HorizontalDirection::Right && idxInParent < numberOfSiblings - 1) {
    canCollapse = !(p.childAtIndex(idxInParent+1).mustHaveLeftSibling());
  }
  Layout sibling;
  bool forceCollapse = false;
  while (canCollapse) {
    if (direction == HorizontalDirection::Right && idxInParent == numberOfSiblings - 1) {
      break;
    }
    if (direction == HorizontalDirection::Left && idxInParent == 0) {
      break;
    }
    int siblingIndex = direction == HorizontalDirection::Right ? idxInParent+1 : idxInParent-1;
    sibling = p.childAtIndex(siblingIndex);
    /* Even if forceCollapse is true, isCollapsable should be called to update
     * the number of open parentheses. */
    bool shouldCollapse = sibling.isCollapsable(&numberOfOpenParenthesis, direction == HorizontalDirection::Left);
    if (shouldCollapse || forceCollapse) {
      /* If the collapse direction is Left and the next sibling to be collapsed
       * must have a left sibling, force the collapsing of this needed left
       * sibling. */
      forceCollapse = direction == HorizontalDirection::Left && sibling.mustHaveLeftSibling();
      p.removeChildAtIndex(siblingIndex, nullptr);
      int newIndex = direction == HorizontalDirection::Right ? absorbingChild.numberOfChildren() : 0;
      horizontalAbsorbingChild.addOrMergeChildAtIndex(sibling, newIndex, true);
      numberOfSiblings--;
      if (direction == HorizontalDirection::Left) {
        idxInParent--;
      }
    } else {
      break;
    }
  }
}

void Layout::collapseSiblings(LayoutCursor * cursor) {
  Layout rootLayout = root();
  if (node()->shouldCollapseSiblingsOnRight()) {
    Layout absorbingChild = childAtIndex(rightCollapsingAbsorbingChildIndex());
    if (!absorbingChild.isHorizontal()) {
      Layout horRef = HorizontalLayout::Builder();
      replaceChild(absorbingChild, horRef, cursor, true);
      horRef.addChildAtIndexInPlace(absorbingChild, 0, 0);
    }
    collapseOnDirection(HorizontalDirection::Right, rightCollapsingAbsorbingChildIndex());
  }
  if (node()->shouldCollapseSiblingsOnLeft()) {
    Layout absorbingChild = childAtIndex(leftCollapsingAbsorbingChildIndex());
    if (!absorbingChild.isHorizontal()) {
      Layout horRef = HorizontalLayout::Builder();
      replaceChild(absorbingChild, horRef, cursor, true);
      horRef.addChildAtIndexInPlace(absorbingChild, 0, 0);
    }
    collapseOnDirection(HorizontalDirection::Left, leftCollapsingAbsorbingChildIndex());
  }
  node()->didCollapseSiblings(cursor);
}

}
