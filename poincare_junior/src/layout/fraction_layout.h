#ifndef POINCARE_JUNIOR_FRACTION_LAYOUT_H
#define POINCARE_JUNIOR_FRACTION_LAYOUT_H

#include <escher/metric.h>

#include "../memory/edition_reference.h"
#include "render.h"

namespace PoincareJ {

class FractionLayout {
 public:
  static KDSize Size(const Node* node, KDFont::Size font);
  static KDCoordinate Baseline(const Node* node, KDFont::Size font);
  static KDPoint PositionOfChild(const Node* node, int childIndex,
                                 KDFont::Size font);
  static void RenderNode(const Node* node, KDContext* ctx, KDPoint p,
                         KDFont::Size font,
                         KDColor expressionColor = KDColorBlack,
                         KDColor backgroundColor = KDColorWhite);
  static EditionReference Parse(const Node* node);

 private:
  constexpr static int k_numeratorIndex = 0;
  constexpr static int k_denominatorIndex = 1;
  constexpr static KDCoordinate k_fractionLineMargin = 2;
  constexpr static KDCoordinate k_fractionLineHeight = 1;
  constexpr static KDCoordinate k_horizontalOverflow =
      Escher::Metric::FractionAndConjugateHorizontalOverflow;
  constexpr static KDCoordinate k_horizontalMargin =
      Escher::Metric::FractionAndConjugateHorizontalMargin;
};

}  // namespace PoincareJ

#endif
