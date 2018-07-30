#include <poincare/vertical_offset_layout_node.h>
#include <poincare/empty_layout_node.h>
#include <poincare/layout_engine.h>
#include <poincare/left_parenthesis_layout_node.h>
#include <poincare/right_parenthesis_layout_node.h>
#include <ion/charset.h>
#include <string.h>
#include <assert.h>

namespace Poincare {

void VerticalOffsetLayoutNode::moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) {
  if (indiceLayout() != nullptr
      && cursor->layoutNode() == indiceLayout()
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

void VerticalOffsetLayoutNode::moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) {
  if (indiceLayout() != nullptr
      && cursor->layoutNode() == indiceLayout()
      && cursor->position() == LayoutCursor::Position::Right)
  {
    // Case: Right of the indice. Go Right.
    cursor->setLayoutNode(this);
    return;
  }
  assert(cursor->layoutNode() == this);
  if (cursor->position() == LayoutCursor::Position::Left) {
    // Case: Left. Go to the indice.
    assert(indiceLayout() != nullptr);
    cursor->setLayoutNode(indiceLayout());
    return;
  }
  // Case: Right. Ask the parent.
  assert(cursor->position() == LayoutCursor::Position::Right);
  LayoutNode * parentNode = parent();
  if (parentNode) {
    parentNode->moveCursorRight(cursor, shouldRecomputeLayout);
  }
}

void VerticalOffsetLayoutNode::moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  if (m_type == Type::Superscript) {
    // Case: Superscript.
    if (cursor->isEquivalentTo(LayoutCursor(this, LayoutCursor::Position::Right))) {
      // Case: Right. Move to the indice.
      assert(indiceLayout() != nullptr);
      cursor->setLayoutNode(indiceLayout());
      cursor->setPosition(LayoutCursor::Position::Right);
      return;
    }
    if (cursor->isEquivalentTo(LayoutCursor(this, LayoutCursor::Position::Left))) {
      // Case: Left. Move to the indice.
      assert(indiceLayout() != nullptr);
      cursor->setLayoutNode(indiceLayout());
      cursor->setPosition(LayoutCursor::Position::Left);
      return;
    }
  }
  /* Case: Subscript, Left or Right of the indice. Put the cursor at the same
   * position, pointing this. */
  if (m_type == Type::Subscript
    && indiceLayout() != nullptr
    && (cursor->isEquivalentTo(LayoutCursor(indiceLayout(), LayoutCursor::Position::Left))
      || cursor->isEquivalentTo(LayoutCursor(indiceLayout(), LayoutCursor::Position::Right))))
  {
      cursor->setLayoutNode(this);
      return;
  }
  LayoutNode::moveCursorUp(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

void VerticalOffsetLayoutNode::moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  if (m_type == Type::Subscript) {
    // Case: Subscript.
    if (cursor->isEquivalentTo(LayoutCursor(this, LayoutCursor::Position::Right))) {
      // Case: Right. Move to the indice.
      assert(indiceLayout() != nullptr);
      cursor->setLayoutNode(indiceLayout());
      cursor->setPosition(LayoutCursor::Position::Right);
      return;
   }
    // Case: Left. Move to the indice.
    if (cursor->isEquivalentTo(LayoutCursor(this, LayoutCursor::Position::Left))) {
      assert(indiceLayout() != nullptr);
      cursor->setLayoutNode(indiceLayout());
      cursor->setPosition(LayoutCursor::Position::Left);
      return;
    }
  }
  /* Case: Superscript, Left or Right of the indice. Put the cursor at the same
   * position, pointing this. */
  if (m_type == Type::Superscript
    && indiceLayout() != nullptr
    && cursor->layoutNode() == indiceLayout())
  {
    cursor->setLayoutNode(this);
    return;
  }
  LayoutNode::moveCursorDown(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

void VerticalOffsetLayoutNode::deleteBeforeCursor(LayoutCursor * cursor) {
  if (cursor->layoutNode() == indiceLayout()) {
    assert(cursor->position() == LayoutCursor::Position::Left);
    LayoutRef parentRef(parent());
    LayoutNode * base = baseLayout();
    if (indiceLayout()->isEmpty()) {
      int indexInParent = parentRef.node()->indexOfChild(this);
      if (base->isEmpty()) {
        // Case: Empty base and indice. Remove the base and the indice layouts.
        cursor->setLayoutNode(this);
        cursor->setPosition(LayoutCursor::Position::Right);
        parentRef.removeChildAtIndex(indexInParent, cursor);
        // WARNING: do not call "this" afterwards
        cursor->setLayoutReference(parentRef.childAtIndex(indexInParent-1));
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

int VerticalOffsetLayoutNode::writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (m_type == Type::Subscript) {
    if (bufferSize == 0) {
      return -1;
    }
    buffer[bufferSize-1] = 0;
    if (bufferSize == 1) {
      return 0;
    }
    // If the layout is a subscript, write "_{indice}"
    int numberOfChar = LayoutEngine::writeOneCharInBuffer(buffer, bufferSize, '_');
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

    numberOfChar += LayoutEngine::writeOneCharInBuffer(buffer+numberOfChar, bufferSize-numberOfChar, '{');
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

    numberOfChar += const_cast<VerticalOffsetLayoutNode *>(this)->indiceLayout()->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar, floatDisplayMode, numberOfSignificantDigits);
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

    numberOfChar += LayoutEngine::writeOneCharInBuffer(buffer+numberOfChar, bufferSize-numberOfChar, '}');
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

    return numberOfChar;
  }
  assert(m_type == Type::Superscript);
  // If the layout is a superscript, write "^(indice)"
  int numberOfChar = LayoutEngine::writePrefixSerializableRefTextInBuffer(SerializableRef(const_cast<VerticalOffsetLayoutNode *>(this)), buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "^");
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

  // Add a multiplication if omitted.
  int indexInParent = -1;
  LayoutNode * parentNode = parent();
  if (parentNode) {
    indexInParent = parentNode->indexOfChild(this);
  }
  if (indexInParent >= 0 && indexInParent < (parentNode->numberOfChildren() - 1) && parentNode->isHorizontal() && parentNode->childAtIndex(indexInParent + 1)->canBeOmittedMultiplicationRightFactor()) {
    buffer[numberOfChar++] = Ion::Charset::MiddleDot;
    if (numberOfChar >= bufferSize-1) { return bufferSize-1;}
  }
  buffer[numberOfChar] = 0;
  return numberOfChar;
}

void VerticalOffsetLayoutNode::computeSize() {
  KDSize indiceSize = indiceLayout()->layoutSize();
  KDCoordinate width = indiceSize.width();
  if (m_type == Type::Superscript) {
    LayoutNode * parentNode = parent();
    assert(parentNode != nullptr);
    assert(parentNode->isHorizontal());
    int idxInParent = parentNode->indexOfChild(this);
    if (idxInParent < parentNode->numberOfChildren() - 1 && parentNode->childAtIndex(idxInParent + 1)->hasUpperLeftIndex()) {
      width += k_separationMargin;
    }
  }
  KDCoordinate height = baseLayout()->layoutSize().height() - k_indiceHeight + indiceLayout()->layoutSize().height();
  m_frame.setSize(KDSize(width, height));
  m_sized = true;
}

void VerticalOffsetLayoutNode::computeBaseline() {
  if (m_type == Type::Subscript) {
    m_baseline = baseLayout()->baseline();
  } else {
    m_baseline = indiceLayout()->layoutSize().height() - k_indiceHeight + baseLayout()->baseline();
  }
  m_baselined = true;
}

KDPoint VerticalOffsetLayoutNode::positionOfChild(LayoutNode * child) {
  assert(child == indiceLayout());
  if (m_type == Type::Superscript) {
    return KDPointZero;
  }
  assert(m_type == Type::Subscript);
  assert(baseLayout());
  return KDPoint(0, baseLayout()->layoutSize().height() - k_indiceHeight);
}

bool VerticalOffsetLayoutNode::willAddSibling(LayoutCursor * cursor, LayoutNode * sibling, bool moveCursor) {
  if (sibling->isVerticalOffset()) {
    VerticalOffsetLayoutNode * verticalOffsetSibling = static_cast<VerticalOffsetLayoutNode *>(sibling);
    if (verticalOffsetSibling->type() == Type::Superscript) {
      LayoutNode * parentNode = parent();
      LayoutRef parentRef = LayoutRef(parentNode);
      assert(parentNode->isHorizontal());
      // Add the Left parenthesis
      int idxInParent = parentNode->indexOfChild(this);
      int leftParenthesisIndex = idxInParent;
      LeftParenthesisLayoutRef leftParenthesis = LeftParenthesisLayoutRef();
      int numberOfOpenParenthesis = 0;
      while (leftParenthesisIndex > 0
          && parentNode->childAtIndex(leftParenthesisIndex-1)->isCollapsable(&numberOfOpenParenthesis, true))
      {
        leftParenthesisIndex--;
      }
      parentRef.addChildAtIndex(leftParenthesis, leftParenthesisIndex, nullptr);
      idxInParent++;

      // Add the Right parenthesis
      RightParenthesisLayoutRef rightParenthesis = RightParenthesisLayoutRef();
      if (cursor->position() == LayoutCursor::Position::Right) {
         parentRef.addChildAtIndex(rightParenthesis, idxInParent + 1, nullptr);
      } else {
        assert(cursor->position() == LayoutCursor::Position::Left);
         parentRef.addChildAtIndex(rightParenthesis, idxInParent, nullptr);
      }
      if (rightParenthesis.parent().isDefined()) {
        cursor->setLayoutReference(rightParenthesis);
      }
    }
  }
  return true;
}

LayoutNode * VerticalOffsetLayoutNode::baseLayout() {
  LayoutNode * parentNode = parent();
  assert(parentNode != nullptr);
  int idxInParent = parentNode->indexOfChild(this);
  assert(idxInParent > 0);
  return parentNode->childAtIndex(idxInParent - 1);
}

}
