#ifndef POINCARE_JUNIOR_PARENTHESIS_LAYOUT_H
#define POINCARE_JUNIOR_PARENTHESIS_LAYOUT_H

#include "../memory/edition_reference.h"
#include "render.h"

namespace PoincareJ {

// This is a simplified ParenthesisLayout, with made-up padding and rendering.
class ParenthesisLayout {
 public:
  static KDSize Size(const Node* node, const Node* root, KDFont::Size font);
  static KDCoordinate Baseline(const Node* node, const Node* root,
                               KDFont::Size font);
  static KDPoint PositionOfChild(const Node* node, int childIndex,
                                 const Node* root, KDFont::Size font);
  static void RenderNode(const Node* node, const Node* root, KDContext* ctx,
                         KDPoint p, KDFont::Size font,
                         KDColor expressionColor = KDColorBlack,
                         KDColor backgroundColor = KDColorWhite);
  static EditionReference Parse(const Node* node);

 private:
  constexpr static KDCoordinate k_verticalPadding = 2;
  constexpr static KDCoordinate HorizontalPadding(KDFont::Size font) {
    return KDFont::GlyphSize(font).width();
  }
};

}  // namespace PoincareJ

#endif
