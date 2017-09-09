#ifndef ESCHER_EXPRESSION_TABLE_CELL_WITH_POINTER_H
#define ESCHER_EXPRESSION_TABLE_CELL_WITH_POINTER_H

#include <escher/expression_table_cell.h>
#include <escher/i18n.h>
#include <escher/message_text_view.h>

class ExpressionTableCellWithPointer : public ExpressionTableCell {
public:
  ExpressionTableCellWithPointer(const I18n::Message *accessoryMessage = &I18n::NullMessage, Layout layout = Layout::Horizontal);
  View * accessoryView() const override;
  void setHighlighted(bool highlight) override;
  void setAccessoryMessage(const I18n::Message *messageBody);
private:
  MessageTextView m_accessoryView;
};

#endif
