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

int NthRootLayoutNode::indexAfterHorizontalCursorMove(OMG::HorizontalDirection direction, int currentIndex, bool * shouldRedrawLayout) {
  if (!m_hasIndex) {
    return LayoutNode::indexAfterHorizontalCursorMove(direction, currentIndex, shouldRedrawLayout);
  }
  switch (currentIndex) {
  case k_outsideIndex:
    return direction.isRight() ? k_indexLayoutIndex : k_radicandLayoutIndex;
  case k_indexLayoutIndex:
    return direction.isRight() ? k_radicandLayoutIndex : k_outsideIndex;
  default:
    assert(currentIndex == k_radicandLayoutIndex);
    return direction.isRight() ? k_outsideIndex : k_indexLayoutIndex;
  }
}

int NthRootLayoutNode::indexAfterVerticalCursorMove(OMG::VerticalDirection direction, int currentIndex, PositionInLayout positionAtCurrentIndex, bool * shouldRedrawLayout) {
  if (!m_hasIndex) {
    return k_cantMoveIndex;
  }

  if (direction.isUp() &&
      positionAtCurrentIndex == PositionInLayout::Left &&
      (currentIndex == k_outsideIndex || currentIndex == k_radicandLayoutIndex))
  {
    return k_indexLayoutIndex;
  }

  if (direction.isDown() &&
      currentIndex == k_indexLayoutIndex &&
      positionAtCurrentIndex != PositionInLayout::Middle)
  {
    return positionAtCurrentIndex == PositionInLayout::Right ? k_radicandLayoutIndex : k_outsideIndex;
  }
  return k_cantMoveIndex;
}

LayoutNode::DeletionMethod NthRootLayoutNode::deletionMethodForCursorLeftOfChild(int childIndex) const {
  return StandardDeletionMethodForLayoutContainingArgument(childIndex, k_radicandLayoutIndex);
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

void NthRootLayoutNode::render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor) {
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
