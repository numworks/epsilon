#include <poincare/nth_root.h>
#include <poincare/nth_root_layout.h>
#include <poincare/square_root.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <assert.h>

namespace Poincare {

static inline KDCoordinate maxCoordinate(KDCoordinate x, KDCoordinate y) { return x > y ? x : y; }

const uint8_t radixPixel[NthRootLayoutNode::k_leftRadixHeight][NthRootLayoutNode::k_leftRadixWidth] = {
  {0x00, 0xFF, 0xFF, 0xFF, 0xFF},
  {0xFF, 0x00, 0xFF, 0xFF, 0xFF},
  {0xFF, 0x00, 0xFF, 0xFF, 0xFF},
  {0xFF, 0xFF, 0x00, 0xFF, 0xFF},
  {0xFF, 0xFF, 0x00, 0xFF, 0xFF},
  {0xFF, 0xFF, 0xFF, 0x00, 0xFF},
  {0xFF, 0xFF, 0xFF, 0x00, 0xFF},
  {0xFF, 0xFF, 0xFF, 0xFF, 0x00},
};

void NthRootLayoutNode::moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) {
  if (cursor->layoutNode() == radicandLayout()
    && cursor->position() == LayoutCursor::Position::Left)
  {
    // Case: Left of the radicand. Go the index if any, ir go Left of the root.
    if (indexLayout() != nullptr) {
      cursor->setLayoutNode(indexLayout());
      cursor->setPosition(LayoutCursor::Position::Right);
    } else {
      cursor->setLayoutNode(this);
    }
    return;
  }
  if (indexLayout() != nullptr
    && cursor->layoutNode() == indexLayout()
    && cursor->position() == LayoutCursor::Position::Left)
  {
    // Case: Left of the index. Go Left of the root.
    cursor->setLayoutNode(this);
    return;
  }
  assert(cursor->layoutNode() == this);
  if (cursor->position() == LayoutCursor::Position::Right) {
    // Case: Right. Go Right of the radicand.
    cursor->setLayoutNode(radicandLayout());
    return;
  }
  assert(cursor->position() == LayoutCursor::Position::Left);
  // Case: Left. Ask the parent.
  LayoutNode * parentNode = parent();
  if (parentNode != nullptr) {
    parentNode->moveCursorLeft(cursor, shouldRecomputeLayout);
  }
}

void NthRootLayoutNode::moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) {
  if (cursor->layoutNode() == radicandLayout()
      && cursor->position() == LayoutCursor::Position::Right)
  {
    // Case: Right of the radicand. Go the Right of the root.
    cursor->setLayoutNode(this);
    return;
  }
  if (indexLayout() != nullptr
    && cursor->layoutNode() == indexLayout()
    && cursor->position() == LayoutCursor::Position::Right)
  {
    assert(radicandLayout() != nullptr);
    cursor->setLayoutNode(radicandLayout());
    cursor->setPosition(LayoutCursor::Position::Left);
    return;
  }
  assert(cursor->layoutNode() == this);
  if (cursor->position() == LayoutCursor::Position::Left) {
    // Case: Left. Go to the index if there is one, else go to the radicand.
    if (indexLayout() != nullptr) {
      cursor->setLayoutNode(indexLayout());
    } else {
      cursor->setLayoutNode(radicandLayout());
    }
    return;
  }
  assert(cursor->position() == LayoutCursor::Position::Right);
  // Case: Right. Ask the parent.
  LayoutNode * parentNode = parent();
  if (parentNode != nullptr) {
    parentNode->moveCursorRight(cursor, shouldRecomputeLayout);
  }
}

