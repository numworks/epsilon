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

bool cursorPositionNeedsEmptyBase(const Tree* node, int p) {
  bool leftIsBase = p > 0 && (!node->child(p - 1)->isVerticalOffsetLayout() ||
                              !VerticalOffset::IsPrefix(node->child(p - 1)));
  bool rightIsBase = p < node->numberOfChildren() &&
                     (!node->child(p)->isVerticalOffsetLayout() ||
                      !VerticalOffset::IsSuffix(node->child(p)));
  return !leftIsBase && !rightIsBase;
}

bool RackLayout::ShouldDrawEmptyBaseAt(const Tree* node, int p) {
  return !(layoutCursor && layoutCursor->cursorNode() == node &&
           layoutCursor->position() == p);
}

void RackLayout::IterBetweenIndexes(const Tree* node, int leftIndex,
                                    int rightIndex, Callback callback,
                                    void* context) {
  assert(0 <= leftIndex && leftIndex <= rightIndex &&
         rightIndex <= node->numberOfChildren());
  int numberOfChildren = node->numberOfChildren();
  if (numberOfChildren == 0) {
    KDSize emptySize = EmptyRectangle::RectangleSize(Render::font);
    KDCoordinate width = ShouldDrawEmptyRectangle(node) ? emptySize.width() : 0;
    callback(nullptr, KDSize(width, emptySize.height()),
             EmptyRectangle::RectangleBaseLine(Render::font),
             {0, EmptyRectangle::RectangleBaseLine(Render::font)}, context);
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
          callback(nullptr, EmptyRectangle::RectangleSize(Render::font),
                   EmptyRectangle::RectangleBaseLine(Render::font),
                   {x, EmptyRectangle::RectangleBaseLine(Render::font)},
                   context);
          x += EmptyRectangle::RectangleSize(Render::font).width();
        }
        baseHeight = EmptyRectangle::RectangleSize(Render::font).height();
        baseBaseline = EmptyRectangle::RectangleBaseLine(Render::font);
      } else {
        // TODO successive offsets
        baseHeight = Render::Height(base);
        baseBaseline = Render::Baseline(base);
      }
      if (VerticalOffset::IsSuperscript(child)) {
        childBaseline =
            baseBaseline + childSize.height() - VerticalOffset::IndiceHeight;
        y = childBaseline;
      } else {
        childBaseline = baseBaseline;
        y = baseBaseline - baseHeight + VerticalOffset::IndiceHeight;
      }
      childSize =
          childSize + KDSize(0, baseHeight - VerticalOffset::IndiceHeight);
    } else {
      lastBase = child;
    }
    callback(child, childSize, childBaseline, {x, y}, context);
    x += childSize.width();
    child = child->nextTree();
  }
}

KDSize RackLayout::SizeBetweenIndexes(const Tree* node, int leftIndex,
                                      int rightIndex) {
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
