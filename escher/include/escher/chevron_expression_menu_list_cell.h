#ifndef ESCHER_CHEVRON_EXPRESSION_MENU_LIST_CELL_H
#define ESCHER_CHEVRON_EXPRESSION_MENU_LIST_CELL_H

#include <escher/chevron_menu_list_cell.h>
#include <escher/expression_view.h>

class ChevronExpressionMenuListCell : public ChevronMenuListCell {
public:
  ChevronExpressionMenuListCell(char * label = nullptr);
  void setHighlighted(bool highlight) override;
  void setExpression(Poincare::ExpressionLayout * expressionLayout);
private:
  static constexpr KDCoordinate k_margin = 8;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  ExpressionView m_subtitleView;
};

#endif
