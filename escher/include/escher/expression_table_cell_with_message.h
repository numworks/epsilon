#ifndef ESCHER_EXPRESSION_TABLE_CELL_WITH_MESSAGE_H
#define ESCHER_EXPRESSION_TABLE_CELL_WITH_MESSAGE_H

#include <escher/expression_table_cell.h>
#include <escher/message_text_view.h>
#include <escher/i18n.h>

namespace Escher {
class ExpressionTableCellWithMessage : public ExpressionTableCell {
public:
  ExpressionTableCellWithMessage(Responder * parentResponder = nullptr, I18n::Message message = (I18n::Message)0);
  View * subLabelView() const override;
  void setHighlighted(bool highlight) override;
  void setSubLabelMessage(I18n::Message textBody);
private:
  MessageTextView m_subLabelView;
};

}

#endif
