#ifndef ESCHER_EVEN_ODD_EXPRESSION_CELL_H
#define ESCHER_EVEN_ODD_EXPRESSION_CELL_H

#include <escher/even_odd_cell.h>
#include <escher/expression_view.h>

class EvenOddExpressionCell : public EvenOddCell {
public:
  EvenOddExpressionCell(float horizontalAlignment = 0.0f, float verticalAlignment = 0.5f,
    KDColor textColor = Palette::PrimaryText, KDColor backgroundColor = Palette::BackgroundHard);
  void setEven(bool even) override;
  void setHighlighted(bool highlight) override;
  void setLayout(Poincare::Layout layout);
  void setBackgroundColor(KDColor backgroundColor);
  void setTextColor(KDColor textColor);
  KDSize minimalSizeForOptimalDisplay() const override;
  void setAlignment(float horizontalAlignment, float verticalAlignment) { m_expressionView.setAlignment(horizontalAlignment, verticalAlignment); }
  void setLeftMargin(KDCoordinate margin);
  void setRightMargin(KDCoordinate margin);
  KDPoint drawingOrigin() const { return m_expressionView.drawingOrigin(); }
  Poincare::Layout layout() const override { return m_expressionView.layout(); }
  void drawRect(KDContext * ctx, KDRect rect) const override;
protected:
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  ExpressionView m_expressionView;
  KDCoordinate m_leftMargin;
  KDCoordinate m_rightMargin;
};

#endif
