#include <poincare/vertical_offset_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/parenthesis_layout.h>
#include <poincare/serialization_helper.h>
#include <string.h>
#include <assert.h>
#include <algorithm>

namespace Poincare {
/*
void VerticalOffsetLayoutNode::moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited, bool forSelection) {
  if (verticalPosition() == VerticalPosition::Superscript) {
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
       * same position, pointing this. *
      cursor->setLayoutNode(this);
      return;
    }
  }
  LayoutNode::moveCursorUp(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

void VerticalOffsetLayoutNode::moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited, bool forSelection) {
  if (verticalPosition() == VerticalPosition::Subscript) {
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
       * same position, pointing this. *
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
      cursor->setLayoutNode(base);
      cursor->setPosition(LayoutCursor::Position::Right);
      parentRef.removeChild(this, cursor);
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
*/
int VerticalOffsetLayoutNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (verticalPosition() == VerticalPosition::Subscript || horizontalPosition() == HorizontalPosition::Prefix) {
    if (bufferSize == 0) {
      return bufferSize-1;
    }
    buffer[bufferSize-1] = 0;
    if (bufferSize == 1) {
      return bufferSize-1;
    }

    /* If the layout is a subscript or a prefix, write "\x14{indice\x14}".
     * System braces are used to avoid confusion with lists. */
    int numberOfChar = SerializationHelper::CodePoint(buffer, bufferSize, UCodePointSystem);
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
    numberOfChar += SerializationHelper::CodePoint(buffer + numberOfChar, bufferSize - numberOfChar, '{');
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

    numberOfChar += const_cast<VerticalOffsetLayoutNode *>(this)->indiceLayout()->serialize(buffer + numberOfChar, bufferSize - numberOfChar, floatDisplayMode, numberOfSignificantDigits);
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

    numberOfChar += SerializationHelper::CodePoint(buffer + numberOfChar, bufferSize - numberOfChar, UCodePointSystem);
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
    numberOfChar += SerializationHelper::CodePoint(buffer + numberOfChar, bufferSize - numberOfChar, '}');
    return std::min(numberOfChar, bufferSize-1);
  }

