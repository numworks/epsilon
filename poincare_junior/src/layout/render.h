#ifndef POINCARE_JUNIOR_LAYOUT_RENDER_H
#define POINCARE_JUNIOR_LAYOUT_RENDER_H

#include <kandinsky/context.h>
#include <kandinsky/coordinate.h>
#include <kandinsky/font.h>
#include <kandinsky/point.h>
#include <poincare_junior/src/memory/tree.h>

namespace PoincareJ {

class Render final {
 public:
  static KDSize Size(const Tree* node, KDFont::Size font);
  static KDPoint AbsoluteOrigin(const Tree* node, const Tree* root,
                                KDFont::Size font);
  static KDPoint PositionOfChild(const Tree* node, int childIndex,
                                 KDFont::Size font);
  static KDCoordinate Baseline(const Tree* node, KDFont::Size font);
  static void Draw(const Tree* node, KDContext* ctx, KDPoint p,
                   KDFont::Size font, KDColor expressionColor = KDColorBlack,
                   KDColor backgroundColor = KDColorWhite);

 private:
  static void PrivateDraw(const Tree* node, KDContext* ctx, KDPoint p,
                          KDFont::Size font,
                          KDColor expressionColor = KDColorBlack,
                          KDColor backgroundColor = KDColorWhite);
  static void RenderNode(const Tree* node, KDContext* ctx, KDPoint p,
                         KDFont::Size font,
                         KDColor expressionColor = KDColorBlack,
                         KDColor backgroundColor = KDColorWhite);
};

}  // namespace PoincareJ

#endif
