#ifndef ESCHER_EXPRESSION_TABLE_CELL_WITH_POINTER_H
#define ESCHER_EXPRESSION_TABLE_CELL_WITH_POINTER_H

#include <escher/expression_table_cell.h>
#include <escher/i18n.h>
#include <escher/message_text_view.h>

namespace Escher {

class ExpressionTableCellWithPointer : public ExpressionTableCell {
public:
  ExpressionTableCellWithPointer(Responder * responder = nullptr, I18n::Message subLabelMessage = (I18n::Message)0);
  const View * subLabelView() const override { return &m_subLabelView; }
  void setHighlighted(bool highlight) override;
  void setSubLabelMessage(I18n::Message messageBody);
private:
  MessageTextView m_subLabelView;
};

}

#endif
