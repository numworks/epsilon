#include <poincare/autocompleted_bracket_pair_layout.h>
#include <poincare/empty_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout.h>
#include <poincare/layout_cursor.h>

namespace Poincare {

void AutocompletedBracketPairLayoutNode::deleteBeforeCursor(LayoutCursor * cursor) {
  Layout thisRef = Layout(this);
  int previousNumberOfGrandchildren = childLayout()->numberOfChildren();
  if (cursor->isEquivalentTo(LayoutCursor(this, LayoutCursor::Position::Right))) {
    if (isTemporary(Side::Right)) {
      cursor->setLayout(thisRef.childAtIndex(0));
      return;
    }
    makeTemporary(Side::Right, cursor);
    // cursor->setLayout(thisRef.childAtIndex(previousNumberOfGrandchildren - 1));
  } else if (cursor->isEquivalentTo(LayoutCursor(childLayout(), LayoutCursor::Position::Left))) {
    if (isTemporary(Side::Left)) {
      cursor->setLayout(thisRef);
      return;
    }
    makeTemporary(Side::Left, cursor);
    // cursor->setLayout(thisRef.childAtIndex(childLayout()->numberOfChildren() - previousNumberOfGrandchildren));
  }
}

void AutocompletedBracketPairLayoutNode::makeTemporary(Side side, LayoutCursor * cursor) {
  absorbSiblings(side, cursor);
  AutocompletedBracketPairLayoutNode * p = autocompletedParent();
  if (p) {
    p->makeTemporary(side, cursor);
  } else {
    m_status |= MaskForSide(side);
    removeIfCompletelyTemporary(cursor);
  }
}

/*
void AutocompletedBracketPairLayoutNode::makePermanent(Side side, int beginIndex, LayoutCursor * cursor) {
  AutocompletedBracketPairLayoutNode * ap = autocompletedParent();
  if (ap) {
    LayoutNode * p = parent();
    assert(p);
    ap->makePermanent(side, p->indexOfChild(this) + (side == Side::Left ? -1 : 1), cursor);
  } else {
    m_status &= ~MaskForSide(side);
  }
  dumpChildrenInParent()
}
*/

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

  HorizontalLayout newChild = HorizontalLayout::Builder();
  Layout oldChild = Layout(childLayout());
  oldChild.replaceWithInPlace(newChild);
  newChild.addOrMergeChildAtIndex(oldChild, 0, true, cursor);

  int injectionIndex, removalStart, removalEnd;
  if (side == Side::Left) {
    injectionIndex = 0;
    removalStart = thisIndex - 1;
    removalEnd = 0;
  } else {
    injectionIndex = newChild.numberOfChildren();
    removalStart = h.numberOfChildren() - 1;
    removalEnd = thisIndex + 1;
  }
  for (int i = removalStart; i >= removalEnd; i--) {
    Layout l = h.childAtIndex(i);
    h.removeChild(l, cursor);
    newChild.addOrMergeChildAtIndex(l, injectionIndex, true, cursor);
  }
  if (newChild.numberOfChildren() == 0) {
    thisRef.replaceChild(newChild, EmptyLayout::Builder(EmptyLayoutNode::Color::Yellow, EmptyLayoutNode::Visibility::Never));
  }
}

/*
void AutocompletedBracketPairLayoutNode::dumpChildrenInParent(Side side, int beginIndex, LayoutCursor * cursor) {
  // TODO factor with absorbSiblings
  Layout thisRef = Layout(this);
  Layout p = parent();
  assert(!p.isUninitialized());
  assert(p.type() == LayoutNode::Type::HorizontalLayout);
  HorizontalLayout h = static_cast<HorizontalLayout &>(p);
  int thisIndex = h.indexOfChild(thisRef);

  Layout c = Layout(childLayout());
  assert(c.type() == LayoutNode::Type::HorizontalLayout);
  HorizontalLayout hc = static_cast<HorizontalLayout &>(c);

  int injectionIndex = thisIndex;
  int removalStart, removalEnd;
  if (side == Side::Left) {
    injectionIndex--;
    removalStart = beginIndex;
    removalEnd = 0;
  } else {
    injectionIndex++;
    removalStart = hc.numberOfChildren() - 1;
    removalEnd = beginIndex;
  }

  for (int i = removalStart; i >= removalEnd; i--) {
    h.addOrMergeChildAtIndex(hc.childAtIndex(i), injectionIndex, true, cursor);
  }
}
*/

static bool isAutocompletedBracket(LayoutNode * l) { return l->type() == LayoutNode::Type::ParenthesisLayout || l->type() == LayoutNode::Type::CurlyBraceLayout; }

AutocompletedBracketPairLayoutNode * AutocompletedBracketPairLayoutNode::autocompletedParent() {
  LayoutNode * p = parent();
  while (p) {
    if (isAutocompletedBracket(p)) {
      return static_cast<AutocompletedBracketPairLayoutNode *>(p);
    }
    if (p->type() != Type::HorizontalLayout) {
      break;
    }
    p = p->parent();
  }
  return nullptr;
}

}
