#ifndef CALCULATION_JUNIOR_LAYOUT_VIEW_H
#define CALCULATION_JUNIOR_LAYOUT_VIEW_H

#include <escher/glyphs_view.h>
#include <kandinsky/color.h>
#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/layout/layout_cursor.h>
#include <poincare_junior/src/layout/layout_selection.h>

namespace CalculationJunior {

class AbstractLayoutView : public Escher::GlyphsView {
 public:
  AbstractLayoutView(KDGlyph::Format format = {})
      : Escher::GlyphsView(format), m_horizontalMargin(0) {}

  virtual PoincareJ::Layout getLayout() const = 0;
  bool setLayout(PoincareJ::Layout layoutR);
  void drawRect(KDContext* ctx, KDRect rect) const override;

  void setHorizontalMargin(KDCoordinate horizontalMargin) {
    m_horizontalMargin = horizontalMargin;
  }
  int numberOfLayouts() const { return getLayout().treeSize(); }
  KDSize minimalSizeForOptimalDisplay() const override;
  KDPoint drawingOrigin() const;
  bool layoutHasNode() const { return !getLayout().isUninitialized(); }

 protected:
  virtual void privateSetLayout(PoincareJ::Layout layoutR) = 0;

 private:
  virtual PoincareJ::LayoutSelection selection() const {
    return PoincareJ::LayoutSelection();
  }
  KDCoordinate m_horizontalMargin;
};

class LayoutView : public AbstractLayoutView {
 public:
  PoincareJ::Layout getLayout() const override { return m_layout; }

 private:
  void privateSetLayout(PoincareJ::Layout layoutR) override {
    m_layout = layoutR;
  }

  mutable PoincareJ::Layout m_layout;
};

class LayoutViewWithCursor : public AbstractLayoutView {
 public:
  LayoutViewWithCursor(PoincareJ::LayoutBufferCursor* cursor,
                       KDGlyph::Format format = {})
      : AbstractLayoutView(format), m_cursor(cursor) {
    assert(cursor);
  }

  PoincareJ::Layout getLayout() const override {
    return PoincareJ::Layout(m_cursor->rootNode());
  }

 private:
  void privateSetLayout(PoincareJ::Layout layoutR) override {
    layoutR.dumpAt(m_cursor->layoutBuffer());
  }
  PoincareJ::LayoutSelection selection() const override {
    return m_cursor->selection();
  }
  PoincareJ::LayoutBufferCursor* m_cursor;
};

}  // namespace CalculationJunior

#endif
