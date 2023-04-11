#ifndef CALCULATION_JUNIOR_EXPRESSION_VIEW_H
#define CALCULATION_JUNIOR_EXPRESSION_VIEW_H

#include <escher/glyphs_view.h>
#include <kandinsky/color.h>
#include <poincare_junior/include/layout.h>

#include "layout_selection.h"

// TODO : Rename this class LayoutView

namespace CalculationJunior {

class ExpressionView : public Escher::GlyphsView {
 public:
  ExpressionView(KDGlyph::Format format = {})
      : Escher::GlyphsView(format), m_horizontalMargin(0) {}

  PoincareJ::Layout* getLayout() const { return &m_layout; }
  bool setLayout(PoincareJ::Layout layout);
  void drawRect(KDContext* ctx, KDRect rect) const override;

  void setHorizontalMargin(KDCoordinate horizontalMargin) {
    m_horizontalMargin = horizontalMargin;
  }
  int numberOfLayouts() const { return m_layout.treeSize(); }
  KDSize minimalSizeForOptimalDisplay() const override;
  KDPoint drawingOrigin() const;
  bool layoutHasNode() const { return m_layout.isInitialized(); }

 protected:
  // TODO find better way to have minimalSizeForOptimalDisplay const
  mutable PoincareJ::Layout m_layout;

 private:
  virtual LayoutSelection selection() const { return LayoutSelection(); }

  KDCoordinate m_horizontalMargin;
};

#if 0
#include "layout_cursor.h"

class ExpressionViewWithCursor : public ExpressionView {
 public:
  ExpressionViewWithCursor(Poincare::LayoutCursor* cursor,
                           KDGlyph::Format format = {})
      : ExpressionView(format), m_cursor(cursor) {
    assert(cursor);
  }

 private:
  LayoutSelection selection() const override {
    return m_cursor->selection();
  }
  Poincare::LayoutCursor* m_cursor;
};

#endif

}  // namespace CalculationJunior

#endif
