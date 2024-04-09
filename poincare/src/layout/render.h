#ifndef POINCARE_LAYOUT_RENDER_H
#define POINCARE_LAYOUT_RENDER_H

#include <kandinsky/context.h>
#include <kandinsky/coordinate.h>
#include <kandinsky/font.h>
#include <kandinsky/point.h>
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
  static KDSize Size(const Tree* node, KDFont::Size fontSize) {
    s_font = fontSize;
    return Size(static_cast<const Rack*>(node), false);
  }

  static KDCoordinate Baseline(const Tree* node, KDFont::Size fontSize) {
    s_font = fontSize;
    return Baseline(static_cast<const Rack*>(node));
  }

  static KDPoint AbsoluteOrigin(const Tree* node, const Tree* root);
  static void Draw(const Tree* node, KDContext* ctx, KDPoint p,
                   KDFont::Size font, KDColor expressionColor = KDColorBlack,
                   KDColor backgroundColor = KDColorWhite,
                   const LayoutCursor* cursor = nullptr);

 private:
  static KDSize Size(const Rack* node, bool showEmpty = true);
  static KDSize Size(const Layout* node);

  static KDCoordinate Height(const Rack* node) { return Size(node).height(); }
  static KDCoordinate Width(const Rack* node, bool showEmpty = true) {
    return Size(node, showEmpty).width();
  }
  static KDCoordinate Height(const Layout* node) { return Size(node).height(); }
  static KDCoordinate Width(const Layout* node) { return Size(node).width(); }

  static KDCoordinate Baseline(const Layout* node);
  // Empty should not change the baseline so no extra argument here
  static KDCoordinate Baseline(const Rack* node);

  static KDPoint PositionOfChild(const Rack* node, int childIndex);
  static KDPoint PositionOfChild(const Layout* node, int childIndex);

  static void DrawSimpleLayout(const Layout* node, KDContext* ctx, KDPoint p,
                               const KDGlyph::Style& style,
                               LayoutSelection selection);
  static void DrawGridLayout(const Layout* node, KDContext* ctx, KDPoint p,
                             const KDGlyph::Style& style,
                             LayoutSelection selection);
  static void DrawRack(const Rack* node, KDContext* ctx, KDPoint p,
                       const KDGlyph::Style& style, LayoutSelection selection,
                       bool showEmpty = true);
  static void RenderNode(const Layout* node, KDContext* ctx, KDPoint p,
                         const KDGlyph::Style& style);

  static KDFont::Size s_font;
};

}  // namespace Poincare::Internal

#endif
