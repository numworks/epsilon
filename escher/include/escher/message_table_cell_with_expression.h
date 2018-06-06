#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_EXPRESSION_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_EXPRESSION_H

#include <escher/message_table_cell.h>
#include <escher/expression_view.h>

class MessageTableCellWithExpression : public MessageTableCell {
public:
  MessageTableCellWithExpression(I18n::Message message = (I18n::Message)0, KDText::FontSize size = KDText::FontSize::Small);
  View * subAccessoryView() const override;
  void setHighlighted(bool highlight) override;
  void setExpressionLayout(Poincare::ExpressionLayout * expressionLayout);
private:
  ExpressionView m_subtitleView;
};

#endif
