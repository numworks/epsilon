#ifndef CALCULATION_JUNIOR_EXPRESSION_VIEW_H
#define CALCULATION_JUNIOR_EXPRESSION_VIEW_H

#include <escher/glyphs_view.h>
#include <kandinsky/color.h>
#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/layout/layout_cursor.h>
#include <poincare_junior/src/layout/layout_selection.h>

// TODO : Rename this class LayoutView

namespace CalculationJunior {

class AbstractExpressionView : public Escher::GlyphsView {
 public:
  AbstractExpressionView(KDGlyph::Format format = {})
      : Escher::GlyphsView(format), m_horizontalMargin(0) {}

  virtual PoincareJ::Layout getLayout() const = 0;
  virtual bool setLayout(PoincareJ::Layout) = 0;
  void drawRect(KDContext* ctx, KDRect rect) const override;

  void setHorizontalMargin(KDCoordinate horizontalMargin) {
    m_horizontalMargin = horizontalMargin;
  }
  int numberOfLayouts() const { return getLayout().treeSize(); }
  KDSize minimalSizeForOptimalDisplay() const override;
  KDPoint drawingOrigin() const;
  bool layoutHasNode() const { return getLayout().isInitialized(); }

 private:
  virtual PoincareJ::LayoutSelection selection() const {
    return PoincareJ::LayoutSelection();
  }
  KDCoordinate m_horizontalMargin;
};

class ExpressionView : public AbstractExpressionView {
 public:
  PoincareJ::Layout getLayout() const override { return m_layout; }
  bool setLayout(PoincareJ::Layout layout) override;

 private:
  mutable PoincareJ::Layout m_layout;
};

class ExpressionViewWithCursor : public AbstractExpressionView {
 public:
  ExpressionViewWithCursor(PoincareJ::LayoutCursor* cursor,
                           KDGlyph::Format format = {})
      : AbstractExpressionView(format), m_cursor(cursor) {
    assert(cursor);
  }

  PoincareJ::Layout getLayout() const override {
    return PoincareJ::Layout(m_cursor->layoutBuffer());
  }
  bool setLayout(PoincareJ::Layout layout) override;

 private:
  PoincareJ::LayoutSelection selection() const override {
    return m_cursor->selection();
  }
  PoincareJ::LayoutCursor* m_cursor;
};

}  // namespace CalculationJunior

#endif
