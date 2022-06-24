#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_CHEVRON_AND_EXPRESSION_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_CHEVRON_AND_EXPRESSION_H

#include <escher/message_table_cell_with_chevron.h>
#include <escher/expression_view.h>

class MessageTableCellWithChevronAndExpression : public MessageTableCellWithChevron<> {
public:
  MessageTableCellWithChevronAndExpression(I18n::Message message = (I18n::Message)0, const KDFont * font = KDFont::SmallFont);
  View * subAccessoryView() const override;
  void setHighlighted(bool highlight) override;
  void setLayout(Poincare::Layout layoutR);
private:
  ExpressionView m_subtitleView;
};

#endif
