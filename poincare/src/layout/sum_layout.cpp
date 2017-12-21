#include "sum_layout.h"
#include "horizontal_layout.h"
#include "uneditable_parenthesis_left_layout.h"
#include "uneditable_parenthesis_right_layout.h"
#include <poincare/expression_layout_cursor.h>
#include <string.h>
#include <assert.h>

namespace Poincare {

const uint8_t symbolPixel[SumLayout::k_symbolHeight][SumLayout::k_symbolWidth] = {
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
  {0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
  {0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
  {0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
  {0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF},
  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF},
  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF},
  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF},
  {0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF},
  {0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
  {0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
  {0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
  {0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
};

SumLayout::SumLayout(ExpressionLayout * lowerBound, ExpressionLayout * upperBound, ExpressionLayout * argument, bool cloneOperands) :
  SequenceLayout(lowerBound, upperBound, argument, cloneOperands)
{
  UneditableParenthesisLeftLayout * parLeft = new UneditableParenthesisLeftLayout();
  UneditableParenthesisRightLayout * parRight = new UneditableParenthesisRightLayout();
  HorizontalLayout * horLayout = new HorizontalLayout();
  ExpressionLayout * argLayout = editableChild(2);
  // We cannot call argument() because it is overrided to handle completely
  // built SumLayouts, not SumLayouts in construction.
  argLayout->replaceWith(horLayout, false);
  horLayout->addChildAtIndex(parLeft, 0);
  horLayout->addChildAtIndex(argLayout, 1);
  horLayout->addChildAtIndex(parRight, 2);
}

ExpressionLayout * SumLayout::clone() const {
  SumLayout * layout = new SumLayout(const_cast<SumLayout *>(this)->lowerBoundLayout(), const_cast<SumLayout *>(this)->upperBoundLayout(), const_cast<SumLayout *>(this)->argumentLayout(), true);
  return layout;
}

ExpressionLayout * SumLayout::argumentLayout() {
  return editableChild(2)->editableChild(1);
}

bool SumLayout::moveLeft(ExpressionLayoutCursor * cursor) {
  // Case: Left of the bounds.
  // Go Left of the sum.
  if (cursor->position() == ExpressionLayoutCursor::Position::Left
      && ((lowerBoundLayout()
          && cursor->pointedExpressionLayout() == lowerBoundLayout())
        || (upperBoundLayout()
          && cursor->pointedExpressionLayout() == upperBoundLayout())))
  {
    cursor->setPointedExpressionLayout(this);
    return true;
  }
  // Case: Left of the argument.
  // Go Right of the lower bound.
  if (cursor->position() == ExpressionLayoutCursor::Position::Left
      && argumentLayout()
      && cursor->pointedExpressionLayout() == editableChild(2))
  {
    assert(lowerBoundLayout() != nullptr);
    cursor->setPointedExpressionLayout(lowerBoundLayout()->editableChild(1));
    cursor->setPosition(ExpressionLayoutCursor::Position::Right);
    return true;
  }
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Right.
  // Go to the argument and move Left.
  if (cursor->position() == ExpressionLayoutCursor::Position::Right) {
    assert(argumentLayout() != nullptr);
    cursor->setPointedExpressionLayout(argumentLayout());
    return true;
  }
  assert(cursor->position() == ExpressionLayoutCursor::Position::Left);
  // Case: Left.
  // Ask the parent.
  if (m_parent) {
    return m_parent->moveLeft(cursor);
  }
  return false;
}

bool SumLayout::moveRight(ExpressionLayoutCursor * cursor) {
  // Case: Right of the bounds.
  // Go Left of the argument.
  if (cursor->position() == ExpressionLayoutCursor::Position::Right
      && ((lowerBoundLayout()
          && cursor->pointedExpressionLayout() == lowerBoundLayout())
        || (upperBoundLayout()
          && cursor->pointedExpressionLayout() == upperBoundLayout())))
  {
    assert(argumentLayout() != nullptr);
    cursor->setPointedExpressionLayout(argumentLayout());
    cursor->setPosition(ExpressionLayoutCursor::Position::Left);
    return true;
  }
  // Case: Right of the argument.
  // Ask the parent.
  if (cursor->position() == ExpressionLayoutCursor::Position::Right
      && argumentLayout()
      && cursor->pointedExpressionLayout() == editableChild(2))
  {
    cursor->setPointedExpressionLayout(this);
    cursor->setPosition(ExpressionLayoutCursor::Position::Right);
    return true;
  }
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Left.
  // Go to the upper bound.
  if (cursor->position() == ExpressionLayoutCursor::Position::Left) {
    assert(upperBoundLayout() != nullptr);
    cursor->setPointedExpressionLayout(upperBoundLayout());
    return true;
  }
  assert(cursor->position() == ExpressionLayoutCursor::Position::Right);
  // Case: Right.
  // Ask the parent.
  if (m_parent) {
    return m_parent->moveRight(cursor);
  }
  return false;
}

void SumLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  KDSize upperBoundSize = upperBoundLayout()->size();
  KDSize lowerBoundSize = lowerBoundLayout()->size();
  KDColor workingBuffer[k_symbolWidth*k_symbolHeight];
  KDRect symbolFrame(p.x() + max(max(0, (upperBoundSize.width()-k_symbolWidth)/2), (lowerBoundSize.width()-k_symbolWidth)/2),
      p.y() + max(upperBoundSize.height()+k_boundHeightMargin, argumentLayout()->baseline()-(k_symbolHeight+1)/2),
      k_symbolWidth, k_symbolHeight);
  ctx->blendRectWithMask(symbolFrame, expressionColor, (const uint8_t *)symbolPixel, (KDColor *)workingBuffer);
}

KDSize SumLayout::computeSize() {
  KDSize argumentSize = editableChild(2)->size();
  KDSize lowerBoundSize = lowerBoundLayout()->size();
  KDSize upperBoundSize = upperBoundLayout()->size();
  return KDSize(
    max(max(k_symbolWidth, lowerBoundSize.width()), upperBoundSize.width())+k_argumentWidthMargin+argumentSize.width(),
    m_baseline + max(k_symbolHeight/2+k_boundHeightMargin+lowerBoundSize.height(), argumentSize.height() - argumentLayout()->baseline())
  );
}

KDPoint SumLayout::positionOfChild(ExpressionLayout * child) {
  KDSize lowerBoundSize = lowerBoundLayout()->size();
  KDSize upperBoundSize = upperBoundLayout()->size();
  KDCoordinate x = 0;
  KDCoordinate y = 0;
  if (child == lowerBoundLayout()) {
    x = max(max(0, (k_symbolWidth-lowerBoundSize.width())/2), (upperBoundSize.width()-lowerBoundSize.width())/2);
    y = m_baseline + k_symbolHeight/2 + k_boundHeightMargin;
  } else if (child == upperBoundLayout()) {
    x = max(max(0, (k_symbolWidth-upperBoundSize.width())/2), (lowerBoundSize.width()-upperBoundSize.width())/2);
    y = m_baseline - (k_symbolHeight+1)/2- k_boundHeightMargin-upperBoundSize.height();
  } else if (child == editableChild(2)) {
    x = max(max(k_symbolWidth, lowerBoundSize.width()), upperBoundSize.width())+k_argumentWidthMargin;
    y = m_baseline - argumentLayout()->baseline();
  } else {
    assert(false);
  }
  return KDPoint(x,y);
}

}
