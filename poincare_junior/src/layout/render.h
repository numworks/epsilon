#ifndef POINCARE_JUNIOR_LAYOUT_RENDER_H
#define POINCARE_JUNIOR_LAYOUT_RENDER_H

#include <kandinsky/context.h>
#include <kandinsky/coordinate.h>
#include <kandinsky/font.h>
#include <kandinsky/point.h>
#include <poincare_junior/src/layout/layout_selection.h>
#include <poincare_junior/src/memory/tree.h>

#include "rack.h"

namespace PoincareJ {

class Render final {
  friend class RackLayout;
  friend class LayoutCursor;
  friend class Grid;

 public:
  static KDSize Size(const Tree* node, KDFont::Size fontSize) {
    s_font = fontSize;
    s_showEmptyRack = false;
    return Size(static_cast<const Rack*>(node));
  }

  static KDCoordinate Baseline(const Tree* node, KDFont::Size fontSize) {
    s_font = fontSize;
    s_showEmptyRack = false;
    return Baseline(static_cast<const Rack*>(node));
  }

  static KDPoint AbsoluteOrigin(const Tree* node, const Tree* root);
  static void Draw(const Tree* node, KDContext* ctx, KDPoint p,
                   KDFont::Size font, KDColor expressionColor = KDColorBlack,
                   KDColor backgroundColor = KDColorWhite,
                   const LayoutCursor* cursor = nullptr);

  // private:
  static KDSize Size(const Rack* node, bool showEmpty = true);
  static KDSize Size(const LayoutT* node);

  static KDCoordinate Height(const Rack* node) { return Size(node).height(); }
  static KDCoordinate Width(const Rack* node) { return Size(node).width(); }
  static KDCoordinate Height(const LayoutT* node) {
    return Size(node).height();
  }
  static KDCoordinate Width(const LayoutT* node) { return Size(node).width(); }

  static KDCoordinate Baseline(const LayoutT* node);
  // Empty should not change the baseline so no extra argument here
  static KDCoordinate Baseline(const Rack* node);

  static KDPoint PositionOfChild(const Rack* node, int childIndex,
                                 bool showEmpty = true);
  static KDPoint PositionOfChild(const LayoutT* node, int childIndex);

  static KDSize SizeAny(const Tree* node) {
    return node->isRackLayout() ? Size(static_cast<const Rack*>(node))
                                : Size(static_cast<const LayoutT*>(node));
  }

  static KDPoint PositionOfChildAny(const Tree* node, int childIndex) {
    return node->isRackLayout()
               ? PositionOfChild(static_cast<const Rack*>(node), childIndex)
               : PositionOfChild(static_cast<const LayoutT*>(node), childIndex);
  }

  static void PrivateDraw(const Tree* node, KDContext* ctx, KDPoint p,
                          KDColor expressionColor, KDColor backgroundColor,
                          LayoutSelection selection);
  static void PrivateDrawSimpleLayout(const LayoutT* node, KDContext* ctx,
                                      KDPoint p, KDColor expressionColor,
                                      KDColor backgroundColor,
                                      LayoutSelection selection);
  static void PrivateDrawGridLayout(const LayoutT* node, KDContext* ctx,
                                    KDPoint p, KDColor expressionColor,
                                    KDColor backgroundColor,
                                    LayoutSelection selection);
  static void PrivateDrawRack(const Rack* node, KDContext* ctx, KDPoint p,
                              KDColor expressionColor, KDColor backgroundColor,
                              LayoutSelection selection);
  static void RenderNode(const LayoutT* node, KDContext* ctx, KDPoint p,
                         KDColor expressionColor, KDColor backgroundColor);

  static KDFont::Size s_font;
  static bool s_showEmptyRack;
};

}  // namespace PoincareJ

#endif
