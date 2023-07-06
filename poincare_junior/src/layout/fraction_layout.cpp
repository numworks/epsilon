#include "fraction_layout.h"

#include "parser.h"

namespace PoincareJ {

KDSize FractionLayout::Size(const Tree* node, const Tree* root,
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

KDCoordinate FractionLayout::Baseline(const Tree* node, const Tree* root,
                                      KDFont::Size font) {
  return Render::Size(node->childAtIndex(k_numeratorIndex), root, font)
             .height() +
         k_fractionLineMargin + k_fractionLineHeight;
}

KDPoint FractionLayout::PositionOfChild(const Tree* node, int childIndex,
                                        const Tree* root, KDFont::Size font) {
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

void FractionLayout::RenderNode(const Tree* node, const Tree* root,
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

EditionReference FractionLayout::Parse(const Tree* node) {
  EditionReference ref = editionPool->push<BlockType::Division>();
  Parser::Parse(node->childAtIndex(k_numeratorIndex));
  Parser::Parse(node->childAtIndex(k_denominatorIndex));
  return ref;
}

}  // namespace PoincareJ
