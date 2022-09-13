#include <poincare/vertical_offset_layout.h>
#include <poincare/empty_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/parenthesis_layout.h>
#include <poincare/serialization_helper.h>
#include <string.h>
#include <assert.h>
#include <algorithm>

namespace Poincare {

void VerticalOffsetLayoutNode::moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  if (cursor->layoutNode() == indiceLayout()
      && cursor->position() == LayoutCursor::Position::Left)
  {
    // Case: Left of the indice. Go Left.
    cursor->setLayoutNode(this);
    return;
  }
  assert(cursor->layoutNode() == this);
  if (cursor->position() == LayoutCursor::Position::Right) {
    // Case: Right. Go to the indice.
    assert(indiceLayout() != nullptr);
    cursor->setLayoutNode(indiceLayout());
    return;
  }
  // Case: Left. Ask the parent.
  assert(cursor->position() == LayoutCursor::Position::Left);
  LayoutNode * parentNode = parent();
  if (parentNode != nullptr) {
    parentNode->moveCursorLeft(cursor, shouldRecomputeLayout);
  }
}

void VerticalOffsetLayoutNode::moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  if (cursor->layoutNode() == indiceLayout()
      && cursor->position() == LayoutCursor::Position::Right)
  {
    // Case: Right of the indice. Go Right.
    cursor->setLayoutNode(this);
    return;
  }
  assert(cursor->layoutNode() == this);
  if (cursor->position() == LayoutCursor::Position::Left) {
    // Case: Left. Go to the indice.
    cursor->setLayoutNode(indiceLayout());
    return;
  }
  // Case: Right. Ask the parent.
  assert(cursor->position() == LayoutCursor::Position::Right);
  LayoutNode * parentNode = parent();
  if (parentNode != nullptr) {
    parentNode->moveCursorRight(cursor, shouldRecomputeLayout);
  }
}

