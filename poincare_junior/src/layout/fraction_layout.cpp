#include "fraction_layout.h"

#include "parser.h"

namespace PoincareJ {

KDSize FractionLayout::Size(const Node* node, const Node* root,
                            KDFont::Size font) {
  KDSize numeratorSize = Render::Size(node->childAtIndex(0), root, font);
  KDSize denominatorSize = Render::Size(node->childAtIndex(1), root, font);
  KDCoordinate width =
      std::max(numeratorSize.width(), denominatorSize.width()) +
      2 * (k_horizontalOverflow + k_horizontalMargin);
  KDCoordinate height = numeratorSize.height() + k_fractionLineMargin +
                        k_fractionLineHeight + k_fractionLineMargin +
                        denominatorSize.height();
  return KDSize(width, height);
}

KDCoordinate FractionLayout::Baseline(const Node* node, const Node* root,
                                      KDFont::Size font) {
  return Render::Size(node->childAtIndex(k_numeratorIndex), root, font)
             .height() +
         k_fractionLineMargin + k_fractionLineHeight;
}

KDPoint FractionLayout::PositionOfChild(const Node* node, int childIndex,
                                        const Node* root, KDFont::Size font) {
  KDCoordinate x =
      (Render::Size(node, root, font).width() -
       Render::Size(node->childAtIndex(childIndex), root, font).width()) /
      2;
  KDCoordinate y =
      (childIndex == k_denominatorIndex)
          ? Render::Size(node->childAtIndex(k_numeratorIndex), root, font)
                    .height() +
                2 * k_fractionLineMargin + k_fractionLineHeight
          : 0;
  return KDPoint(x, y);
}

void FractionLayout::RenderNode(const Node* node, const Node* root,
                                KDContext* ctx, KDPoint p, KDFont::Size font,
                                KDColor expressionColor,
                                KDColor backgroundColor) {
  KDCoordinate fractionLineY =
      p.y() +
      Render::Size(node->childAtIndex(k_numeratorIndex), root, font).height() +
      k_fractionLineMargin;
  ctx->fillRect(
      KDRect(p.x() + k_horizontalMargin, fractionLineY,
             Render::Size(node, root, font).width() - 2 * k_horizontalMargin,
             k_fractionLineHeight),
      expressionColor);
}

EditionReference FractionLayout::Parse(const Node* node) {
  EditionReference ref =
      EditionPool::sharedEditionPool()->push<BlockType::Division>();
  Parser::Parse(node->childAtIndex(k_numeratorIndex));
  Parser::Parse(node->childAtIndex(k_denominatorIndex));
  return ref;
}

}  // namespace PoincareJ
