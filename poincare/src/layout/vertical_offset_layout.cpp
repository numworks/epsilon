#include "vertical_offset_layout.h"
#include "empty_layout.h"
#include "left_parenthesis_layout.h"
#include "right_parenthesis_layout.h"
#include <ion/charset.h>
#include <poincare/expression_layout_cursor.h>
#include <poincare/layout_engine.h>
#include <string.h>
#include <assert.h>

namespace Poincare {

VerticalOffsetLayout::VerticalOffsetLayout(ExpressionLayout * indice, Type type, bool cloneOperands) :
  StaticLayoutHierarchy(indice, cloneOperands),
  m_type(type)
{
}

ExpressionLayout * VerticalOffsetLayout::clone() const {
  VerticalOffsetLayout * layout = new VerticalOffsetLayout(const_cast<VerticalOffsetLayout *>(this)->indiceLayout(), m_type, true);
  return layout;
}

void VerticalOffsetLayout::deleteBeforeCursor(ExpressionLayoutCursor * cursor) {
  if (cursor->pointedExpressionLayout() == indiceLayout()) {
    assert(cursor->position() == ExpressionLayoutCursor::Position::Left);
    ExpressionLayout * base = baseLayout();
    if (indiceLayout()->isEmpty()) {
      int indexInParent = m_parent->indexOfChild(this);
      if (base->isEmpty()) {
        // Case: Empty base and indice.
        // Remove both the base and the indice layout.
        ExpressionLayout * parent = m_parent;
        cursor->setPointedExpressionLayout(this);
        cursor->setPosition(ExpressionLayoutCursor::Position::Right);
        parent->removePointedChildAtIndexAndMoveCursor(indexInParent, true, cursor);
        cursor->setPointedExpressionLayout(parent->editableChild(indexInParent-1));
        cursor->setPosition(ExpressionLayoutCursor::Position::Right);
        parent->removePointedChildAtIndexAndMoveCursor(indexInParent-1, true, cursor);
        return;
      }
      // Case: Empty indice only.
      // Delete the layout.
      cursor->setPointedExpressionLayout(base);
      cursor->setPosition(ExpressionLayoutCursor::Position::Right);
      m_parent->removeChildAtIndex(indexInParent, true);
      return;
    }
    // Case: Non-empty indice.
    // Move Left of the VerticalOffsetLayout.
    cursor->setPointedExpressionLayout(this);
    cursor->setPosition(ExpressionLayoutCursor::Position::Left);
    return;
  }
  if (cursor->pointedExpressionLayout() == this
      && cursor->position() == ExpressionLayoutCursor::Position::Right)
  {
    // Case: Right.
    // Move to the indice.
    cursor->setPointedExpressionLayout(indiceLayout());
    return;
  }
  ExpressionLayout::deleteBeforeCursor(cursor);
}

ExpressionLayoutCursor VerticalOffsetLayout::cursorLeftOf(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout) {
  // Case: Left of the indice. Go Left.
  if (indiceLayout()
      && cursor.pointedExpressionLayout() == indiceLayout()
      && cursor.position() == ExpressionLayoutCursor::Position::Left)
  {
    return ExpressionLayoutCursor(this, ExpressionLayoutCursor::Position::Left);
  }

  assert(cursor.pointedExpressionLayout() == this);
  // Case: Right. Go to the indice.
  if (cursor.position() == ExpressionLayoutCursor::Position::Right) {
    assert(indiceLayout() != nullptr);
    return ExpressionLayoutCursor(indiceLayout(), ExpressionLayoutCursor::Position::Right);
  }
  // Case: Left. Ask the parent.
  assert(cursor.position() == ExpressionLayoutCursor::Position::Left);
  if (m_parent) {
    return m_parent->cursorLeftOf(cursor, shouldRecomputeLayout);
  }
  return ExpressionLayoutCursor();
}

ExpressionLayoutCursor VerticalOffsetLayout::cursorRightOf(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout) {
  // Case: Right of the indice. Go Right.
  if (indiceLayout()
      && cursor.pointedExpressionLayout() == indiceLayout()
      && cursor.position() == ExpressionLayoutCursor::Position::Right)
  {
    return ExpressionLayoutCursor(this, ExpressionLayoutCursor::Position::Right);
  }

  assert(cursor.pointedExpressionLayout() == this);
  // Case: Left. Go to the indice.
  if (cursor.position() == ExpressionLayoutCursor::Position::Left) {
    assert(indiceLayout() != nullptr);
    return ExpressionLayoutCursor(indiceLayout(), ExpressionLayoutCursor::Position::Left);
  }
  // Case: Right. Ask the parent.
  assert(cursor.position() == ExpressionLayoutCursor::Position::Right);
  if (m_parent) {
    return m_parent->cursorRightOf(cursor, shouldRecomputeLayout);
  }
  return ExpressionLayoutCursor();
}

ExpressionLayoutCursor VerticalOffsetLayout::cursorAbove(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  // Case: Superscript.
  if (m_type == VerticalOffsetLayout::Type::Superscript) {
    // Case: Right. Move to the indice.
    if (cursor.isEquivalentTo(ExpressionLayoutCursor(this, ExpressionLayoutCursor::Position::Right))) {
      assert(indiceLayout() != nullptr);
      return ExpressionLayoutCursor(indiceLayout(), ExpressionLayoutCursor::Position::Right);
    }
    // Case: Left. Move to the indice.
    if (cursor.isEquivalentTo(ExpressionLayoutCursor(this, ExpressionLayoutCursor::Position::Left))) {
      assert(indiceLayout() != nullptr);
      return ExpressionLayoutCursor(indiceLayout(), ExpressionLayoutCursor::Position::Left);
    }
  }
  /* Case: Subscript, Left or Right of the indice. Put the cursor at the same
   * position, pointing this. */
  if (m_type == VerticalOffsetLayout::Type::Subscript
    && indiceLayout() != nullptr
    && (cursor.isEquivalentTo(ExpressionLayoutCursor(indiceLayout(), ExpressionLayoutCursor::Position::Left))
      || cursor.isEquivalentTo(ExpressionLayoutCursor(indiceLayout(), ExpressionLayoutCursor::Position::Right))))
  {
      return ExpressionLayoutCursor(this, cursor.position());
  }
  return ExpressionLayout::cursorAbove(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

ExpressionLayoutCursor VerticalOffsetLayout::cursorUnder(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  // Case: Subscript.
  if (m_type == VerticalOffsetLayout::Type::Subscript) {
    // Case: Right. Move to the indice.
    if (cursor.isEquivalentTo(ExpressionLayoutCursor(this, ExpressionLayoutCursor::Position::Right))) {
      assert(indiceLayout() != nullptr);
      return ExpressionLayoutCursor(indiceLayout(), ExpressionLayoutCursor::Position::Right);
    }
    // Case: Left. Move to the indice.
    if (cursor.isEquivalentTo(ExpressionLayoutCursor(this, ExpressionLayoutCursor::Position::Left))) {
      assert(indiceLayout() != nullptr);
      return ExpressionLayoutCursor(indiceLayout(), ExpressionLayoutCursor::Position::Left);
    }
  }
  /* Case: Superscript, Left or Right of the indice. Put the cursor at the same
   * position, pointing this. */
  if (m_type == VerticalOffsetLayout::Type::Superscript
    && indiceLayout() != nullptr
    && cursor.pointedExpressionLayout() == indiceLayout())
  {
    return ExpressionLayoutCursor(this, cursor.position());
  }
  return ExpressionLayout::cursorUnder(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

int VerticalOffsetLayout::writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits) const {
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

    numberOfChar += const_cast<VerticalOffsetLayout *>(this)->indiceLayout()->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar, numberOfSignificantDigits);
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

    numberOfChar += LayoutEngine::writeOneCharInBuffer(buffer+numberOfChar, bufferSize-numberOfChar, '}');
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

    return numberOfChar;
  }
  assert(m_type == Type::Superscript);
  // If the layout is a superscript, write "^(indice)"
  int numberOfChar = LayoutEngine::writePrefixExpressionLayoutTextInBuffer(this, buffer, bufferSize, numberOfSignificantDigits, "^");

  // Add a multiplication if omitted.
  int indexInParent = -1;
  if (m_parent) {
    indexInParent = m_parent->indexOfChild(this);
  }
  if (indexInParent >= 0 && indexInParent < (m_parent->numberOfChildren() - 1) && m_parent->isHorizontal() && m_parent->child(indexInParent + 1)->canBeOmittedMultiplicationRightFactor()) {
    buffer[numberOfChar++] = Ion::Charset::MiddleDot;
    if (numberOfChar >= bufferSize-1) { return bufferSize-1;}
  }
  buffer[numberOfChar] = 0;
  return numberOfChar;
}

ExpressionLayout * VerticalOffsetLayout::baseLayout() {
  int indexInParent = parent()->indexOfChild(this);
  assert(indexInParent > 0);
  return editableParent()->editableChild(indexInParent - 1);
}

KDSize VerticalOffsetLayout::computeSize() {
  KDSize indiceSize = indiceLayout()->size();
  KDCoordinate width = indiceSize.width();
  if (m_type == Type::Superscript) {
    assert(m_parent != nullptr);
    assert(m_parent->isHorizontal());
    int indexInParent = m_parent->indexOfChild(this);
    if (indexInParent < m_parent-> numberOfChildren() - 1 && m_parent->editableChild(indexInParent + 1)->hasUpperLeftIndex()) {
      width += k_separationMargin;
    }
  }
  KDCoordinate height = baseLayout()->size().height() - k_indiceHeight + indiceLayout()->size().height();
  return KDSize(width, height);
}

void VerticalOffsetLayout::computeBaseline() {
  if (m_type == Type::Subscript) {
    m_baseline = baseLayout()->baseline();
  } else {
    m_baseline = indiceLayout()->size().height() - k_indiceHeight + baseLayout()->baseline();
  }
  m_baselined = true;
}

KDPoint VerticalOffsetLayout::positionOfChild(ExpressionLayout * child) {
  assert(child == indiceLayout());
  if (m_type == Type::Superscript) {
    return KDPointZero;
  }
  assert(m_type == Type::Subscript);
  ExpressionLayout * base = baseLayout();
  return KDPoint(0, base->size().height() - k_indiceHeight);
}

void VerticalOffsetLayout::privateAddSibling(ExpressionLayoutCursor * cursor, ExpressionLayout * sibling, bool moveCursor) {
  if (sibling->isVerticalOffset()) {
    VerticalOffsetLayout * verticalOffsetSibling = static_cast<VerticalOffsetLayout *>(sibling);
    if (verticalOffsetSibling->type() == Type::Superscript) {
      assert(m_parent->isHorizontal());
      // Add the Left parenthesis
      int indexInParent = m_parent->indexOfChild(this);
      int leftParenthesisIndex = indexInParent;
      LeftParenthesisLayout * leftParenthesis = new LeftParenthesisLayout();
      int numberOfOpenParenthesis = 0;
      while (leftParenthesisIndex > 0
          && editableParent()->editableChild(leftParenthesisIndex-1)->isCollapsable(&numberOfOpenParenthesis, true))
      {
        leftParenthesisIndex--;
      }
      m_parent->addChildAtIndex(leftParenthesis, leftParenthesisIndex);
      indexInParent = m_parent->indexOfChild(this);

      // Add the Right parenthesis
      RightParenthesisLayout * rightParenthesis = new RightParenthesisLayout();
      if (cursor->position() == ExpressionLayoutCursor::Position::Right) {
        m_parent->addChildAtIndex(rightParenthesis, indexInParent + 1);
      } else {
        assert(cursor->position() == ExpressionLayoutCursor::Position::Left);
        m_parent->addChildAtIndex(rightParenthesis, indexInParent);
      }
      cursor->setPointedExpressionLayout(rightParenthesis);
      if (moveCursor) {
        rightParenthesis->addSiblingAndMoveCursor(cursor, sibling);
      } else {
        rightParenthesis->addSibling(cursor, sibling);
      }
      return;
    }
  }
  ExpressionLayout::privateAddSibling(cursor, sibling, moveCursor);
}


}
