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

class Render {
  friend class RackLayout;
  friend class LayoutCursor;
  friend class Grid;
  // Used by render_metrics
  friend KDCoordinate Baseline(const Rack* rack);
  friend KDCoordinate Baseline(const LayoutT* layout);
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
  static KDSize Size(const LayoutT* node);

  static KDCoordinate Height(const Rack* node) { return Size(node).height(); }
  static KDCoordinate Width(const Rack* node, bool showEmpty = true) {
    return Size(node, showEmpty).width();
  }
  static KDCoordinate Height(const LayoutT* node) {
    return Size(node).height();
  }
  static KDCoordinate Width(const LayoutT* node) { return Size(node).width(); }

  static KDCoordinate Baseline(const LayoutT* node);
  // Empty should not change the baseline so no extra argument here
  static KDCoordinate Baseline(const Rack* node);

  static KDPoint PositionOfChild(const Rack* node, int childIndex);
  static KDPoint PositionOfChild(const LayoutT* node, int childIndex);

  static void DrawSimpleLayout(const LayoutT* node, KDContext* ctx, KDPoint p,
                               const KDGlyph::Style& style,
                               LayoutSelection selection);
  static void DrawGridLayout(const LayoutT* node, KDContext* ctx, KDPoint p,
                             const KDGlyph::Style& style,
                             LayoutSelection selection);
  static void DrawRack(const Rack* node, KDContext* ctx, KDPoint p,
                       const KDGlyph::Style& style, LayoutSelection selection,
                       bool showEmpty = true);
  static void RenderNode(const LayoutT* node, KDContext* ctx, KDPoint p,
                         const KDGlyph::Style& style);

  static KDFont::Size s_font;
};

}  // namespace PoincareJ

#endif
