#include "parenthesis_layout.h"

#include "parser.h"

namespace PoincareJ {

KDSize ParenthesisLayout::Size(const Node* node, const Node* root,
                               KDFont::Size font) {
  KDSize childSize = Render::Size(node->childAtIndex(0), root, font);
  return childSize + KDSize(2 * HorizontalPadding(font), 2 * k_verticalPadding);
}

KDCoordinate ParenthesisLayout::Baseline(const Node* node, const Node* root,
                                         KDFont::Size font) {
  return Render::Baseline(node->childAtIndex(0), root, font) +
         k_verticalPadding;
}

KDPoint ParenthesisLayout::PositionOfChild(const Node* node, int childIndex,
                                           const Node* root,
                                           KDFont::Size font) {
  return KDPoint(HorizontalPadding(font), k_verticalPadding);
}

void ParenthesisLayout::RenderNode(const Node* node, const Node* root,
                                   KDContext* ctx, KDPoint p, KDFont::Size font,
                                   KDColor expressionColor,
                                   KDColor backgroundColor) {
  KDSize size = Size(node, root, font);
  KDCoordinate y = p.y() + Baseline(node, root, font) -
                   (KDFont::GlyphSize(font).height()) / 2;
  KDCoordinate x = p.x();
  ctx->drawString("(", KDPoint(x, y),
                  KDGlyph::Style{.glyphColor = expressionColor,
                                 .backgroundColor = backgroundColor,
                                 .font = font});
  x += size.width() - HorizontalPadding(font);
  ctx->drawString(")", KDPoint(x, y),
                  KDGlyph::Style{.glyphColor = expressionColor,
                                 .backgroundColor = backgroundColor,
                                 .font = font});
}

EditionReference ParenthesisLayout::Parse(const Node* node) {
  return Parser::Parse(node->childAtIndex(0));
}

}  // namespace PoincareJ
