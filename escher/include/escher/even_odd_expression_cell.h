#ifndef ESCHER_EVEN_ODD_EXPRESSION_CELL_H
#define ESCHER_EVEN_ODD_EXPRESSION_CELL_H

#include <escher/even_odd_cell.h>
#include <escher/expression_view.h>

class EvenOddExpressionCell : public EvenOddCell {
public:
  EvenOddExpressionCell(float horizontalAlignment = 0.0f, float verticalAlignment = 0.5f,
    KDColor textColor = KDColorBlack, KDColor backgroundColor = KDColorWhite);
  void setEven(bool even) override;
  void setHighlighted(bool highlight) override;
  void setExpressionLayout(Poincare::ExpressionLayout * expressionLayout);
  void setBackgroundColor(KDColor backgroundColor);
  void setTextColor(KDColor textColor);
  KDSize minimalSizeForOptimalDisplay() const override;
  void setAlignment(float horizontalAlignment, float verticalAlignment);
  Poincare::ExpressionLayout * expressionLayout() const override { return m_expressionView.expressionLayout(); }
protected:
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  ExpressionView m_expressionView;
};

#endif
