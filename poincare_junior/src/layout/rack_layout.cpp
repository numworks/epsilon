#include "rack_layout.h"

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

const LayoutCursor* RackLayout::s_layoutCursor = nullptr;

KDSize RackLayout::Size(const Rack* node) {
  return SizeBetweenIndexes(node, 0, node->numberOfChildren());
}

KDCoordinate RackLayout::Baseline(const Rack* node) {
  return BaselineBetweenIndexes(node, 0, node->numberOfChildren());
}

bool RackLayout::ShouldDrawEmptyBaseAt(const Rack* node, int p) {
  return !(s_layoutCursor && s_layoutCursor->cursorNode() == node &&
           s_layoutCursor->position() == p);
}

void UpdateChildWithBase(bool isSuperscript, KDCoordinate baseHeight,
                         KDCoordinate baseBaseline, KDCoordinate* childBaseline,
                         KDCoordinate* childHeight,
                         KDCoordinate* childY = nullptr) {
  if (isSuperscript) {
    *childBaseline = baseBaseline + *childHeight - VerticalOffset::IndiceHeight;
    if (childY) {
      *childY = *childBaseline;
    }
  } else {
    *childBaseline = baseBaseline;
    if (childY) {
      *childY = baseBaseline - baseHeight + VerticalOffset::IndiceHeight;
    }
  }
  *childHeight += baseHeight - VerticalOffset::IndiceHeight;
}

#if 0
/* TODO this code will be needed to render nested prefix subscript which are not
 * used currently */
void FindBaseForward(const LayoutT* child, int maxDepth,
                     KDCoordinate* baseHeight, KDCoordinate* baseBaseline,
                     KDFont::Size font) {
  if (maxDepth == 0) {
    *baseBaseline = EmptyRectangle::Baseline(font);
    *baseHeight = EmptyRectangle::Size(font).width();
  }
  const LayoutT* candidateBase = child->nextTree();
  if (candidateBase->isVerticalOffsetLayout()) {
    if (VerticalOffset::IsSuffix(candidateBase)) {
      // Add an empty base
      return FindBaseForward(child, 0, baseHeight, baseBaseline, font);
    }
    /* This case is there to support successive prefix offsets that are never
     * useful, we could delete it and simplify the code. */
    KDCoordinate baseBaseHeight;
    KDCoordinate baseBaseBaseline;
    FindBaseForward(candidateBase, maxDepth - 1, &baseBaseHeight,
                    &baseBaseBaseline, font);
    UpdateChildWithBase(VerticalOffset::IsSuperscript(candidateBase),
                        baseBaseHeight, baseBaseBaseline, baseBaseline,
                        baseHeight);
    return;
  }
  *baseHeight = Render::Size(candidateBase).height();
  *baseBaseline = Render::Baseline(candidateBase);
}
#endif

void RackLayout::IterBetweenIndexes(const Rack* node, int leftIndex,
                                    int rightIndex, Callback callback,
                                    void* context) {
  assert(0 <= leftIndex && leftIndex <= rightIndex &&
         rightIndex <= node->numberOfChildren());
  int numberOfChildren = node->numberOfChildren();
  if (numberOfChildren == 0) {
    KDSize emptySize = EmptyRectangle::Size(Render::s_font);
    KDCoordinate width = ShouldDrawEmptyRectangle(node) ? emptySize.width() : 0;
    callback(nullptr, KDSize(width, emptySize.height()),
             EmptyRectangle::Baseline(Render::s_font),
             {0, EmptyRectangle::Baseline(Render::s_font)}, context);
    return;
  }
  const LayoutT* lastBase = nullptr;
  const LayoutT* child = node->child(0);
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
      const LayoutT* base = nullptr;
      if (VerticalOffset::IsSuffix(child)) {
        // Use base
        base = lastBase;
      } else {
        // Find base forward
        int j = i;
        const LayoutT* candidateBase = child->nextTree();
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
          callback(nullptr, EmptyRectangle::Size(Render::s_font),
                   EmptyRectangle::Baseline(Render::s_font),
                   {x, EmptyRectangle::Baseline(Render::s_font)}, context);
          x += EmptyRectangle::Size(Render::s_font).width();
        }
        baseHeight = EmptyRectangle::Size(Render::s_font).height();
        baseBaseline = EmptyRectangle::Baseline(Render::s_font);
      } else {
        baseHeight = Render::Height(base);
        baseBaseline = Render::Baseline(base);
        base = base->nextTree();
        while (base < child) {
          KDCoordinate oldBaseHeight = baseHeight;
          KDCoordinate oldBaseBaseline = baseBaseline;
          KDSize baseSize = Render::Size(base);
          baseHeight = baseSize.height();
          baseBaseline = Render::Baseline(base);
          baseHeight = childHeight;
          baseBaseline = childBaseline;
          UpdateChildWithBase(VerticalOffset::IsSuperscript(base),
                              oldBaseHeight, oldBaseBaseline, &baseBaseline,
                              &baseHeight);
          base = base->nextTree();
        }
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

KDSize RackLayout::SizeBetweenIndexes(const Rack* node, int leftIndex,
                                      int rightIndex) {
  if (IsTrivial(node) && leftIndex == 0 && rightIndex == 1) {
    return Render::Size(node->child(0));
  }
  struct Context {
    KDCoordinate maxUnderBaseline;
    KDCoordinate maxAboveBaseline;
    KDCoordinate totalWidth;
  };
  Callback* iter = [](const LayoutT* child, KDSize childSize,
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

KDPoint RackLayout::ChildPosition(const Rack* node, int i) {
  if (IsTrivial(node)) {
    return KDPointZero;
  }
  KDCoordinate baseline = Baseline(node);
  struct Context {
    KDCoordinate x;
    KDCoordinate baseline;
  };
  Callback* iter = [](const LayoutT* child, KDSize childSize, KDCoordinate,
                      KDPoint position, void* ctx) {
    Context* context = static_cast<Context*>(ctx);
    context->x = position.x();
    context->baseline = position.y();
  };
  Context context;
  IterBetweenIndexes(node, 0, i + 1, iter, &context);
  return KDPoint(context.x, baseline - context.baseline);
}

KDCoordinate RackLayout::BaselineBetweenIndexes(const Rack* node, int leftIndex,
                                                int rightIndex) {
  if (IsTrivial(node) && leftIndex == 0 && rightIndex == 1) {
    return Render::Baseline(node->child(0));
  }
  struct Context {
    KDCoordinate maxUnderBaseline;
    KDCoordinate maxAboveBaseline;
    KDCoordinate totalWidth;
  };
  Callback* iter = [](const LayoutT* child, KDSize childSize,
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

bool RackLayout::ShouldDrawEmptyRectangle(const Rack* node) {
  if (node->numberOfChildren() != 0 || !Render::s_showEmptyRack) {
    return false;
  }
  if (!RackLayout::s_layoutCursor) {
    return true;
  }
  if (node == s_layoutCursor->cursorNode()) {
    return false;
  }
  return true;
}

void RackLayout::RenderNode(const Rack* node, KDContext* ctx, KDPoint pos,
                            bool isGridPlaceholder) {
  if (ShouldDrawEmptyRectangle(node)) {
    EmptyRectangle::DrawEmptyRectangle(ctx, pos, Render::s_font,
                                       isGridPlaceholder
                                           ? EmptyRectangle::Color::Gray
                                           : EmptyRectangle::Color::Yellow);
  }
}

}  // namespace PoincareJ
