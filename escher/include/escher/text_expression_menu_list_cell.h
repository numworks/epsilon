#ifndef ESCHER_EXPRESSION_BUFFER_MENU_LIST_CELL_H
#define ESCHER_EXPRESSION_BUFFER_MENU_LIST_CELL_H

#include <escher/view.h>
#include <escher/expression_view.h>
#include <escher/pointer_text_view.h>
#include <escher/palette.h>
#include <escher/metric.h>
#include <escher/table_view_cell.h>

class TextExpressionMenuListCell : public TableViewCell {
public:
  TextExpressionMenuListCell(char * accessoryText = nullptr);
  void setHighlighted(bool highlight) override;
  void setExpression(Poincare::ExpressionLayout * expressionLayout);
  void setAccessoryText(const char * textBody);
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  constexpr static KDCoordinate k_separatorThickness = 1;
  ExpressionView m_labelExpressionView;
  PointerTextView m_accessoryView;
};

#endif
