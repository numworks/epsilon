#ifndef POINCARE_LAYOUT_RENDER_H
#define POINCARE_LAYOUT_RENDER_H

#include <kandinsky/context.h>
#include <kandinsky/coordinate.h>
#include <kandinsky/font.h>
#include <kandinsky/point.h>
#include <poincare/layout_style.h>
#include <poincare/src/memory/tree.h>

#include "layout_selection.h"
#include "rack.h"

namespace Poincare::Internal {

class Render {
  friend class RackLayout;
  friend class LayoutCursor;
  friend class Grid;
  // Used by render_metrics
  friend KDCoordinate Baseline(const Rack* rack);
  friend KDCoordinate Baseline(const Layout* layout);
  friend KDSize Size(const Rack* rack);

 public:
  static KDSize Size(const Tree* l, KDFont::Size fontSize) {
    s_font = fontSize;
    return Size(static_cast<const Rack*>(l), false);
  }

  static KDCoordinate Baseline(const Tree* l, KDFont::Size fontSize) {
    s_font = fontSize;
    return Baseline(static_cast<const Rack*>(l));
  }

  static KDPoint AbsoluteOrigin(const Tree* l, const Tree* root);
  static void Draw(const Tree* l, KDContext* ctx, KDPoint p,
                   const LayoutStyle& style,
                   const LayoutCursor* cursor = nullptr);

  constexpr static KDCoordinate k_maxLayoutSize = 3 * KDCOORDINATE_MAX / 4;

 private:
  static KDSize Size(const Rack* l, bool showEmpty = true);
  static KDSize Size(const Layout* l);

  static KDCoordinate Height(const Rack* l) { return Size(l).height(); }
  static KDCoordinate Width(const Rack* l, bool showEmpty = true) {
    return Size(l, showEmpty).width();
  }
  static KDCoordinate Height(const Layout* l) { return Size(l).height(); }
  static KDCoordinate Width(const Layout* l) { return Size(l).width(); }

  static KDCoordinate Baseline(const Layout* l);
  // Empty should not change the baseline so no extra argument here
  static KDCoordinate Baseline(const Rack* l);

  static KDPoint PositionOfChild(const Rack* l, int childIndex);
  static KDPoint PositionOfChild(const Layout* l, int childIndex);

  static void DrawSimpleLayout(const Layout* l, KDContext* ctx, KDPoint p,
                               const LayoutStyle& style,
                               LayoutSelection selection);
  static void DrawGridLayout(const Layout* l, KDContext* ctx, KDPoint p,
                             const LayoutStyle& style,
                             LayoutSelection selection);
  static void DrawRack(const Rack* l, KDContext* ctx, KDPoint p,
                       const LayoutStyle& style, LayoutSelection selection,
                       bool showEmpty = true);
  static void RenderNode(const Layout* l, KDContext* ctx, KDPoint p,
                         const LayoutStyle& style);

  static KDFont::Size s_font;
};

}  // namespace Poincare::Internal

#endif
