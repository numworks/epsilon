#ifndef POINCARE_JUNIOR_LAYOUT_RENDER_H
#define POINCARE_JUNIOR_LAYOUT_RENDER_H

#include <kandinsky/context.h>
#include <kandinsky/coordinate.h>
#include <kandinsky/font.h>
#include <kandinsky/point.h>
#include <poincare_junior/src/layout/layout_cursor.h>
#include <poincare_junior/src/layout/layout_selection.h>
#include <poincare_junior/src/memory/tree.h>

namespace PoincareJ {

class Render final {
 public:
  static KDSize Size(const Tree* node);

  static KDCoordinate Height(const Tree* node) { return Size(node).height(); }
  static KDCoordinate Width(const Tree* node) { return Size(node).width(); }

  static KDPoint AbsoluteOrigin(const Tree* node, const Tree* root);
  static KDPoint PositionOfChild(const Tree* node, int childIndex);
  static KDCoordinate Baseline(const Tree* node);
  static void Draw(const Tree* node, KDContext* ctx, KDPoint p,
                   KDFont::Size font, KDColor expressionColor = KDColorBlack,
                   KDColor backgroundColor = KDColorWhite,
                   const LayoutCursor* cursor = nullptr,
                   LayoutSelection selection = {});

 private:
  static void PrivateDraw(const Tree* node, KDContext* ctx, KDPoint p,
                          KDColor expressionColor, KDColor backgroundColor,
                          LayoutSelection selection);
  static void RenderNode(const Tree* node, KDContext* ctx, KDPoint p,
                         KDColor expressionColor, KDColor backgroundColor);

  static KDFont::Size font;
};

}  // namespace PoincareJ

#endif
