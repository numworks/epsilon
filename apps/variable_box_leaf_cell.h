#ifndef APPS_VARIABLE_BOX_LEAF_CELL_H
#define APPS_VARIABLE_BOX_LEAF_CELL_H

#include <escher.h>
#include <poincare.h>

class VariableBoxLeafCell final : public HighlightCell {
public:
  VariableBoxLeafCell();
  void displayExpression(bool displayExpression) {
    m_displayExpression = displayExpression;
  }
  void reloadCell() override;
  void setLabel(const char * text) { m_labelView.setText(text); }
  void setSubtitle(const char * text);
  void setExpressionLayout(Poincare::ExpressionLayout * expressionLayout) {
    m_expressionView.setExpressionLayout(expressionLayout);
  }
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  constexpr static KDCoordinate k_separatorThickness = 1;
  constexpr static KDCoordinate k_widthMargin = 10;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  BufferTextView m_labelView;
  BufferTextView m_subtitleView;
  ExpressionView m_expressionView;
  bool m_displayExpression;
};

#endif
