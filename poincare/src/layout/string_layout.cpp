#include "string_layout.h"
#include <poincare/expression_layout_cursor.h>
#include <assert.h>
#include <stdlib.h>

namespace Poincare {

StringLayout::StringLayout(const char * string, size_t length, KDText::FontSize fontSize) :
  StaticLayoutHierarchy<0>(),
  m_fontSize(fontSize)
{
  m_string = new char[length+1];
  memcpy(m_string, string, length);
  m_string[length] = 0;
}

StringLayout::~StringLayout() {
  delete[] m_string;
}

ExpressionLayout * StringLayout::clone() const {
  StringLayout * layout = new StringLayout(m_string, strlen(m_string), m_fontSize);
  return layout;
}

bool StringLayout::moveLeft(ExpressionLayoutCursor * cursor) {
  // A StringLayout is not editable, and the cursor cannot go inside it.
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Right.
  // If there is a Left brother, go Right of it. Else go Left of the
  // grandparent. We need to do this to avoid adding text left or right of a
  // string layout, for instance left of "n=" in a Sum layout.
  if (cursor->position() == ExpressionLayoutCursor::Position::Right) {
    int indexOfThis = m_parent->indexOfChild(this);
    if (indexOfThis > 1) {
      cursor->setPointedExpressionLayout(m_parent->editableChild(indexOfThis-1));
      cursor->setPosition(ExpressionLayoutCursor::Position::Right);
      return true;
    }
    if (m_parent->parent()) {
      cursor->setPointedExpressionLayout(const_cast<ExpressionLayout *>(m_parent->parent()));
      cursor->setPosition(ExpressionLayoutCursor::Position::Left);
      return true;
    }
  }
  // Case: Left.
  // Ask the parent.
  if (m_parent) {
    return m_parent->moveLeft(cursor);
  }
  return false;
}

bool StringLayout::moveRight(ExpressionLayoutCursor * cursor) {
  // A StringLayout is not editable, and the cursor cannot go inside it.
  assert(cursor->pointedExpressionLayout() == this);
  assert(m_parent != nullptr);
  // Case: Left.
  // If there is a Right brother, go Left of it. Else go Right of the
  // grandparent. We need to do this to avoid adding text left or right of a
  // string layout, for instance right of "dx" in an integral layout.
  if (cursor->position() == ExpressionLayoutCursor::Position::Left) {
    int indexOfThis = m_parent->indexOfChild(this);
    if (m_parent->editableChild(indexOfThis+1) != nullptr) {
      cursor->setPointedExpressionLayout(m_parent->editableChild(indexOfThis+1));
      cursor->setPosition(ExpressionLayoutCursor::Position::Left);
      return true;
    }
    if (m_parent->parent()) {
      cursor->setPointedExpressionLayout(const_cast<ExpressionLayout *>(m_parent->parent()));
      cursor->setPosition(ExpressionLayoutCursor::Position::Right);
      return true;
    }
  }
  // Case: Right.
  // Ask the parent.
  return m_parent->moveRight(cursor);
}

void StringLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  ctx->drawString(m_string, p, m_fontSize, expressionColor, backgroundColor);
}

KDPoint StringLayout::positionOfChild(ExpressionLayout * child) {
  assert(false); // We should never be here
  return KDPointZero;
}

KDSize StringLayout::computeSize() {
  return KDText::stringSize(m_string, m_fontSize);
}

void StringLayout::computeBaseline() {
  // Half height of the font.
  m_baseline = (KDText::charSize(m_fontSize).height()+1)/2;
  m_baselined = true;
}

}
