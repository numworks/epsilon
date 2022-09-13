#include <poincare/autocompleted_bracket_pair_layout.h>
#include <poincare/empty_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout.h>

namespace Poincare {

bool AutocompletedBracketPairLayoutNode::willAddSibling(LayoutCursor * cursor, LayoutNode * sibling, bool moveCursor) {
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
  AutocompletedBracketPairLayoutNode * bracketSibling = sibling->type() == type() ? static_cast<AutocompletedBracketPairLayoutNode *>(sibling) : nullptr;
  bool ignoreSibling = bracketSibling && bracketSibling->m_insertedAs == insertionSide;
  if (ignoreSibling && insertionSide == Side::Left) {
    cursor->setPosition(LayoutCursor::Position::Left);
    cursor->setLayout(Layout(childLayout()));
  }
  return !ignoreSibling;
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
    return BracketPairLayoutNode::deleteBeforeCursor(cursor);
  }

  LayoutCursor nextCursor = cursorAfterDeletion(deletionSide);
  makeTemporary(deletionSide, cursor);
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

void AutocompletedBracketPairLayoutNode::balanceAfterInsertion(Side insertedSide, LayoutCursor * cursor) {
  assert(cursor);
  Layout thisRef(this);
  makeTemporary(OtherSide(insertedSide), cursor);
  if (insertedSide == Side::Left) {
    cursor->setPosition(LayoutCursor::Position::Left);
    cursor->setLayout(thisRef.childAtIndex(0));
  } else {
    cursor->setPosition(LayoutCursor::Position::Right);
    cursor->setLayout(thisRef);
  }
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
    newThis->removeIfCompletelyTemporary(cursor);
  }
  return true;
}

void AutocompletedBracketPairLayoutNode::removeIfCompletelyTemporary(LayoutCursor * cursor) {
  if (!(isTemporary(Side::Left) && isTemporary(Side::Right))) {
    return;
  }
  assert(parent());
  Layout thisRef = Layout(this);
  Layout childRef = thisRef.childAtIndex(0);
  Layout parentRef = thisRef.parent();
  parentRef.replaceChild(thisRef, childRef, cursor);
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

  int injectionIndex, removalStart, removalEnd;
  if (side == Side::Left) {
    injectionIndex = 0;
    removalStart = thisIndex - 1;
    removalEnd = 0;
  } else {
    injectionIndex = child.isEmpty() ? 0 : child.numberOfChildren();
    removalStart = h.numberOfChildren() - 1;
    removalEnd = thisIndex + 1;
  }
  for (int i = removalStart; i >= removalEnd; i--) {
    Layout l = h.childAtIndex(i);
    h.removeChild(l, cursor);
    child.addOrMergeChildAtIndex(l, injectionIndex, true, cursor);
  }
  if (child.numberOfChildren() == 0) {
    thisRef.replaceChild(child, EmptyLayout::Builder(EmptyLayoutNode::Color::Yellow, EmptyLayoutNode::Visibility::Never));
  }
}

static Layout childBypassHorizontalLayout(Layout child, int index) {
  if (child.type() == LayoutNode::Type::HorizontalLayout) {
    assert(child.numberOfChildren() > 0);
    return child.childAtIndex(index);
  }
  return child;
}

LayoutCursor AutocompletedBracketPairLayoutNode::cursorAfterDeletion(Side side) const {
  Layout thisRef(this);
  Layout childRef(childLayout());
  Layout parentRef = thisRef.parent();
  assert(!parentRef.isUninitialized());
  int thisIndex = parentRef.indexOfChild(thisRef);
  bool willDisappear = isTemporary(OtherSide(side));

  if (side == Side::Left) {
    if (thisIndex > 0) {
      return LayoutCursor(parentRef.childAtIndex(thisIndex - 1), LayoutCursor::Position::Right);
    }
    if (willDisappear) {
      assert(!childRef.isEmpty());
      return LayoutCursor(childBypassHorizontalLayout(childRef, 0), LayoutCursor::Position::Left);
    }
    return LayoutCursor(thisRef, LayoutCursor::Position::Left);
  }

  assert(side == Side::Right);
  if (!childRef.isEmpty()) {
    return LayoutCursor(childBypassHorizontalLayout(childRef, childRef.numberOfChildren() - 1), LayoutCursor::Position::Right);
  }
  assert(!willDisappear);
  if (thisIndex < parentRef.numberOfChildren() - 1) {
    return LayoutCursor(parentRef.childAtIndex(thisIndex + 1), LayoutCursor::Position::Left);
  }
  return LayoutCursor(childBypassHorizontalLayout(childRef, 0), LayoutCursor::Position::Left);
}

void AutocompletedBracketPairLayoutNode::makePermanent(Side side) {
  if (!isTemporary(side)) {
    return;
  }
  int childIndex = side == Side::Left ? 0 : childLayout()->numberOfChildren() - 1;
  Layout child = childBypassHorizontalLayout(Layout(childLayout()), childIndex);
  if (type() == child.type()) {
    AutocompletedBracketPairLayoutNode * bracket = static_cast<AutocompletedBracketPairLayoutNode *>(child.node());
    bracket->makePermanent(side);
  }
  setTemporary(side, false);
}

}
