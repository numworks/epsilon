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

bool RackLayout::ShouldDrawEmptyBaseAt(const Tree* node, int p) {
  return !(layoutCursor && layoutCursor->cursorNode() == node &&
           layoutCursor->position() == p);
}

void UpdateChildWithBase(bool isSuperscript, KDCoordinate baseHeight,
                         KDCoordinate baseBaseline, KDCoordinate* childBaseline,
                         KDCoordinate* childHeight, KDCoordinate* childY) {
  if (isSuperscript) {
    *childBaseline = baseBaseline + *childHeight - VerticalOffset::IndiceHeight;
    *childY = *childBaseline;
  } else {
    *childBaseline = baseBaseline;
    *childY = baseBaseline - baseHeight + VerticalOffset::IndiceHeight;
  }
  *childHeight += baseHeight - VerticalOffset::IndiceHeight;
}

void RackLayout::IterBetweenIndexes(const Tree* node, int leftIndex,
                                    int rightIndex, Callback callback,
                                    void* context) {
  assert(0 <= leftIndex && leftIndex <= rightIndex &&
         rightIndex <= node->numberOfChildren());
  int numberOfChildren = node->numberOfChildren();
  if (numberOfChildren == 0) {
    KDSize emptySize = EmptyRectangle::Size(Render::font);
    KDCoordinate width = ShouldDrawEmptyRectangle(node) ? emptySize.width() : 0;
    callback(nullptr, KDSize(width, emptySize.height()),
             EmptyRectangle::Baseline(Render::font),
             {0, EmptyRectangle::Baseline(Render::font)}, context);
    return;
  }
  const Tree* lastBase = nullptr;
  const Tree* child = node->child(0);
  for (int i = 0; i < leftIndex; i++) {
    if (!child->isVerticalOffsetLayout()) {
      lastBase = child;
    }
    child = child->nextTree();
  }
  KDCoordinate x = 0;
  for (int i = leftIndex; i < rightIndex; i++) {
    KDSize childSize = Render::Size(child);
    KDCoordinate childWidth = childSize.width();
    KDCoordinate childHeight = childSize.height();
    KDCoordinate childBaseline = Render::Baseline(child);
    KDCoordinate y = childBaseline;
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
          callback(nullptr, EmptyRectangle::Size(Render::font),
                   EmptyRectangle::Baseline(Render::font),
                   {x, EmptyRectangle::Baseline(Render::font)}, context);
          x += EmptyRectangle::Size(Render::font).width();
        }
        baseHeight = EmptyRectangle::Size(Render::font).height();
        baseBaseline = EmptyRectangle::Baseline(Render::font);
      } else {
        // TODO successive offsets
        baseHeight = Render::Height(base);
        baseBaseline = Render::Baseline(base);
      }
      UpdateChildWithBase(VerticalOffset::IsSuperscript(child), baseHeight,
                          baseBaseline, &childBaseline, &childHeight, &y);
    } else {
      lastBase = child;
    }
    callback(child, {childWidth, childHeight}, childBaseline, {x, y}, context);
    x += childWidth;
    child = child->nextTree();
  }
}

KDSize RackLayout::SizeBetweenIndexes(const Tree* node, int leftIndex,
                                      int rightIndex) {
  if (IsTrivial(node) && leftIndex == 0 && rightIndex == 1) {
    return Render::Size(node->child(0));
  }
  struct Context {
    KDCoordinate maxUnderBaseline;
    KDCoordinate maxAboveBaseline;
    KDCoordinate totalWidth;
  };
  auto iter = [](const Tree* child, KDSize childSize,
                 KDCoordinate childBaseline, KDPoint, void* ctx) {
    Context* context = static_cast<Context*>(ctx);
    context->totalWidth += childSize.width();
    context->maxUnderBaseline = std::max<KDCoordinate>(
        context->maxUnderBaseline, childSize.height() - childBaseline);
    context->maxAboveBaseline =
        std::max(context->maxAboveBaseline, childBaseline);
  };
  Context context = {};
  IterBetweenIndexes(node, leftIndex, rightIndex, iter, &context);
  return KDSize(context.totalWidth,
                context.maxUnderBaseline + context.maxAboveBaseline);
}

KDPoint RackLayout::ChildPosition(const Tree* node, int i) {
  if (IsTrivial(node)) {
    return KDPointZero;
  }
  KDCoordinate baseline = Baseline(node);
  struct Context {
    KDCoordinate x;
    KDCoordinate baseline;
  };
  auto iter2 = [](const Tree* child, KDSize childSize, KDCoordinate,
                  KDPoint position, void* ctx) {
    Context* context = static_cast<Context*>(ctx);
    context->x = position.x();
    context->baseline = position.y();
  };
  Context context;
  IterBetweenIndexes(node, 0, i + 1, iter2, &context);
  return KDPoint(context.x, baseline - context.baseline);
}

KDCoordinate RackLayout::BaselineBetweenIndexes(const Tree* node, int leftIndex,
                                                int rightIndex) {
  if (IsTrivial(node) && leftIndex == 0 && rightIndex == 1) {
    return Render::Baseline(node->child(0));
  }
  struct Context {
    KDCoordinate maxUnderBaseline;
    KDCoordinate maxAboveBaseline;
    KDCoordinate totalWidth;
  };
  auto iter = [](const Tree* child, KDSize childSize,
                 KDCoordinate childBaseline, KDPoint, void* ctx) {
    Context* context = static_cast<Context*>(ctx);
    context->totalWidth += childSize.width();
    context->maxUnderBaseline = std::max<KDCoordinate>(
        context->maxUnderBaseline, childSize.height() - childBaseline);
    context->maxAboveBaseline =
        std::max(context->maxAboveBaseline, childBaseline);
  };
  Context context = {};
  IterBetweenIndexes(node, leftIndex, rightIndex, iter, &context);
  return context.maxAboveBaseline;
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
}

}  // namespace PoincareJ
