#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_CHEVRON_AND_EXPRESSION_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_CHEVRON_AND_EXPRESSION_H

#include <escher/message_table_cell_with_chevron.h>
#include <escher/expression_view.h>

class MessageTableCellWithChevronAndExpression : public MessageTableCellWithChevron {
public:
  MessageTableCellWithChevronAndExpression(const I18n::Message *message = &I18n::NullMessage, KDText::FontSize size = KDText::FontSize::Small);
  View * subAccessoryView() const override;
  void setHighlighted(bool highlight) override;
  void setExpression(Poincare::ExpressionLayout * expressionLayout);
private:
  ExpressionView m_subtitleView;
};

#endif
