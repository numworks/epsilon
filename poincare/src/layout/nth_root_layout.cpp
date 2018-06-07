#include "nth_root_layout.h"
#include "horizontal_layout.h"
#include <ion/charset.h>
#include <poincare/expression_layout_cursor.h>
#include <string.h>
#include <assert.h>

namespace Poincare {

static inline uint16_t max(uint16_t x, uint16_t y) { return (x>y ? x : y); }

const uint8_t radixPixel[NthRootLayout::k_leftRadixHeight][NthRootLayout::k_leftRadixWidth] = {
  {0x00, 0xFF, 0xFF, 0xFF, 0xFF},
  {0xFF, 0x00, 0xFF, 0xFF, 0xFF},
  {0xFF, 0x00, 0xFF, 0xFF, 0xFF},
  {0xFF, 0xFF, 0x00, 0xFF, 0xFF},
  {0xFF, 0xFF, 0x00, 0xFF, 0xFF},
  {0xFF, 0xFF, 0xFF, 0x00, 0xFF},
  {0xFF, 0xFF, 0xFF, 0x00, 0xFF},
  {0xFF, 0xFF, 0xFF, 0xFF, 0x00},
};

ExpressionLayout * NthRootLayout::clone() const {
  if (numberOfChildren() == 1) {
    return new NthRootLayout(const_cast<NthRootLayout *>(this)->radicandLayout(), true);
  }
  assert(numberOfChildren() == 2);
  return new NthRootLayout(const_cast<NthRootLayout *>(this)->radicandLayout(), const_cast<NthRootLayout *>(this)->indexLayout(), true);
}

void NthRootLayout::collapseSiblingsAndMoveCursor(ExpressionLayoutCursor * cursor) {
  // If the radicand layout is not an HorizontalLayout, replace it with one.
  if (!radicandLayout()->isHorizontal()) {
    ExpressionLayout * previousRadicand = radicandLayout();
    HorizontalLayout * horizontalRadicandLayout = new HorizontalLayout(previousRadicand, false);
    replaceChild(previousRadicand, horizontalRadicandLayout, false);
  }
  ExpressionLayout::collapseOnDirection(HorizontalDirection::Right, 0);
  cursor->setPointedExpressionLayout(radicandLayout());
  cursor->setPosition(ExpressionLayoutCursor::Position::Left);
}

void NthRootLayout::deleteBeforeCursor(ExpressionLayoutCursor * cursor) {
  if (cursor->pointedExpressionLayout() == radicandLayout()
      && cursor->position() == ExpressionLayoutCursor::Position::Left)
  {
    // Case: Left of the radicand. Delete the layout, keep the radicand.
    replaceWithAndMoveCursor(radicandLayout(), true, cursor);
    return;
  }
  ExpressionLayout::deleteBeforeCursor(cursor);
}

ExpressionLayoutCursor NthRootLayout::cursorLeftOf(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout) {
  // Case: Left of the radicand. Go the index if any, else go Left of the root.
  if (radicandLayout()
    && cursor.pointedExpressionLayout() == radicandLayout()
    && cursor.position() == ExpressionLayoutCursor::Position::Left)
  {
    if (indexLayout()) {
      return ExpressionLayoutCursor(indexLayout(), ExpressionLayoutCursor::Position::Right);
    }
    return ExpressionLayoutCursor(this, ExpressionLayoutCursor::Position::Left);
  }
  // Case: Left of the index. Go Left of the root.
  if (indexLayout()
    && cursor.pointedExpressionLayout() == indexLayout()
    && cursor.position() == ExpressionLayoutCursor::Position::Left)
  {
    return ExpressionLayoutCursor(this, ExpressionLayoutCursor::Position::Left);
  }
  assert(cursor.pointedExpressionLayout() == this);
  // Case: Right. Go Right of the radicand.
  if (cursor.position() == ExpressionLayoutCursor::Position::Right) {
    assert(radicandLayout() != nullptr);
    return ExpressionLayoutCursor(radicandLayout(), ExpressionLayoutCursor::Position::Right);
  }
  assert(cursor.position() == ExpressionLayoutCursor::Position::Left);
  // Case: Left. Ask the parent.
  if (m_parent) {
    return m_parent->cursorLeftOf(cursor, shouldRecomputeLayout);
  }
  return ExpressionLayoutCursor();
}

ExpressionLayoutCursor NthRootLayout::cursorRightOf(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout) {
  // Case: Right of the radicand. Go the Right of the root.
  if (radicandLayout()
    && cursor.pointedExpressionLayout() == radicandLayout()
    && cursor.position() == ExpressionLayoutCursor::Position::Right)
  {
    return ExpressionLayoutCursor(this, ExpressionLayoutCursor::Position::Right);
  }
  // Case: Right of the index. Go Left of the integrand.
  if (indexLayout()
    && cursor.pointedExpressionLayout() == indexLayout()
    && cursor.position() == ExpressionLayoutCursor::Position::Right)
  {
    assert(radicandLayout() != nullptr);
    return ExpressionLayoutCursor(radicandLayout(), ExpressionLayoutCursor::Position::Left);
  }
  assert(cursor.pointedExpressionLayout() == this);
  // Case: Left. Go index if there is one, else go to the radicand.
  if (cursor.position() == ExpressionLayoutCursor::Position::Left) {
    if (indexLayout()) {
      return ExpressionLayoutCursor(indexLayout(), ExpressionLayoutCursor::Position::Left);
    }
    assert(radicandLayout() != nullptr);
    return ExpressionLayoutCursor(radicandLayout(), ExpressionLayoutCursor::Position::Left);
  }
  assert(cursor.position() == ExpressionLayoutCursor::Position::Right);
  // Case: Right. Ask the parent.
  if (m_parent) {
    return m_parent->cursorRightOf(cursor, shouldRecomputeLayout);
  }
  return ExpressionLayoutCursor();
}

ExpressionLayoutCursor NthRootLayout::cursorAbove(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  // If the cursor is Left of the radicand, move it to the index.
  if (indexLayout()
      && radicandLayout()
      && cursor.isEquivalentTo(ExpressionLayoutCursor(radicandLayout(), ExpressionLayoutCursor::Position::Left)))
  {
    return ExpressionLayoutCursor(indexLayout(), ExpressionLayoutCursor::Position::Right);
  }
  // If the cursor is Left, move it to the index.
  if (indexLayout()
      && cursor.pointedExpressionLayout() == this
      && cursor.position() == ExpressionLayoutCursor::Position::Left)
  {
    return ExpressionLayoutCursor(indexLayout(), ExpressionLayoutCursor::Position::Left);
  }
  return ExpressionLayout::cursorAbove(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

ExpressionLayoutCursor NthRootLayout::cursorUnder(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  if (indexLayout() && cursor.pointedExpressionLayout()->hasAncestor(indexLayout(), true)) {
    // If the cursor is Right of the index, move it to the radicand.
    if (cursor.isEquivalentTo(ExpressionLayoutCursor(indexLayout(), ExpressionLayoutCursor::Position::Right))) {
      assert(radicandLayout() != nullptr);
      return ExpressionLayoutCursor(radicandLayout(), ExpressionLayoutCursor::Position::Left);
    }
    // If the cursor is Left of the index, move it Left .
    if (cursor.isEquivalentTo(ExpressionLayoutCursor(indexLayout(), ExpressionLayoutCursor::Position::Left))) {
      return ExpressionLayoutCursor(this, ExpressionLayoutCursor::Position::Left);
    }
  }
  return ExpressionLayout::cursorUnder(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

static_assert('\x90' == Ion::Charset::Root, "Unicode error");
int NthRootLayout::writeTextInBuffer(char * buffer, int bufferSize) const {
  // Case: root(x,n)
  if (numberOfChildren() == 2
      && (const_cast<NthRootLayout *>(this))->indexLayout()
      && !(const_cast<NthRootLayout *>(this))->indexLayout()->isEmpty())
  {
    return LayoutEngine::writePrefixExpressionLayoutTextInBuffer(this, buffer, bufferSize, "root");
  }
  // Case: squareRoot(x)
  if (numberOfChildren() == 1) {
    return LayoutEngine::writePrefixExpressionLayoutTextInBuffer(this, buffer, bufferSize, "\x90");
  }
  // Case: root(x,empty)
  // Write "'SquareRootSymbol'('radicandLayout')".
  assert((const_cast<NthRootLayout *>(this))->indexLayout() && (const_cast<NthRootLayout *>(this))->indexLayout()->isEmpty());
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;
  int numberOfChar = 0;

  buffer[numberOfChar++] = '\x90';
  if (numberOfChar >= bufferSize-1) {
    return bufferSize-1;
  }

  buffer[numberOfChar++] = '(';
  if (numberOfChar >= bufferSize-1) {
    return bufferSize-1;
  }

  numberOfChar += (const_cast<NthRootLayout *>(this))->radicandLayout()->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar);
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

  buffer[numberOfChar++] = ')';
  buffer[numberOfChar] = 0;
  return numberOfChar;
}

void NthRootLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  KDSize radicandSize = radicandLayout()->size();
  KDSize indexSize = adjustedIndexSize();
  KDColor workingBuffer[k_leftRadixWidth*k_leftRadixHeight];
  KDRect leftRadixFrame(p.x() + indexSize.width() + k_widthMargin - k_leftRadixWidth,
    p.y() + baseline() + radicandSize.height() - radicandLayout()->baseline() + k_heightMargin - k_leftRadixHeight,
    k_leftRadixWidth, k_leftRadixHeight);
  ctx->blendRectWithMask(leftRadixFrame, expressionColor, (const uint8_t *)radixPixel, (KDColor *)workingBuffer);
  // If the indice is higher than the root.
  if (indexSize.height() + k_indexHeight > radicandLayout()->baseline() + k_radixLineThickness + k_heightMargin) {
    // Vertical radix bar
    ctx->fillRect(KDRect(p.x() + indexSize.width() + k_widthMargin,
                         p.y() + indexSize.height() + k_indexHeight - radicandLayout()->baseline() - k_radixLineThickness - k_heightMargin,
                         k_radixLineThickness,
                         radicandSize.height() + 2*k_heightMargin + k_radixLineThickness), expressionColor);
    // Horizontal radix bar
    ctx->fillRect(KDRect(p.x() + indexSize.width() + k_widthMargin,
                         p.y() + indexSize.height() + k_indexHeight - radicandLayout()->baseline() - k_radixLineThickness - k_heightMargin,
                         radicandSize.width() + 2*k_widthMargin + k_radixHorizontalOverflow,
                         k_radixLineThickness), expressionColor);
    // Right radix bar
    ctx->fillRect(KDRect(p.x() + indexSize.width() + k_widthMargin + radicandSize.width() + 2*k_widthMargin + k_radixHorizontalOverflow,
                         p.y() + indexSize.height() + k_indexHeight - radicandLayout()->baseline() - k_radixLineThickness - k_heightMargin,
                         k_radixLineThickness,
                         k_rightRadixHeight + k_radixLineThickness), expressionColor);
  } else {
    ctx->fillRect(KDRect(p.x() + indexSize.width() + k_widthMargin,
                         p.y(),
                         k_radixLineThickness,
                         radicandSize.height() + 2*k_heightMargin + k_radixLineThickness), expressionColor);
    ctx->fillRect(KDRect(p.x() + indexSize.width() + k_widthMargin,
                         p.y(),
                         radicandSize.width() + 2*k_widthMargin + k_radixHorizontalOverflow,
                         k_radixLineThickness), expressionColor);
    ctx->fillRect(KDRect(p.x() + indexSize.width() + k_widthMargin + radicandSize.width() + 2*k_widthMargin + k_radixHorizontalOverflow,
                         p.y(),
                         k_radixLineThickness,
                         k_rightRadixHeight + k_radixLineThickness), expressionColor);
  }

}

KDSize NthRootLayout::computeSize() {
  KDSize radicandSize = radicandLayout()->size();
  KDSize indexSize = adjustedIndexSize();
  return KDSize(
      indexSize.width() + 3*k_widthMargin + 2*k_radixLineThickness + radicandSize.width() + k_radixHorizontalOverflow,
      baseline() + radicandSize.height() - radicandLayout()->baseline() + k_heightMargin
      );
}

void NthRootLayout::computeBaseline() {
  if (indexLayout() != nullptr) {
    m_baseline = max(radicandLayout()->baseline() + k_radixLineThickness + k_heightMargin,
      indexLayout()->size().height() + k_indexHeight);
  } else {
    m_baseline = radicandLayout()->baseline() + k_radixLineThickness + k_heightMargin;
  }
  m_baselined = true;
}

KDPoint NthRootLayout::positionOfChild(ExpressionLayout * child) {
  KDCoordinate x = 0;
  KDCoordinate y = 0;
  KDSize indexSize = adjustedIndexSize();
  if (child == radicandLayout()) {
    x = indexSize.width() + 2*k_widthMargin + k_radixLineThickness;
    y = baseline() - radicandLayout()->baseline();
  } else if (indexLayout() && child == indexLayout()) {
    x = 0;
    y = baseline() - indexSize.height() -  k_indexHeight;
  } else {
    assert(false);
  }
  return KDPoint(x,y);
}

KDSize NthRootLayout::adjustedIndexSize() {
  return indexLayout() != nullptr ? KDSize(max(k_leftRadixWidth, indexLayout()->size().width()), indexLayout()->size().height()) : KDSize(k_leftRadixWidth,0);
}

}
