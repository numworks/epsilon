#include "rack_layout.h"

#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/n_ary.h>

#include "empty_rectangle.h"
#include "indices.h"
#include "layout_cursor.h"
#include "render.h"

namespace PoincareJ {

namespace VerticalOffset {
constexpr static KDCoordinate IndiceHeight = 10;
}  // namespace VerticalOffset

const LayoutCursor* RackLayout::layoutCursor = nullptr;

KDSize RackLayout::Size(const Tree* node) {
  return SizeBetweenIndexes(node, 0, node->numberOfChildren());
}

KDCoordinate RackLayout::Baseline(const Tree* node) {
  return BaselineBetweenIndexes(node, 0, node->numberOfChildren());
}

/* Return -1 if base is unavailable */
int baseForChild(const Tree* rack, const Tree* child, int childIndex) {
  int baseIndex = childIndex + (VerticalOffset::IsSuffix(child) ? -1 : 1);
  if (baseIndex == -1 || baseIndex == rack->numberOfChildren()) {
    return -1;
  }
  const Tree* base = rack->child(baseIndex);
  if (base->isVerticalOffsetLayout() &&
      (VerticalOffset::IsSuffix(child) != VerticalOffset::IsSuffix(base))) {
    // Break infinite loop
    return -1;
  }
  return baseIndex;
}

KDCoordinate RackLayout::ChildBaseline(const Tree* node, int i) {
  const Tree* childI = node->child(i);
  if (!childI->isVerticalOffsetLayout()) {
    return Render::Baseline(childI);
  }
  int baseIndex = baseForChild(node, childI, i);
  KDCoordinate baseBaseline = baseIndex == -1
                                  ? KDFont::GlyphHeight(Render::font) / 2
                                  : ChildBaseline(node, baseIndex);
  if (!VerticalOffset::IsSuperscript(childI)) {
    return baseBaseline;
  }
  return baseBaseline + Render::Height(childI) - VerticalOffset::IndiceHeight;
}

KDCoordinate RackLayout::ChildYPosition(const Tree* node, int i) {
  const Tree* childI = node->child(i);
  if (!childI->isVerticalOffsetLayout() ||
      VerticalOffset::IsSuperscript(childI)) {
    return Baseline(node) - RackLayout::ChildBaseline(node, i);
  }
  int baseIndex = baseForChild(node, childI, i);
  KDCoordinate baseHeight =
      baseIndex == -1
          ? KDFont::GlyphHeight(Render::font)
          : SizeBetweenIndexes(node, baseIndex, baseIndex + 1).height();
  return Baseline(node) - RackLayout::ChildBaseline(node, i) + baseHeight -
         VerticalOffset::IndiceHeight;
}

bool cursorPositionNeedsEmptyBase(const Tree* node, int p) {
  bool leftIsBase = p > 0 && (!node->child(p - 1)->isVerticalOffsetLayout() ||
                              !VerticalOffset::IsPrefix(node->child(p - 1)));
  bool rightIsBase = p < node->numberOfChildren() &&
                     (!node->child(p)->isVerticalOffsetLayout() ||
                      !VerticalOffset::IsSuffix(node->child(p)));
  return !leftIsBase && !rightIsBase;
}

KDPoint RackLayout::ChildPosition(const Tree* node, int i) {
  KDCoordinate x = SizeBetweenIndexes(node, 0, i).width();
  if (cursorPositionNeedsEmptyBase(node, i) && ShouldDrawEmptyBaseAt(node, i)) {
    x += EmptyRectangle::RectangleSize(Render::font).width();
  }
  KDCoordinate y = ChildYPosition(node, i);
  return KDPoint(x, y);
}

bool RackLayout::ShouldDrawEmptyBaseAt(const Tree* node, int p) {
  return !(layoutCursor && layoutCursor->cursorNode() == node &&
           layoutCursor->position() == p);
}

KDSize RackLayout::SizeBetweenIndexes(const Tree* node, int leftIndex,
                                      int rightIndex) {
  assert(0 <= leftIndex && leftIndex <= rightIndex &&
         rightIndex <= node->numberOfChildren());
  if (node->numberOfChildren() == 0) {
    KDSize emptyRectangleSize = EmptyRectangle::RectangleSize(Render::font);
    KDCoordinate width =
        ShouldDrawEmptyRectangle(node) ? emptyRectangleSize.width() : 0;
    return KDSize(width, emptyRectangleSize.height());
  }
  KDCoordinate totalWidth = 0;
  KDCoordinate maxUnderBaseline = 0;
  KDCoordinate maxAboveBaseline = 0;
  int numberOfChildren = node->numberOfChildren();
  const Tree* lastBase = nullptr;
  const Tree* child = node->child(0);
  for (int i = 0; i < leftIndex; i++) {
    if (!child->isVerticalOffsetLayout()) {
      lastBase = child;
    }
    child = child->nextTree();
  }
  for (int i = leftIndex; i < rightIndex; i++) {
    KDSize childSize = Render::Size(child);
    KDCoordinate childBaseline = Render::Baseline(child);
    if (child->isVerticalOffsetLayout()) {
      const Tree* base = nullptr;
      if (VerticalOffset::IsSuffix(child)) {
        // Use base
        base = lastBase;
      } else {
        // Find base forward
        int j = i;
        const Tree* candidateBase = child->nextTree();
        while (j < numberOfChildren) {
          if (!candidateBase->isVerticalOffsetLayout()) {
            base = candidateBase;
            break;
          }
          if (VerticalOffset::IsSuffix(candidateBase)) {
            // Add an empty base
            base = nullptr;
            break;
          }
          candidateBase = candidateBase->nextTree();
          j++;
        }
      }
      KDCoordinate baseHeight, baseBaseline;
      if (!base) {
        // Add an empty base
        if (ShouldDrawEmptyBaseAt(node, i)) {
          totalWidth += EmptyRectangle::RectangleSize(Render::font).width();
        }
        baseHeight = EmptyRectangle::RectangleSize(Render::font).height();
        baseBaseline = EmptyRectangle::RectangleBaseLine(Render::font);
      } else {
        // TODO successive offsets
        baseHeight = Render::Height(base);
        baseBaseline = Render::Baseline(base);
      }
      if (!VerticalOffset::IsSuperscript(child)) {
        childBaseline = baseBaseline;
      } else {
        childBaseline =
            baseBaseline + childSize.height() - VerticalOffset::IndiceHeight;
      }
      childSize =
          childSize + KDSize(0, baseHeight - VerticalOffset::IndiceHeight);
    } else {
      lastBase = child;
    }
    child = child->nextTree();
    totalWidth += childSize.width();
    maxUnderBaseline = std::max<KDCoordinate>(
        maxUnderBaseline, childSize.height() - childBaseline);
    maxAboveBaseline = std::max(maxAboveBaseline, childBaseline);
  }
  return KDSize(totalWidth, maxUnderBaseline + maxAboveBaseline);
}

KDCoordinate RackLayout::BaselineBetweenIndexes(const Tree* node, int leftIndex,
                                                int rightIndex) {
  assert(0 <= leftIndex && leftIndex <= rightIndex &&
         rightIndex <= node->numberOfChildren());
  if (node->numberOfChildren() == 0) {
    return EmptyRectangle::RectangleBaseLine(Render::font);
  }
  KDCoordinate result = 0;
  for (int i = leftIndex; i < rightIndex; i++) {
    result = std::max(result, ChildBaseline(node, i));
  }
  return result;
}

bool RackLayout::ShouldDrawEmptyRectangle(const Tree* node) {
  if (node->numberOfChildren() != 0 || !Render::showEmptyRack) {
    return false;
  }
  if (!RackLayout::layoutCursor) {
    return true;
  }
  if (node == layoutCursor->cursorNode()) {
    return false;
  }
  return true;
}

void RackLayout::RenderNode(const Tree* node, KDContext* ctx, KDPoint pos,
                            bool isGridPlaceholder) {
  if (ShouldDrawEmptyRectangle(node)) {
    EmptyRectangle::DrawEmptyRectangle(ctx, pos, Render::font,
                                       isGridPlaceholder
                                           ? EmptyRectangle::Color::Gray
                                           : EmptyRectangle::Color::Yellow);
  }
  if (node->numberOfChildren() > 0) {
    for (int p = 0; p <= node->numberOfChildren(); p++) {
      if (cursorPositionNeedsEmptyBase(node, p) &&
          ShouldDrawEmptyBaseAt(node, p)) {
        EmptyRectangle::DrawEmptyRectangle(
            ctx,
            pos.translatedBy(KDPoint(
                SizeBetweenIndexes(node, 0, p).width(),
                Baseline(node) - KDFont::GlyphHeight(Render::font) / 2)),
            Render::font,
            isGridPlaceholder ? EmptyRectangle::Color::Gray
                              : EmptyRectangle::Color::Yellow);
      }
    }
  }
}

}  // namespace PoincareJ
