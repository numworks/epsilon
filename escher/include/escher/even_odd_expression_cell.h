#ifndef ESCHER_EVEN_ODD_EXPRESSION_CELL_H
#define ESCHER_EVEN_ODD_EXPRESSION_CELL_H

#include <escher/even_odd_cell.h>
#include <escher/expression_view.h>

class EvenOddExpressionCell final : public EvenOddCell {
public:
  EvenOddExpressionCell(float horizontalAlignment = 0.0f, float verticalAlignment = 0.5f,
    KDColor textColor = KDColorBlack, KDColor backgroundColor = KDColorWhite);
  void setEven(bool even) override;
  void setHighlighted(bool highlight) override;
  void setExpressionLayout(Poincare::ExpressionLayout * expressionLayout) { m_expressionView.setExpressionLayout(expressionLayout); }
  void setBackgroundColor(KDColor backgroundColor) { m_expressionView.setBackgroundColor(backgroundColor); }
  void setTextColor(KDColor textColor) { m_expressionView.setTextColor(textColor); }
  KDSize minimalSizeForOptimalDisplay() const override;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
protected:
  ExpressionView m_expressionView;
};

#endif
