#ifndef ESCHER_EXPRESSION_MENU_LIST_CELL_H
#define ESCHER_EXPRESSION_MENU_LIST_CELL_H

#include <escher/view.h>
#include <escher/expression_view.h>
#include <escher/palette.h>
#include <escher/metric.h>
#include <escher/table_view_cell.h>

class ExpressionMenuListCell : public TableViewCell {
public:
  ExpressionMenuListCell();
  void setHighlighted(bool highlight) override;
  void setExpression(Poincare::ExpressionLayout * expressionLayout);
  void drawRect(KDContext * ctx, KDRect rect) const override;
protected:
  constexpr static KDCoordinate k_separatorThickness = 1;
  ExpressionView m_labelExpressionView;
private:
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
};

#endif
