#include "render.h"

#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/memory/node_iterator.h>

#include "code_point_layout.h"
#include "fraction_layout.h"
#include "parenthesis_layout.h"
#include "rack_layout.h"
#include "vertical_offset_layout.h"

namespace PoincareJ {

KDSize Render::Size(const Node* node, KDFont::Size font) {
  assert(node->block()->isLayout());
  switch (node->type()) {
    case BlockType::RackLayout:
      return RackLayout::Size(node, font);
    case BlockType::FractionLayout:
      return FractionLayout::Size(node, font);
    case BlockType::ParenthesisLayout:
      return ParenthesisLayout::Size(node, font);
    case BlockType::VerticalOffsetLayout:
      return VerticalOffsetLayout::Size(node, font);
    case BlockType::CodePointLayout:
      return CodePointLayout::Size(node, font);
    default:
      assert(false);
  };
  return KDSizeZero;
}

KDPoint Render::AbsoluteOrigin(const Node* node, KDFont::Size font) {
  assert(node->block()->isLayout());
  const Node* parent = node->parent();
  if (!parent) {
    return KDPointZero;
  }
  return AbsoluteOrigin(parent, font)
      .translatedBy(PositionOfChild(parent, parent->indexOfChild(node), font));
}

KDPoint Render::PositionOfChild(const Node* node, int childIndex,
                                KDFont::Size font) {
  assert(node->block()->isLayout());
  switch (node->type()) {
    case BlockType::RackLayout:
      return RackLayout::PositionOfChild(node, childIndex, font);
    case BlockType::FractionLayout:
      return FractionLayout::PositionOfChild(node, childIndex, font);
    case BlockType::ParenthesisLayout:
      return ParenthesisLayout::PositionOfChild(node, childIndex, font);
    case BlockType::VerticalOffsetLayout:
      return VerticalOffsetLayout::PositionOfChild(node, childIndex, font);
    default:
      assert(false);
      return KDPointZero;
  };
}

KDCoordinate Render::Baseline(const Node* node, KDFont::Size font) {
  assert(node->block()->isLayout());
  switch (node->type()) {
    case BlockType::RackLayout:
      return RackLayout::Baseline(node, font);
    case BlockType::FractionLayout:
      return FractionLayout::Baseline(node, font);
    case BlockType::ParenthesisLayout:
      return ParenthesisLayout::Baseline(node, font);
    case BlockType::VerticalOffsetLayout:
      return VerticalOffsetLayout::Baseline(node, font);
    case BlockType::CodePointLayout:
      return CodePointLayout::Baseline(node, font);
    default:
      assert(false);
      return static_cast<KDCoordinate>(0);
  };
}

void Render::Draw(const Node* node, KDContext* ctx, KDPoint p,
                  KDFont::Size font, KDColor expressionColor,
                  KDColor backgroundColor) {
  /* AbsoluteOrigin relies on the fact that any layout is drawn as a whole.
   * Drawing is therefore restricted to the highest parent only. */
  assert(!node->parent());
  PrivateDraw(node, ctx, p, font, expressionColor, backgroundColor);
}

void Render::PrivateDraw(const Node* node, KDContext* ctx, KDPoint p,
                         KDFont::Size font, KDColor expressionColor,
                         KDColor backgroundColor) {
  assert(node->block()->isLayout());
  KDSize size = Size(node, font);
  if (size.height() <= 0 || size.width() <= 0 ||
      size.height() > KDCOORDINATE_MAX - p.y() ||
      size.width() > KDCOORDINATE_MAX - p.x()) {
    // Layout size overflows KDCoordinate
    return;
  }
  /* Redraw the background for each Node* (used with selection which isn't
   * implemented yet) */
  ctx->fillRect(KDRect(p, size), backgroundColor);
  RenderNode(node, ctx, p, font, expressionColor, backgroundColor);
  for (auto [child, index] :
       NodeIterator::Children<Forward, NoEditable>(node)) {
    PrivateDraw(child, ctx, PositionOfChild(node, index, font).translatedBy(p),
                font, expressionColor, backgroundColor);
  }
}

void Render::RenderNode(const Node* node, KDContext* ctx, KDPoint p,
                        KDFont::Size font, KDColor expressionColor,
                        KDColor backgroundColor) {
  assert(node->block()->isLayout());
  switch (node->type()) {
    case BlockType::FractionLayout:
      return FractionLayout::RenderNode(node, ctx, p, font, expressionColor,
                                        backgroundColor);
    case BlockType::ParenthesisLayout:
      return ParenthesisLayout::RenderNode(node, ctx, p, font, expressionColor,
                                           backgroundColor);
    case BlockType::CodePointLayout:
      return CodePointLayout::RenderNode(node, ctx, p, font, expressionColor,
                                         backgroundColor);
    case BlockType::RackLayout:
      return RackLayout::RenderNode(node, ctx, p, font, expressionColor,
                                    backgroundColor);
    default:;
  };
}

int Render::IndexAfterHorizontalCursorMove(const Node* node,
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
