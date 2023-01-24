#include <poincare/nth_root.h>
#include <poincare/nth_root_layout.h>
#include <poincare/square_root.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <assert.h>
#include <algorithm>

namespace Poincare {

constexpr KDCoordinate NthRootLayoutNode::k_leftRadixHeight;
constexpr KDCoordinate NthRootLayoutNode::k_leftRadixWidth;

const uint8_t radixPixel[NthRootLayoutNode::k_leftRadixHeight][NthRootLayoutNode::k_leftRadixWidth] = {
{0x51, 0xCC, 0xFF, 0xFF, 0xFF},
{0x96, 0x37, 0xFD, 0xFF, 0xFF},
{0xFC, 0x34, 0x9A, 0xFF, 0xFF},
{0xFF, 0xC8, 0x15, 0xEC, 0xFF},
{0xFF, 0xFF, 0x65, 0x66, 0xFF},
{0xFF, 0xFF, 0xEC, 0x15, 0xC9},
{0xFF, 0xFF, 0xFF, 0x99, 0x34},
{0xFF, 0xFF, 0xFF, 0xFD, 0x36},
{0xFF, 0xFF, 0xFF, 0xFF, 0xCB}
};

bool NthRootLayoutNode::isSquareRoot() const {
  if (!m_hasIndex) {
    return true;
  }
  assert((const_cast<NthRootLayoutNode *>(this))->indexLayout() != nullptr);
  if ((const_cast<NthRootLayoutNode *>(this))->indexLayout()->isEmpty()) {
    return true;
  }
  return false;
}

void NthRootLayoutNode::moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
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

void NthRootLayoutNode::moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
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

void NthRootLayoutNode::moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited, bool forSelection) {
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

void NthRootLayoutNode::moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited, bool forSelection) {
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
  if (!deleteBeforeCursorForLayoutContainingArgument(radicandLayout(), cursor)) {
    LayoutNode::deleteBeforeCursor(cursor);
  }
}

int NthRootLayoutNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (isSquareRoot()) {
    /* Case: squareRoot(x) or root(x,empty):
     * Write "SquareRootSymbol(radicandLayout) */
    return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, SquareRoot::s_functionHelper.aliasesList().mainAlias(), SerializationHelper::ParenthesisType::System, 0);
  }
  // Case: root(x,n)
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, NthRoot::s_functionHelper.aliasesList().mainAlias(), SerializationHelper::ParenthesisType::System);
}

KDSize NthRootLayoutNode::computeSize(KDFont::Size font) {
  KDSize radicandSize = radicandLayout()->layoutSize(font);
  KDSize indexSize = adjustedIndexSize(font);
  KDSize newSize = KDSize(
      indexSize.width() + 3*k_widthMargin + k_radixLineThickness + radicandSize.width(),
      baseline(font) + radicandSize.height() - radicandLayout()->baseline(font)
      );
  return newSize;
}

KDCoordinate NthRootLayoutNode::computeBaseline(KDFont::Size font) {
  return std::max<KDCoordinate>(
      radicandLayout()->baseline(font) + k_radixLineThickness + k_heightMargin,
      adjustedIndexSize(font).height());
}

KDPoint NthRootLayoutNode::positionOfChild(LayoutNode * child, KDFont::Size font) {
  KDCoordinate x = 0;
  KDCoordinate y = 0;
  KDSize indexSize = adjustedIndexSize(font);
  if (child == radicandLayout()) {
    x = indexSize.width() + 2*k_widthMargin + k_radixLineThickness;
    y = baseline(font) - radicandLayout()->baseline(font);
  } else if (indexLayout() != nullptr && child == indexLayout()) {
    x = 0;
    y = baseline(font) - indexSize.height();
  } else {
    assert(false);
  }
  return KDPoint(x,y);
}

KDSize NthRootLayoutNode::adjustedIndexSize(KDFont::Size font) {
  return indexLayout() == nullptr ?
    KDSize(k_leftRadixWidth, 0) :
    KDSize(std::max(k_leftRadixWidth, indexLayout()->layoutSize(font).width()), indexLayout()->layoutSize(font).height());
}

void NthRootLayoutNode::render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart, Layout * selectionEnd, KDColor selectionColor) {
  KDSize radicandSize = radicandLayout()->layoutSize(font);
  KDSize indexSize = adjustedIndexSize(font);
  KDColor workingBuffer[k_leftRadixWidth*k_leftRadixHeight];
  KDRect leftRadixFrame(p.x() + indexSize.width() + k_widthMargin - k_leftRadixWidth,
    p.y() + baseline(font) + radicandSize.height() - radicandLayout()->baseline(font) - k_leftRadixHeight,
    k_leftRadixWidth, k_leftRadixHeight);
  ctx->blendRectWithMask(leftRadixFrame, expressionColor, (const uint8_t *)radixPixel, (KDColor *)workingBuffer);
  // If the indice is higher than the root.
  if (indexSize.height() > radicandLayout()->baseline(font) + k_radixLineThickness + k_heightMargin) {
    // Vertical radix bar
    ctx->fillRect(KDRect(p.x() + indexSize.width() + k_widthMargin,
                         p.y() + indexSize.height() - radicandLayout()->baseline(font) - k_radixLineThickness - k_heightMargin,
                         k_radixLineThickness,
                         radicandSize.height() + k_heightMargin + k_radixLineThickness), expressionColor);
    // Horizontal radix bar
    ctx->fillRect(KDRect(p.x() + indexSize.width() + k_widthMargin,
                         p.y() + indexSize.height() - radicandLayout()->baseline(font) - k_radixLineThickness - k_heightMargin,
                         radicandSize.width() + 2*k_widthMargin+1,
                         k_radixLineThickness), expressionColor);
  } else {
    ctx->fillRect(KDRect(p.x() + indexSize.width() + k_widthMargin,
                         p.y(),
                         k_radixLineThickness,
                         radicandSize.height() + k_heightMargin + k_radixLineThickness), expressionColor);
    ctx->fillRect(KDRect(p.x() + indexSize.width() + k_widthMargin,
                         p.y(),
                         radicandSize.width() + 2*k_widthMargin,
                         k_radixLineThickness), expressionColor);
  }
}

bool NthRootLayoutNode::protectedIsIdenticalTo(Layout l) {
  assert(l.type() == Type::NthRootLayout);
  NthRootLayout & nrl = static_cast<NthRootLayout &>(l);
  return hasUpperLeftIndex() == nrl.node()->hasUpperLeftIndex() && LayoutNode::protectedIsIdenticalTo(l);
}

NthRootLayout NthRootLayout::Builder(Layout child) {
  void * bufferNode = TreePool::sharedPool->alloc(sizeof(NthRootLayoutNode));
  NthRootLayoutNode * node = new (bufferNode) NthRootLayoutNode(false);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  h.replaceChildAtIndexInPlace(0, child);
  return static_cast<NthRootLayout &>(h);
}

NthRootLayout NthRootLayout::Builder(Layout child, Layout index) {
  void * bufferNode = TreePool::sharedPool->alloc(sizeof(NthRootLayoutNode));
  NthRootLayoutNode * node = new (bufferNode) NthRootLayoutNode(true);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  h.replaceChildAtIndexInPlace(0, child);
  h.replaceChildAtIndexInPlace(1, index);
  return static_cast<NthRootLayout &>(h);
}

}
