#ifndef POINCARE_LAYOUT_RENDER_H
#define POINCARE_LAYOUT_RENDER_H

#include <poincare_junior/src/memory/node.h>
#include <kandinsky/font.h>
#include <kandinsky/point.h>
#include <kandinsky/coordinate.h>
#include <kandinsky/context.h>

namespace PoincareJ {

class Render final {
public:
  static KDSize Size(const Node node, KDFont::Size font);
  static KDPoint AbsoluteOrigin(const Node node, KDFont::Size font);
  static KDPoint PositionOfChild(const Node node, int childIndex, KDFont::Size font);
  static KDCoordinate Baseline(const Node node, KDFont::Size font);
  static void Draw(const Node node, KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor = KDColorBlack, KDColor backgroundColor = KDColorWhite);
  static void RenderNode(const Node node, KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor = KDColorBlack, KDColor backgroundColor = KDColorWhite);
};

}

#endif
