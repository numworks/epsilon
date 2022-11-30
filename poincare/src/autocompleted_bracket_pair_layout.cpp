#include <poincare/autocompleted_bracket_pair_layout.h>
#include <poincare/empty_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout.h>

namespace Poincare {

bool AutocompletedBracketPairLayoutNode::willAddSibling(LayoutCursor * cursor, Layout * sibling, bool moveCursor) {
  /* Make a temporary bracket permanent if something would be inserted beyond
   * its bounds.
   * ([ and ] denote temporary parentheses)
   * e.g. (1+2]| -> "+" -> (1+2)+| */
  Side insertionSide;
  if (cursor->isEquivalentTo(LayoutCursor(this, LayoutCursor::Position::Right))) {
    insertionSide = Side::Right;
  } else if (cursor->isEquivalentTo(LayoutCursor(this, LayoutCursor::Position::Left))) {
    insertionSide = Side::Left;
  } else {
    return BracketPairLayoutNode::willAddSibling(cursor, sibling, moveCursor);
  }
  if (!isTemporary(insertionSide)) {
    return true;
  }
  makePermanent(insertionSide);
  /* If the inserted layout is a bracket of the same type as the temporary
   * bracket, do not inserted it, as though the user had closed it manually.
   * e.g. (1+2]| -> ")" -> (1+2)| instead of [(1+2))|*/
  AutocompletedBracketPairLayoutNode * bracketSibling = sibling->type() == type() ? static_cast<AutocompletedBracketPairLayoutNode *>(sibling->node()) : nullptr;
  bool ignoreSibling = bracketSibling && bracketSibling->isTemporary(OtherSide(insertionSide));
  if (ignoreSibling) {
    if (insertionSide == Side::Left) {
      cursor->setPosition(LayoutCursor::Position::Left);
      cursor->setLayout(Layout(childLayout()));
    }
    *sibling = Layout(this);
    return false;
  }
  return true;
}

void AutocompletedBracketPairLayoutNode::deleteBeforeCursor(LayoutCursor * cursor) {
  Side deletionSide;
  if (cursor->isEquivalentTo(LayoutCursor(this, LayoutCursor::Position::Right))) {
    deletionSide = Side::Right;
  } else if (cursor->isEquivalentTo(LayoutCursor(childLayout(), LayoutCursor::Position::Left))) {
    deletionSide = Side::Left;
  } else {
    return BracketPairLayoutNode::deleteBeforeCursor(cursor);
  }
  if (childLayout()->isEmpty() && isTemporary(OtherSide(deletionSide))) {
    /* Use LayoutNode::deleteBeforeCursor instead of BracketPairLayoutNode::,
     * as that one will attempt to enter the bracket from the right. */
    cursor->setLayout(Layout(this));
    cursor->setPosition(LayoutCursor::Position::Right);
    return LayoutNode::deleteBeforeCursor(cursor);
  }

  LayoutCursor nextCursor = cursorAfterDeletion(deletionSide);
  makeTemporary(deletionSide, cursor);
  assert(!nextCursor.layout().parent().isUninitialized());
  cursor->setTo(&nextCursor);
}

void AutocompletedBracketPairLayoutNode::setTemporary(Side side, bool temporary) {
  if (side == Side::Left) {
    m_leftIsTemporary = temporary;
  } else {
    assert(side == Side::Right);
    m_rightIsTemporary = temporary;
  }
}

void AutocompletedBracketPairLayoutNode::makePermanent(Side side) {
  /* Recursively make all bracket children permanent on that side.
   * e.g. (((1]]|] -> "+" -> (((1))+|] */
  if (!isTemporary(side)) {
    return;
  }
  Layout child = childOnSide(side);
  if (type() == child.type()) {
    AutocompletedBracketPairLayoutNode * bracket = static_cast<AutocompletedBracketPairLayoutNode *>(child.node());
    bracket->makePermanent(side);
  }
  setTemporary(side, false);
}

Layout AutocompletedBracketPairLayoutNode::balanceAfterInsertion(Side insertedSide, LayoutCursor * cursor) {
  assert(cursor);
  Layout thisRef(this);
  // Try to move the inserted temporary bracket up into parents
  setTemporary(OtherSide(insertedSide), false);
  makeTemporary(OtherSide(insertedSide), cursor);
  if (insertedSide == Side::Left) {
    cursor->setPosition(LayoutCursor::Position::Left);
    cursor->setLayout(thisRef.childAtIndex(0));
  } else {
    cursor->setPosition(LayoutCursor::Position::Right);
    cursor->setLayout(thisRef);
  }
  return thisRef;
}

AutocompletedBracketPairLayoutNode * AutocompletedBracketPairLayoutNode::autocompletedParent() const {
  LayoutNode * p = parent();
  while (p) {
    if (type() == p->type()) {
      return static_cast<AutocompletedBracketPairLayoutNode *>(p);
    }
    if (p->type() != LayoutNode::Type::HorizontalLayout) {
      break;
    }
    p = p->parent();
  }
  return nullptr;
}

bool AutocompletedBracketPairLayoutNode::makeTemporary(Side side, LayoutCursor * cursor) {
  if (isTemporary(side)) {
    return false;
  }
  Layout thisRef(this);
  absorbSiblings(side, cursor);
  /* 'this' may be invalid after the call to absorbSiblings. */
  AutocompletedBracketPairLayoutNode * newThis = static_cast<AutocompletedBracketPairLayoutNode *>(thisRef.node());
  AutocompletedBracketPairLayoutNode * p = newThis->autocompletedParent();
  if (!(p && p->makeTemporary(side, cursor))) {
    /* 'this' was the topmost pair without a temporary bracket on this side. */
    newThis->setTemporary(side, true);
    if (newThis->isTemporary(Side::Left) && newThis->isTemporary(Side::Right)) {
      assert(newThis->parent());
      thisRef.parent().replaceChild(thisRef, thisRef.childAtIndex(0), cursor);
    }
  }
  return true;
}

void AutocompletedBracketPairLayoutNode::absorbSiblings(Side side, LayoutCursor * cursor) {
  Layout thisRef = Layout(this);
  Layout p = parent();
  assert(!p.isUninitialized());
  if (p.type() != LayoutNode::Type::HorizontalLayout) {
    return;
  }
  HorizontalLayout h = static_cast<HorizontalLayout &>(p);
  int thisIndex = h.indexOfChild(thisRef);

  if (childLayout()->type() != Type::HorizontalLayout) {
    HorizontalLayout newChild = HorizontalLayout::Builder();
    Layout oldChild = Layout(childLayout());
    thisRef.replaceChild(oldChild, newChild, cursor);
    newChild.addOrMergeChildAtIndex(oldChild, 0, false, cursor);
  }
  assert(childLayout()->type() == Type::HorizontalLayout);
  HorizontalLayout child = HorizontalLayout(static_cast<HorizontalLayoutNode *>(childLayout()));

  int injectionIndex, removalIndex, removalEnd;
  if (side == Side::Left) {
    injectionIndex = 0;
    removalIndex = thisIndex - 1;
    removalEnd = 0;
  } else {
    injectionIndex = child.isEmpty() ? 0 : child.numberOfChildren();
    removalIndex = h.numberOfChildren() - 1;
    removalEnd = thisIndex + 1;
  }
  while (removalIndex >= removalEnd) {
    Layout l = h.childAtIndex(removalIndex);
    removalIndex -= 1 + h.removeChild(l, cursor);
    child.addOrMergeChildAtIndex(l, injectionIndex, true, cursor);
    /* InjectionIndex usually stays the same. Sometimes, an empty layout may
     * have been removed while inserting l. For example :
     * (1§▯|) absorbing 234| -> (1§|4) absorbing 23|4 -> (1§|34) absorbing 2|34
     * with § being the prefix superscript and ▯ an empty layout
     * TODO : addOrMergeChildAtIndex should take &injectionIndex and update it
     *        itself. */
    int injectedIndex = child.indexOfChild(l);
    if (injectedIndex >= 0) {
      injectionIndex = injectedIndex;
    }
  }
  assert(removalIndex == removalEnd - 1);
  if (child.numberOfChildren() == 0) {
    thisRef.replaceChild(child, EmptyLayout::Builder(EmptyLayoutNode::Color::Yellow, EmptyLayoutNode::Visibility::Never));
  }
}

LayoutNode * AutocompletedBracketPairLayoutNode::childOnSide(Side side) const {
  LayoutNode * child = childLayout();
  if (child->type() == LayoutNode::Type::HorizontalLayout) {
    assert(child->numberOfChildren() > 0);
    return child->childAtIndex(side == Side::Left ? 0 : child->numberOfChildren() - 1);
  }
  return child;
}

LayoutCursor AutocompletedBracketPairLayoutNode::cursorAfterDeletion(Side side) const {
  /* Attempting to delete a bracket can cause the children and sibling to be
   * shuffled around and this to disappear. Before the deletion occurs, anchor
   * the cursor to a layout that is bound to remain after the deletion. For
   * instance, if the bracket changes children, pointing on its horizontal
   * layout might not be safe.
   * In the following comments, the text before the arrow depicts the layout
   * as it currently stands, and the text after the arrow the future state of
   * the Layout after deletion. */
  Layout thisRef(this);
  Layout childRef(childLayout());
  Layout parentRef = thisRef.parent();
  bool parentIsHorizontalLayout = parentRef.type() == Type::HorizontalLayout;
  assert(!parentRef.isUninitialized());
  int thisIndex = parentRef.indexOfChild(thisRef);
  bool willDisappear = isTemporary(OtherSide(side));

  AutocompletedBracketPairLayoutNode * topMostBracket = const_cast<AutocompletedBracketPairLayoutNode *>(this);
  AutocompletedBracketPairLayoutNode * parentBracket = autocompletedParent();
  while(parentBracket) {
    topMostBracket = parentBracket;
    parentBracket = parentBracket->autocompletedParent();
  }
  bool parentWillDisappear = topMostBracket->isTemporary(OtherSide(side));

  if (side == Side::Left) {
    if (parentIsHorizontalLayout && thisIndex > 0) {
      /* e.g. 12(|34) -> [12|34) or 12(|▯^3) -> [12|^3) */
      return LayoutCursor(parentRef.childAtIndex(thisIndex - 1), LayoutCursor::Position::Right);
    }
    if (willDisappear || parentWillDisappear) {
      /* e.g. (|12] -> |12 or ((|12)] -> (|12) or ((|)] -> (|) */
      return LayoutCursor(childOnSide(Side::Left), LayoutCursor::Position::Left);
    }
    assert(!willDisappear);
    /* e.g. (|12) -> |[12) */
    return LayoutCursor(thisRef, LayoutCursor::Position::Left);
  }

  assert(side == Side::Right);
  if (thisIndex < parentRef.numberOfChildren() - 1) {
    assert(!(willDisappear && parentIsHorizontalLayout && childRef.isEmpty()));
    /* e.g. ()|34 -> (|34] or (12)|3 -> (12|3] or (1§▯)|23 -> (1§|23] */
    return LayoutCursor(parentRef.childAtIndex(thisIndex + 1), LayoutCursor::Position::Left);
  }
  if (!(parentIsHorizontalLayout && childRef.isEmpty())) {
    /* e.g. (12)| -> (12|] */
    return LayoutCursor(childOnSide(Side::Right), LayoutCursor::Position::Right);
  }
  assert(!willDisappear);
  /* e.g. ()| -> (|] */
  return LayoutCursor(childOnSide(Side::Left), LayoutCursor::Position::Left);
}

}