  assert(verticalPosition() == VerticalPosition::Superscript);
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

int VerticalOffsetLayoutNode::indexOfNextChildToPointToAfterVerticalCursorMove(OMG::VerticalDirection direction, int currentIndex, PositionInLayout positionAtCurrentIndex) const {
  if (currentIndex == k_outsideIndex && ((direction == OMG::VerticalDirection::Up && m_verticalPosition == VerticalPosition::Superscript) || (direction == OMG::VerticalDirection::Down && m_verticalPosition == VerticalPosition::Subscript))) {
    return 0;
  }
  if (currentIndex == 0 && ((direction == OMG::VerticalDirection::Down && m_verticalPosition == VerticalPosition::Superscript) || (direction == OMG::VerticalDirection::Up && m_verticalPosition == VerticalPosition::Subscript)) && positionAtCurrentIndex != PositionInLayout::Middle) {
    return k_outsideIndex;
  }
  return k_cantMoveIndex;
}

KDSize VerticalOffsetLayoutNode::computeSize(KDFont::Size font) {
  KDSize indiceSize = indiceLayout()->layoutSize(font);
  KDCoordinate width = indiceSize.width();
  if (verticalPosition() == VerticalPosition::Superscript) {
    LayoutNode * parentNode = parent();
    assert(parentNode != nullptr);
    assert(parentNode->isHorizontal());
    int idxInParent = parentNode->indexOfChild(this);
    if (idxInParent < parentNode->numberOfChildren() - 1 && parentNode->childAtIndex(idxInParent + 1)->hasUpperLeftIndex()) {
      width += k_separationMargin;
    }
  }
  if (m_emptyBaseVisibility == EmptyRectangle::State::Visible && baseLayout() == nullptr) {
    width += EmptyRectangle::RectangleSize(font).width();
  }
  KDCoordinate height = baseSize(font).height() - k_indiceHeight + indiceLayout()->layoutSize(font).height();
  return KDSize(width, height);
}

KDCoordinate VerticalOffsetLayoutNode::computeBaseline(KDFont::Size font) {
  if (verticalPosition() == VerticalPosition::Subscript) {
    return baseBaseline(font);
  }
  assert(verticalPosition() == VerticalPosition::Superscript);
  return indiceLayout()->layoutSize(font).height() - k_indiceHeight + baseBaseline(font);
}

KDPoint VerticalOffsetLayoutNode::positionOfChild(LayoutNode * child, KDFont::Size font) {
  assert(child == indiceLayout());
  KDCoordinate shiftForEmpty = baseLayout() == nullptr && horizontalPosition() == HorizontalPosition::Suffix ? baseSize(font).width() : 0;
  if (verticalPosition() == VerticalPosition::Superscript) {
    return KDPoint(shiftForEmpty, 0);
  }
  assert(verticalPosition() == VerticalPosition::Subscript);
  return KDPoint(shiftForEmpty, baseSize(font).height() - k_indiceHeight);
}
/*
bool VerticalOffsetLayoutNode::willAddSibling(LayoutCursor * cursor, Layout * sibling, bool moveCursor) {
  if (sibling->type() != Type::VerticalOffsetLayout) {
    return true;
  }
  VerticalOffsetLayoutNode * verticalOffsetSibling = static_cast<VerticalOffsetLayoutNode *>(sibling->node());
  if (verticalOffsetSibling->verticalPosition() != VerticalPosition::Superscript) {
    return true;
  }
  /* A power will be inserted next to another power. To avoid ambiguity between
   * a^(b^c) and (a^b)^c when representing a^b^c, add parentheses to make (a^b)^c. *
  Layout thisRef = Layout(this);
  Layout parentRef = Layout(parent());
  assert(parentRef.isHorizontal());
  int thisIndex = parentRef.indexOfChild(thisRef);
  int leftParenthesisIndex = thisIndex - 1;
  int numberOfOpenParenthesis = 0;
  while (leftParenthesisIndex >= 0 && parentRef.childAtIndex(leftParenthesisIndex).isCollapsable(&numberOfOpenParenthesis, true)) {
    leftParenthesisIndex--;
  }

  if (cursor->position() == LayoutCursor::Position::Left) {
    /* If b is insterted on the left of c in a^c, output (a^b)^c. *
    parentRef.addChildAtIndex(*sibling, thisIndex, parentRef.numberOfChildren(), nullptr);
  }

  HorizontalLayout h = HorizontalLayout::Builder();
  int n = 0;
  int i = thisIndex;
  while (i > leftParenthesisIndex) {
    Layout child = parentRef.childAtIndex(i);
    parentRef.removeChild(child, nullptr, true);
    i--;
    h.addChildAtIndex(child, 0, n++, nullptr);
  }
  assert(n > 0 && i == leftParenthesisIndex);
  ParenthesisLayout parentheses = ParenthesisLayout::Builder(h);
  parentRef.addChildAtIndex(parentheses, leftParenthesisIndex + 1, parentRef.numberOfChildren(), nullptr);

  if (cursor->position() == LayoutCursor::Position::Right) {
    /* If b is insterted on the right of c in a^c, output (a^c)^b. *
    parentRef.addChildAtIndex(*sibling, leftParenthesisIndex + 2, parentRef.numberOfChildren(), nullptr);
  }

  assert(!parentheses.parent().isUninitialized());
  if (cursor->position() == LayoutCursor::Position::Left) {
    cursor->setLayout(h);
    cursor->setPosition(LayoutCursor::Position::Right);
  } else {
    cursor->setLayout(parentheses);
  }
  return false;
}*/

LayoutNode * VerticalOffsetLayoutNode::baseLayout() {
  LayoutNode * parentNode = parent();
  assert(parentNode != nullptr);
  if (!parentNode) {
    return nullptr;
  }
  int idxInParent = parentNode->indexOfChild(this);
  assert(idxInParent >= 0);
  int baseIndex = idxInParent + baseOffsetInParent();
  if (baseIndex < 0 || baseIndex >= parent()->numberOfChildren()) {
    return nullptr;
  }
  return parentNode->childAtIndex(baseIndex);
}

KDSize VerticalOffsetLayoutNode::baseSize(KDFont::Size font) {
  return baseLayout() ? baseLayout()->layoutSize(font) : (m_emptyBaseVisibility == EmptyRectangle::State::Visible ? EmptyRectangle::RectangleSize(font) : KDSize(0, EmptyRectangle::RectangleSize(font).height()));
}

KDCoordinate VerticalOffsetLayoutNode::baseBaseline(KDFont::Size font) {
  return baseLayout() ? baseLayout()->baseline(font) : EmptyRectangle::RectangleSize(font).height() / 2;
}

bool VerticalOffsetLayoutNode::protectedIsIdenticalTo(Layout l) {
  assert(l.type() == Type::VerticalOffsetLayout);
  VerticalOffsetLayoutNode * n = static_cast<VerticalOffsetLayoutNode *>(l.node());
  return verticalPosition() == n->verticalPosition() && horizontalPosition() == n->horizontalPosition() && LayoutNode::protectedIsIdenticalTo(l);
}

void VerticalOffsetLayoutNode::render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor) {
  if (baseLayout() || m_emptyBaseVisibility == EmptyRectangle::State::Hidden) {
    return;
  }
  KDCoordinate emptyRectangleHorizontalOrigin;
  if (horizontalPosition() == HorizontalPosition::Suffix) {
    emptyRectangleHorizontalOrigin = 0;
  } else {
    assert(horizontalPosition() == HorizontalPosition::Prefix);
    KDCoordinate totalWidth = layoutSize(font).width();
    emptyRectangleHorizontalOrigin = totalWidth - EmptyRectangle::RectangleSize(font).width();
  }
  KDPoint emptyRectangleOrigin = p.translatedBy(KDPoint(emptyRectangleHorizontalOrigin, baseline(font) - baseBaseline(font)));
  EmptyRectangle::DrawEmptyRectangle(ctx, emptyRectangleOrigin, font, EmptyRectangle::Color::Yellow);
}

VerticalOffsetLayout VerticalOffsetLayout::Builder(Layout l, VerticalOffsetLayoutNode::VerticalPosition verticalPosition, VerticalOffsetLayoutNode::HorizontalPosition horizontalPosition) {
  void * bufferNode = TreePool::sharedPool->alloc(sizeof(VerticalOffsetLayoutNode));
  VerticalOffsetLayoutNode * node = new (bufferNode) VerticalOffsetLayoutNode(verticalPosition, horizontalPosition);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  h.replaceChildAtIndexInPlace(0, l);
  return static_cast<VerticalOffsetLayout &>(h);
}

}