void NthRootLayoutNode::moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  if (indexLayout() != nullptr
      && cursor->isEquivalentTo(LayoutCursor(radicandLayout(), LayoutCursor::Position::Left)))
  {
    // If the cursor is Left of the radicand, move it to the index.
    cursor->setLayoutNode(indexLayout());
    cursor->setPosition(LayoutCursor::Position::Right);
    return;
  }
  if (indexLayout() != nullptr
      && cursor->layoutNode() == this
      && cursor->position() == LayoutCursor::Position::Left)
  {
    // If the cursor is Left, move it to the index.
    cursor->setLayoutNode(indexLayout());
    cursor->setPosition(LayoutCursor::Position::Left);
    return;
  }
  LayoutNode::moveCursorUp(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

void NthRootLayoutNode::moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  if (indexLayout() != nullptr
      && cursor->layoutNode()->hasAncestor(indexLayout(), true))
  {
    if (cursor->isEquivalentTo(LayoutCursor(indexLayout(), LayoutCursor::Position::Right))) {
      // If the cursor is Right of the index, move it to the radicand.
      cursor->setLayoutNode(radicandLayout());
      cursor->setPosition(LayoutCursor::Position::Left);
      return;
    }
    // If the cursor is Left of the index, move it Left .
    if (cursor->isEquivalentTo(LayoutCursor(indexLayout(), LayoutCursor::Position::Left))) {
      cursor->setLayoutNode(this);
      cursor->setPosition(LayoutCursor::Position::Left);
      return;
    }
  }
  LayoutNode::moveCursorDown(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

void NthRootLayoutNode::deleteBeforeCursor(LayoutCursor * cursor) {
  if (cursor->layoutNode() == radicandLayout()
      && cursor->position() == LayoutCursor::Position::Left)
  {
    // Case: Left of the radicand. Delete the layout, keep the radicand.
    Layout radicand = Layout(radicandLayout());
    Layout thisRef = Layout(this);
    thisRef.replaceChildWithGhostInPlace(radicand);
    // WARNING: Do not call "this" afterwards
    cursor->setLayout(thisRef.childAtIndex(0));
    thisRef.replaceWith(radicand, cursor);
    return;
  }
  LayoutNode::deleteBeforeCursor(cursor);
}

int NthRootLayoutNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (m_hasIndex) {
    assert((const_cast<NthRootLayoutNode *>(this))->indexLayout());
    if ((const_cast<NthRootLayoutNode *>(this))->indexLayout()->isEmpty()) {
      // Case: root(x,empty): Write "'SquareRootSymbol'('radicandLayout')"
      return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, SquareRoot::s_functionHelper.name(), 0);
    }
    // Case: root(x,n)
    return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, NthRoot::s_functionHelper.name());
  }
  // Case: squareRoot(x)
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, SquareRoot::s_functionHelper.name());
}

KDSize NthRootLayoutNode::computeSize() {
  KDSize radicandSize = radicandLayout()->layoutSize();
  KDSize indexSize = adjustedIndexSize();
  KDSize newSize = KDSize(
      indexSize.width() + 3*k_widthMargin + 2*k_radixLineThickness + radicandSize.width() + k_radixHorizontalOverflow,
      baseline() + radicandSize.height() - radicandLayout()->baseline() + k_heightMargin
      );
  return newSize;
}

KDCoordinate NthRootLayoutNode::computeBaseline() {
  if (indexLayout() != nullptr) {
    return maxCoordinate(
        radicandLayout()->baseline() + k_radixLineThickness + k_heightMargin,
        indexLayout()->layoutSize().height() + k_indexHeight);
  } else {
    return radicandLayout()->baseline() + k_radixLineThickness + k_heightMargin;
  }
}

KDPoint NthRootLayoutNode::positionOfChild(LayoutNode * child) {
  KDCoordinate x = 0;
  KDCoordinate y = 0;
  KDSize indexSize = adjustedIndexSize();
  if (child == radicandLayout()) {
    x = indexSize.width() + 2*k_widthMargin + k_radixLineThickness;
    y = baseline() - radicandLayout()->baseline();
  } else if (indexLayout() != nullptr && child == indexLayout()) {
    x = 0;
    y = baseline() - indexSize.height() -  k_indexHeight;
  } else {
    assert(false);
  }
  return KDPoint(x,y);
}

KDSize NthRootLayoutNode::adjustedIndexSize() {
  return indexLayout() == nullptr ?
    KDSize(k_leftRadixWidth, 0) :
    KDSize(maxCoordinate(k_leftRadixWidth, indexLayout()->layoutSize().width()), indexLayout()->layoutSize().height());
}

void NthRootLayoutNode::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  KDSize radicandSize = radicandLayout()->layoutSize();
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

bool NthRootLayoutNode::protectedIsIdenticalTo(Layout l) {
  assert(l.type() == Type::NthRootLayout);
  NthRootLayout & nrl = static_cast<NthRootLayout &>(l);
  return hasUpperLeftIndex() == nrl.node()->hasUpperLeftIndex() && LayoutNode::protectedIsIdenticalTo(l);
}

NthRootLayout NthRootLayout::Builder(Layout child) {
  void * bufferNode = TreePool::sharedPool()->alloc(sizeof(NthRootLayoutNode));
  NthRootLayoutNode * node = new (bufferNode) NthRootLayoutNode(false);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  h.replaceChildAtIndexInPlace(0, child);
  return static_cast<NthRootLayout &>(h);
}

NthRootLayout NthRootLayout::Builder(Layout child, Layout index) {
  void * bufferNode = TreePool::sharedPool()->alloc(sizeof(NthRootLayoutNode));
  NthRootLayoutNode * node = new (bufferNode) NthRootLayoutNode(true);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  h.replaceChildAtIndexInPlace(0, child);
  h.replaceChildAtIndexInPlace(1, index);
  return static_cast<NthRootLayout &>(h);
}

}
