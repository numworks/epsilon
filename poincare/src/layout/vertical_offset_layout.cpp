#include "vertical_offset_layout.h"
#include "empty_visible_layout.h"
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

void VerticalOffsetLayout::backspaceAtCursor(ExpressionLayoutCursor * cursor) {
  if (cursor->pointedExpressionLayout() == indiceLayout()) {
    assert(cursor->position() == ExpressionLayoutCursor::Position::Left);
    ExpressionLayout * base = baseLayout();
    if (indiceLayout()->isEmpty()) {
      int indexInParent = m_parent->indexOfChild(this);
      if (base->isEmpty()) {
        // Case: Empty base and indice.
        // Replace with the empty base layout.
        if (indexInParent <= 1) {
          cursor->setPointedExpressionLayout(m_parent);
        } else {
          cursor->setPointedExpressionLayout(m_parent->editableChild(indexInParent - 2));
          cursor->setPosition(ExpressionLayoutCursor::Position::Right);
        }
        m_parent->removeChildAtIndex(indexInParent - 1, false);
        replaceWith(base, true);
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
  ExpressionLayout::backspaceAtCursor(cursor);
}

bool VerticalOffsetLayout::moveLeft(ExpressionLayoutCursor * cursor) {
  // Case: Left of the indice.
  // Go Left.
  if (indiceLayout()
      && cursor->pointedExpressionLayout() == indiceLayout()
      && cursor->position() == ExpressionLayoutCursor::Position::Left)
  {
    cursor->setPointedExpressionLayout(this);
    cursor->setPosition(ExpressionLayoutCursor::Position::Left);
    return true;
  }

  assert(cursor->pointedExpressionLayout() == this);
  // Case: Right.
  // Go to the indice.
  if (cursor->position() == ExpressionLayoutCursor::Position::Right) {
    assert(indiceLayout() != nullptr);
    cursor->setPointedExpressionLayout(indiceLayout());
    return true;
  }
  // Case: Left.
  // Ask the parent.
  assert(cursor->position() == ExpressionLayoutCursor::Position::Left);
  if (m_parent) {
    return m_parent->moveLeft(cursor);
  }
  return false;
}

bool VerticalOffsetLayout::moveRight(ExpressionLayoutCursor * cursor) {
  // Case: Right of the indice.
  // Go Right.
  if (indiceLayout()
      && cursor->pointedExpressionLayout() == indiceLayout()
      && cursor->position() == ExpressionLayoutCursor::Position::Right)
  {
    cursor->setPointedExpressionLayout(this);
    return true;
  }

  assert(cursor->pointedExpressionLayout() == this);
  // Case: Left.
  // Go to the indice.
  if (cursor->position() == ExpressionLayoutCursor::Position::Left) {
    assert(indiceLayout() != nullptr);
    cursor->setPointedExpressionLayout(indiceLayout());
    return true;
  }
  // Case: Right.
  // Ask the parent.
  assert(cursor->position() == ExpressionLayoutCursor::Position::Right);
  if (m_parent) {
    return m_parent->moveRight(cursor);
  }
  return false;
}

bool VerticalOffsetLayout::moveUp(ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) {
  // Case: Superscript.
  if (m_type == VerticalOffsetLayout::Type::Superscript) {
    // Case: Right.
    // Move to the indice.
    if (cursor->positionIsEquivalentTo(this, ExpressionLayoutCursor::Position::Right)) {
      assert(indiceLayout() != nullptr);
      cursor->setPointedExpressionLayout(indiceLayout());
      cursor->setPosition(ExpressionLayoutCursor::Position::Right);
      return true;
    }
    // Case: Left.
    // Move to the indice.
    if (cursor->positionIsEquivalentTo(this, ExpressionLayoutCursor::Position::Left)) {
      assert(indiceLayout() != nullptr);
      cursor->setPointedExpressionLayout(indiceLayout());
      cursor->setPosition(ExpressionLayoutCursor::Position::Left);
      return true;
    }
  }
  // Case: Subscript.
  // Case: Left or Right of the indice.
  // Put the cursor at the same position, pointing this.
  if (m_type == VerticalOffsetLayout::Type::Subscript
    && indiceLayout() != nullptr
    && (cursor->positionIsEquivalentTo(indiceLayout(), ExpressionLayoutCursor::Position::Left)
      || cursor->positionIsEquivalentTo(indiceLayout(), ExpressionLayoutCursor::Position::Right)))
  {
    cursor->setPointedExpressionLayout(this);
    return true;
  }
  return ExpressionLayout::moveUp(cursor, previousLayout, previousPreviousLayout);
}

bool VerticalOffsetLayout::moveDown(ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) {
  // Case: Subscript.
  if (m_type == VerticalOffsetLayout::Type::Subscript) {
    // Case: Right.
    // Move to the indice.
    if (cursor->positionIsEquivalentTo(this, ExpressionLayoutCursor::Position::Right)) {
      assert(indiceLayout() != nullptr);
      cursor->setPointedExpressionLayout(indiceLayout());
      cursor->setPosition(ExpressionLayoutCursor::Position::Right);
      return true;
    }
    // Case: Left.
    // Move to the indice.
    if (cursor->positionIsEquivalentTo(this, ExpressionLayoutCursor::Position::Left)) {
      assert(indiceLayout() != nullptr);
      cursor->setPointedExpressionLayout(indiceLayout());
      cursor->setPosition(ExpressionLayoutCursor::Position::Left);
      return true;
    }
  }
  // Case: Superscript.
  // Case: Left or Right of the indice.
  // Put the cursor at the same position, pointing this.
  if (m_type == VerticalOffsetLayout::Type::Superscript
    && indiceLayout() != nullptr
    && cursor->pointedExpressionLayout() == indiceLayout())
  {
    cursor->setPointedExpressionLayout(this);
    return true;
  }
  return ExpressionLayout::moveDown(cursor, previousLayout, previousPreviousLayout);
}

int VerticalOffsetLayout::writeTextInBuffer(char * buffer, int bufferSize) const {
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

    numberOfChar += const_cast<VerticalOffsetLayout *>(this)->indiceLayout()->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar);
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

    numberOfChar += LayoutEngine::writeOneCharInBuffer(buffer+numberOfChar, bufferSize-numberOfChar, '}');
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

    return numberOfChar;
  }
  assert(m_type == Type::Superscript);
  // If the layout is a superscript, write "^(indice)"
  return LayoutEngine::writePrefixExpressionLayoutTextInBuffer(this, buffer, bufferSize, "^");
}

ExpressionLayout * VerticalOffsetLayout::indiceLayout() {
  return editableChild(0);
}

ExpressionLayout * VerticalOffsetLayout::baseLayout() {
  int indexInParent = parent()->indexOfChild(this);
  assert(indexInParent > 0);
  return editableParent()->editableChild(indexInParent - 1);
}

void VerticalOffsetLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  // There is nothing to draw for a subscript/superscript, only the position of the child matters
}

KDSize VerticalOffsetLayout::computeSize() {
  KDSize indiceSize = indiceLayout()->size();
  KDCoordinate width = indiceSize.width();
  KDCoordinate height = 0;
  if (m_type == Type::Subscript) {
    height = positionOfChild(indiceLayout()).y()+ indiceLayout()->size().height();
  } else {
    height = indiceLayout()->size().height() + baseLayout()->baseline() - k_indiceHeight;
  }
  return KDSize(width, height);
}

void VerticalOffsetLayout::computeBaseline() {
  if (m_type == Type::Subscript) {
    m_baseline = 0;
  } else {
    m_baseline = size().height();
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
  return KDPoint(0, base->size().height() - base->baseline() - k_indiceHeight);
}

}
