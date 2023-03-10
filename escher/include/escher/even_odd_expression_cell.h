#ifndef ESCHER_EVEN_ODD_EXPRESSION_CELL_H
#define ESCHER_EVEN_ODD_EXPRESSION_CELL_H

#include <escher/even_odd_cell.h>
#include <escher/expression_view.h>

namespace Escher {

class EvenOddExpressionCell : public EvenOddCell {
 public:
  EvenOddExpressionCell(float horizontalAlignment = KDGlyph::k_alignLeft,
                        float verticalAlignment = KDGlyph::k_alignCenter,
                        KDColor textColor = KDColorBlack,
                        KDColor backgroundColor = KDColorWhite,
                        KDFont::Size font = KDFont::Size::Large);
  void setLayout(Poincare::Layout layout);
  void setBackgroundColor(KDColor backgroundColor);
  void setTextColor(KDColor textColor);
  void setFont(KDFont::Size font) { m_expressionView.setFont(font); }
  KDSize minimalSizeForOptimalDisplay() const override;
  void setAlignment(float horizontalAlignment, float verticalAlignment) {
    m_expressionView.setAlignment(horizontalAlignment, verticalAlignment);
  }
  void setLeftMargin(KDCoordinate margin);
  void setRightMargin(KDCoordinate margin);
  KDPoint drawingOrigin() const { return m_expressionView.drawingOrigin(); }
  Poincare::Layout layout() const override { return m_expressionView.layout(); }
  KDFont::Size font() const { return m_expressionView.font(); }
  void drawRect(KDContext* ctx, KDRect rect) const override;

 protected:
  void updateSubviewsBackgroundAfterChangingState() override;
  int numberOfSubviews() const override;
  View* subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  ExpressionView m_expressionView;
  KDCoordinate m_leftMargin;
  KDCoordinate m_rightMargin;
};

}  // namespace Escher

#endif
