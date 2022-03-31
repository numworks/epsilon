#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_EXPRESSION_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_EXPRESSION_H

#include <escher/message_table_cell.h>
#include <escher/expression_view.h>

class MessageTableCellWithExpression : public MessageTableCell {
public:
  MessageTableCellWithExpression(I18n::Message message = (I18n::Message)0, const KDFont * font = KDFont::LargeFont);
  View * accessoryView() const override;
  void setHighlighted(bool highlight) override;
  void setLayout(Poincare::Layout layout);
private:
  ExpressionView m_subtitleView;
};

#endif
