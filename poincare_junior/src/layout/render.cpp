#include "render.h"

#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/memory/node_iterator.h>

#include "code_point_layout.h"
#include "fraction_layout.h"
#include "parenthesis_layout.h"
#include "rack_layout.h"
#include "vertical_offset_layout.h"

namespace PoincareJ {

KDSize Render::Size(const Tree* node, const Tree* root, KDFont::Size font) {
  switch (node->layoutType()) {
    case LayoutType::Rack:
      return RackLayout::Size(node, root, font);
    case LayoutType::Fraction:
      return FractionLayout::Size(node, root, font);
    case LayoutType::Parenthesis:
      return ParenthesisLayout::Size(node, root, font);
    case LayoutType::VerticalOffset:
      return VerticalOffsetLayout::Size(node, root, font);
    case LayoutType::CodePoint:
      return CodePointLayout::Size(node, root, font);
  };
}

KDPoint Render::AbsoluteOrigin(const Tree* node, const Tree* root,
                               KDFont::Size font) {
  assert(node->type().isLayout());
  if (node == root) {
    return KDPointZero;
  }
  int index;
  const Tree* parent = root->parentOfDescendant(node, &index);
  return AbsoluteOrigin(parent, root, font)
      .translatedBy(PositionOfChild(parent, index, root, font));
}

KDPoint Render::PositionOfChild(const Tree* node, int childIndex,
                                const Tree* root, KDFont::Size font) {
  switch (node->layoutType()) {
    case LayoutType::Rack:
      return RackLayout::PositionOfChild(node, childIndex, root, font);
    case LayoutType::Fraction:
      return FractionLayout::PositionOfChild(node, childIndex, root, font);
    case LayoutType::Parenthesis:
      return ParenthesisLayout::PositionOfChild(node, childIndex, root, font);
    case LayoutType::VerticalOffset:
      return VerticalOffsetLayout::PositionOfChild(node, childIndex, root,
                                                   font);
    case LayoutType::CodePoint:
      assert(false);
  };
}

KDCoordinate Render::Baseline(const Tree* node, const Tree* root,
                              KDFont::Size font) {
  switch (node->layoutType()) {
    case LayoutType::Rack:
      return RackLayout::Baseline(node, root, font);
    case LayoutType::Fraction:
      return FractionLayout::Baseline(node, root, font);
    case LayoutType::Parenthesis:
      return ParenthesisLayout::Baseline(node, root, font);
    case LayoutType::VerticalOffset:
      return VerticalOffsetLayout::Baseline(node, root, font);
    case LayoutType::CodePoint:
      return CodePointLayout::Baseline(node, root, font);
  };
}

void Render::Draw(const Tree* node, KDContext* ctx, KDPoint p,
                  KDFont::Size font, KDColor expressionColor,
                  KDColor backgroundColor) {
  PrivateDraw(node, node, ctx, p, font, expressionColor, backgroundColor);
}

void Render::PrivateDraw(const Tree* node, const Tree* root, KDContext* ctx,
                         KDPoint p, KDFont::Size font, KDColor expressionColor,
                         KDColor backgroundColor) {
  assert(node->type().isLayout());
  KDSize size = Size(node, root, font);
  if (size.height() <= 0 || size.width() <= 0 ||
      size.height() > KDCOORDINATE_MAX - p.y() ||
      size.width() > KDCOORDINATE_MAX - p.x()) {
    // Layout size overflows KDCoordinate
    return;
  }
  /* Redraw the background for each Tree* (used with selection which isn't
   * implemented yet) */
  ctx->fillRect(KDRect(p, size), backgroundColor);
  RenderNode(node, root, ctx, p, font, expressionColor, backgroundColor);
  for (auto [child, index] : NodeIterator::Children<NoEditable>(node)) {
    PrivateDraw(child, root, ctx,
                PositionOfChild(node, index, root, font).translatedBy(p), font,
                expressionColor, backgroundColor);
  }
}

void Render::RenderNode(const Tree* node, const Tree* root, KDContext* ctx,
                        KDPoint p, KDFont::Size font, KDColor expressionColor,
                        KDColor backgroundColor) {
  switch (node->layoutType()) {
    case LayoutType::Fraction:
      return FractionLayout::RenderNode(node, root, ctx, p, font,
                                        expressionColor, backgroundColor);
    case LayoutType::Parenthesis:
      return ParenthesisLayout::RenderNode(node, root, ctx, p, font,
                                           expressionColor, backgroundColor);
    case LayoutType::CodePoint:
      return CodePointLayout::RenderNode(node, root, ctx, p, font,
                                         expressionColor, backgroundColor);
    case LayoutType::Rack:
      return RackLayout::RenderNode(node, root, ctx, p, font, expressionColor,
                                    backgroundColor);
    case LayoutType::VerticalOffset:;
  };
}

int Render::IndexAfterHorizontalCursorMove(const Tree* node,
                                           OMG::HorizontalDirection direction,
                                           int currentIndex,
                                           bool* shouldRedraw) {
  int nChildren = node->numberOfChildren();
  if (nChildren == 0) {
    assert(currentIndex == k_outsideIndex);
    return k_outsideIndex;
  }
  if (Layout::IsHorizontal(node)) {
    nChildren += 1;
  }
  if (nChildren == 1) {
    assert(currentIndex == k_outsideIndex || currentIndex == 0);
    return currentIndex == k_outsideIndex ? 0 : k_outsideIndex;
  }
#if 0
  assert(false);
  return k_cantMoveIndex;
#else
  // TODO Implement other layout's logic instead of this dummy generalization
  currentIndex += (direction.isRight() ? 1 : -1);
  if (currentIndex == nChildren) {
    return k_outsideIndex;
  }
  if (currentIndex == k_outsideIndex - 1) {
    return nChildren - 1;
  }
  return currentIndex;
#endif
}

}  // namespace PoincareJ
