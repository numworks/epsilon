#include <poincare/vector_layout.h>

namespace Poincare
{
  const uint8_t arrowMask[VectorLayoutNode::k_arrowHeight][VectorLayoutNode::k_arrowWidth] = {
    {0xff, 0xf7, 0xff, 0xff, 0xff},
    {0xf3, 0x2c, 0xd9, 0xff, 0xff},
    {0xff, 0x93, 0x46, 0xfb, 0xff},
    {0xff, 0xfb, 0x46, 0x93, 0xff},
    {0x13, 0x13, 0x13, 0x13, 0xf0},
    {0xff, 0xfb, 0x46, 0x93, 0xff},
    {0xff, 0x93, 0x46, 0xfb, 0xff},
    {0xf3, 0x2c, 0xd9, 0xff, 0xff},
    {0xff, 0xf7, 0xff, 0xff, 0xff}
  };

  void VectorLayoutNode::moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
    if (cursor->layoutNode() == childAtIndex(0)
      && cursor->position() == LayoutCursor::Position::Left)
    {
      // Case: Left of the operand. Go Left of the brackets.
      cursor->setLayout(this);
      return;
    }
    assert(cursor->layoutNode() == this);
    if (cursor->position() == LayoutCursor::Position::Right) {
      // Case: Right of the brackets. Go Right of the operand.
      cursor->setLayout(childAtIndex(0));
      return;
    }
    assert(cursor->position() == LayoutCursor::Position::Left);
    // Case: Left of the brackets. Ask the parent.
    LayoutNode * parentNode = parent();
    if (parentNode != nullptr) {
      parentNode->moveCursorLeft(cursor, shouldRecomputeLayout);
    }
  }

  void VectorLayoutNode::moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
    if (cursor->layoutNode() == childAtIndex(0)
      && cursor->position() == LayoutCursor::Position::Right)
    {
      // Case: Right of the operand. Go Right of the brackets.
      cursor->setLayout(this);
      return;
    }
    assert(cursor->layoutNode() == this);
    if (cursor->position() == LayoutCursor::Position::Left) {
      // Case: Left of the brackets. Go Left of the operand.
      cursor->setLayout(childAtIndex(0));
      return;
    }
    assert(cursor->position() == LayoutCursor::Position::Right);
    // Case: Right of the brackets. Ask the parent.
    LayoutNode * parentNode = parent();
    if (parentNode != nullptr) {
      parentNode->moveCursorRight(cursor, shouldRecomputeLayout);
    }
  }
  KDSize VectorLayoutNode::computeSize() {
    KDSize size = childAtIndex(0)->layoutSize();
    return KDSize(2 * k_sideMargin + size.width() + k_arrowWidth + k_sideMargin, k_topMargin + (k_arrowHeight+k_arrowLineHeight)/2 + size.height());
  }

  KDCoordinate VectorLayoutNode::computeBaseline() {
    return childAtIndex(0)->baseline() + (k_arrowHeight+k_arrowLineHeight)/2 + k_arrowLineHeight + k_topMargin;
  }

  KDPoint VectorLayoutNode::positionOfChild(LayoutNode * child) {
    assert(child == childAtIndex(0));
    return KDPoint(k_sideMargin * 2, k_topMargin + (k_arrowHeight+k_arrowLineHeight)/2 + k_arrowLineHeight);
  }
  void VectorLayoutNode::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart, Layout * selectionEnd, KDColor selectionColor) {
    KDColor workingBuffer[k_arrowWidth * k_arrowHeight];
    ctx->fillRect(KDRect(p.x() + k_sideMargin, p.y() + k_topMargin + (k_arrowHeight-k_arrowLineHeight)/2, 2 * k_sideMargin + childAtIndex(0)->layoutSize().width(), k_arrowLineHeight), expressionColor);
    ctx->blendRectWithMask(KDRect(p.x() + 2 * k_sideMargin + childAtIndex(0)->layoutSize().width(), p.y() + k_topMargin, k_arrowWidth, k_arrowHeight), expressionColor, (const uint8_t *)arrowMask, workingBuffer);
  }
  
} 