void VerticalOffsetLayoutNode::moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited, bool forSelection) {
  if (m_position == Position::Superscript) {
    // Case: Superscript.
    if (cursor->isEquivalentTo(LayoutCursor(this, LayoutCursor::Position::Right))) {
      // Case: Right. Move to the indice.
      cursor->setLayoutNode(indiceLayout());
      cursor->setPosition(LayoutCursor::Position::Right);
      return;
    }
    if (cursor->isEquivalentTo(LayoutCursor(this, LayoutCursor::Position::Left))) {
      // Case: Left. Move to the indice.
      cursor->setLayoutNode(indiceLayout());
      cursor->setPosition(LayoutCursor::Position::Left);
      return;
    }
  } else {
    if (cursor->isEquivalentTo(LayoutCursor(Layout(indiceLayout()), cursor->position()))) {
      /* Case: Subscript, Left or Right of the indice. Put the cursor at the
       * same position, pointing this. */
      cursor->setLayoutNode(this);
      return;
    }
  }
  LayoutNode::moveCursorUp(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

void VerticalOffsetLayoutNode::moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited, bool forSelection) {
  if (m_position == Position::Subscript) {
    // Case: Subscript.
    if (cursor->isEquivalentTo(LayoutCursor(this, LayoutCursor::Position::Right))) {
      // Case: Right. Move to the indice.
      cursor->setLayoutNode(indiceLayout());
      cursor->setPosition(LayoutCursor::Position::Right);
      return;
   }
    // Case: Left. Move to the indice.
    if (cursor->isEquivalentTo(LayoutCursor(this, LayoutCursor::Position::Left))) {
      cursor->setLayoutNode(indiceLayout());
      cursor->setPosition(LayoutCursor::Position::Left);
      return;
    }
  } else {
    if (cursor->isEquivalentTo(LayoutCursor(Layout(indiceLayout()), cursor->position()))){
      /* Case: Superscript, Left or Right of the indice. Put the cursor at the
       * same position, pointing this. */
      cursor->setLayoutNode(this);
      return;
    }
  }
  LayoutNode::moveCursorDown(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

void VerticalOffsetLayoutNode::deleteBeforeCursor(LayoutCursor * cursor) {
  if (cursor->layoutNode() == indiceLayout()) {
    assert(cursor->position() == LayoutCursor::Position::Left);
    Layout parentRef(parent());
    LayoutNode * base = baseLayout();
    if (indiceLayout()->isEmpty()) {
      int indexInParent = parentRef.node()->indexOfChild(this);
      if (base->isEmpty()) {
        // Case: Empty base and indice. Remove the base and the indice layouts.
        cursor->setLayoutNode(this);
        cursor->setPosition(LayoutCursor::Position::Right);
        parentRef.removeChildAtIndex(indexInParent, cursor);
        // WARNING: do not call "this" afterwards
        cursor->setLayout(parentRef.childAtIndex(indexInParent-1));
        cursor->setPosition(LayoutCursor::Position::Right);
        parentRef.removeChildAtIndex(indexInParent-1, cursor);
        return;
      }
      // Case: Empty indice only. Delete the layout.
      cursor->setLayoutNode(base);
      cursor->setPosition(LayoutCursor::Position::Right);
      parentRef.removeChildAtIndex(indexInParent, cursor);
      // WARNING: do not call "this" afterwards
      return;
    }
    // Case: Non-empty indice. Move Left of the VerticalOffsetLayoutNode.
    cursor->setLayoutNode(this);
    cursor->setPosition(LayoutCursor::Position::Left);
    return;
  }
  if (cursor->layoutNode() == this
      && cursor->position() == LayoutCursor::Position::Right)
  {
    // Case: Right. Move to the indice.
    cursor->setLayoutNode(indiceLayout());
    return;
  }
  LayoutNode::deleteBeforeCursor(cursor);
}

int VerticalOffsetLayoutNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (m_position == Position::Subscript) {
    if (bufferSize == 0) {
      return bufferSize-1;
    }
    buffer[bufferSize-1] = 0;
    if (bufferSize == 1) {
      return bufferSize-1;
    }

    // If the layout is a subscript, write "{indice}"
    int numberOfChar = SerializationHelper::CodePoint(buffer, bufferSize, '{');
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

    numberOfChar += const_cast<VerticalOffsetLayoutNode *>(this)->indiceLayout()->serialize(buffer+numberOfChar, bufferSize-numberOfChar, floatDisplayMode, numberOfSignificantDigits);
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

    numberOfChar += SerializationHelper::CodePoint(buffer+numberOfChar, bufferSize-numberOfChar, '}');
    return std::min(numberOfChar, bufferSize-1);
  }

  assert(m_position == Position::Superscript);
  // If the layout is a superscript, write: '^' 'System(' indice 'System)'
  int numberOfChar = SerializationHelper::CodePoint(buffer, bufferSize, '^');
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
  numberOfChar += SerializationHelper::CodePoint(buffer+numberOfChar, bufferSize-numberOfChar, UCodePointLeftSystemParenthesis);
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
  numberOfChar += const_cast<VerticalOffsetLayoutNode *>(this)->indiceLayout()->serialize(buffer+numberOfChar, bufferSize-numberOfChar, floatDisplayMode, numberOfSignificantDigits);
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
  numberOfChar += SerializationHelper::CodePoint(buffer+numberOfChar, bufferSize-numberOfChar, UCodePointRightSystemParenthesis);
  return std::min(numberOfChar, bufferSize-1);
}

KDSize VerticalOffsetLayoutNode::computeSize(KDFont::Size font) {
  KDSize indiceSize = indiceLayout()->layoutSize(font);
  KDCoordinate width = indiceSize.width();
  if (m_position == Position::Superscript) {
    LayoutNode * parentNode = parent();
    assert(parentNode != nullptr);
    assert(parentNode->type() == Type::HorizontalLayout);
    int idxInParent = parentNode->indexOfChild(this);
    if (idxInParent < parentNode->numberOfChildren() - 1 && parentNode->childAtIndex(idxInParent + 1)->hasUpperLeftIndex()) {
      width += k_separationMargin;
    }
  }
  KDCoordinate height = baseLayout()->layoutSize(font).height() - k_indiceHeight + indiceLayout()->layoutSize(font).height();
  return KDSize(width, height);
}

KDCoordinate VerticalOffsetLayoutNode::computeBaseline(KDFont::Size font) {
  if (m_position == Position::Subscript) {
    return baseLayout()->baseline(font);
  } else {
    return indiceLayout()->layoutSize(font).height() - k_indiceHeight + baseLayout()->baseline(font);
  }
}

KDPoint VerticalOffsetLayoutNode::positionOfChild(LayoutNode * child, KDFont::Size font) {
  assert(child == indiceLayout());
  if (m_position == Position::Superscript) {
    return KDPointZero;
  }
  assert(m_position == Position::Subscript);
  return KDPoint(0, baseLayout()->layoutSize(font).height() - k_indiceHeight);
}

bool VerticalOffsetLayoutNode::willAddSibling(LayoutCursor * cursor, LayoutNode * sibling, bool moveCursor) {
  if (sibling->type() != Type::VerticalOffsetLayout) {
    return true;
  }
  VerticalOffsetLayoutNode * verticalOffsetSibling = static_cast<VerticalOffsetLayoutNode *>(sibling);
  if (verticalOffsetSibling->position() != Position::Superscript) {
    return true;
  }
  /* A power will be inserted next to another power. To avoid ambiguity between
   * a^(b^c) and (a^b)^c when representing a^b^c, add parentheses to make (a^b)^c. */
  Layout thisRef = Layout(this);
  Layout parentRef = Layout(parent());
  Layout siblingRef = Layout(sibling);
  assert(parentRef.type() == Type::HorizontalLayout);
  int thisIndex = parentRef.indexOfChild(thisRef);
  int leftParenthesisIndex = thisIndex - 1;
  int numberOfOpenParenthesis = 0;
  while (leftParenthesisIndex >= 0 && parentRef.childAtIndex(leftParenthesisIndex).isCollapsable(&numberOfOpenParenthesis, true)) {
    leftParenthesisIndex--;
  }
  HorizontalLayout h = HorizontalLayout::Builder();
  int n = 0;
  for (int i = thisIndex - (cursor->position() == LayoutCursor::Position::Left); i > leftParenthesisIndex; i--) {
    Layout child = parentRef.childAtIndex(i);
    parentRef.removeChild(child, nullptr, true);
    h.addChildAtIndex(child, 0, n++, nullptr);
  }
  ParenthesisLayout parentheses = ParenthesisLayout::Builder(h);
  parentRef.addChildAtIndex(parentheses, leftParenthesisIndex + 1, parentRef.numberOfChildren(), nullptr);
  /* Handle the sibling insertion, as the index might have changed after
   * collapsing nodes inside the parenthesis. */
  parentRef.addChildAtIndex(siblingRef, leftParenthesisIndex + 2, parentRef.numberOfChildren(), nullptr);
  if (!parentheses.parent().isUninitialized()) {
    if (cursor->position() == LayoutCursor::Position::Left) {
      cursor->setLayout(h);
      cursor->setPosition(LayoutCursor::Position::Right);
    } else {
      cursor->setLayout(parentheses);
    }
  }
  return false;
}

LayoutNode * VerticalOffsetLayoutNode::baseLayout() {
  LayoutNode * parentNode = parent();
  assert(parentNode != nullptr);
  assert(parentNode->type() == Type::HorizontalLayout);
  int idxInParent = parentNode->indexOfChild(this);
  assert(idxInParent > 0);
  return parentNode->childAtIndex(idxInParent - 1);
}

bool VerticalOffsetLayoutNode::protectedIsIdenticalTo(Layout l) {
  assert(l.type() == Type::VerticalOffsetLayout);
  VerticalOffsetLayoutNode * n = static_cast<VerticalOffsetLayoutNode *>(l.node());
  return position() == n->position() && LayoutNode::protectedIsIdenticalTo(l);
}

VerticalOffsetLayout VerticalOffsetLayout::Builder(Layout l, VerticalOffsetLayoutNode::Position position) {
  void * bufferNode = TreePool::sharedPool()->alloc(sizeof(VerticalOffsetLayoutNode));
  VerticalOffsetLayoutNode * node = new (bufferNode) VerticalOffsetLayoutNode(position);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  h.replaceChildAtIndexInPlace(0, l);
  return static_cast<VerticalOffsetLayout &>(h);
}

}
