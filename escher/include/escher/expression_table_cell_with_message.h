#ifndef ESCHER_EXPRESSION_TABLE_CELL_WITH_MESSAGE_H
#define ESCHER_EXPRESSION_TABLE_CELL_WITH_MESSAGE_H

#include <escher/expression_table_cell.h>
#include <escher/i18n.h>
#include <escher/message_text_view.h>

namespace Escher {

class ExpressionTableCellWithMessage : public ExpressionTableCell {
 public:
  ExpressionTableCellWithMessage(
      Responder* responder = nullptr,
      I18n::Message subLabelMessage = (I18n::Message)0,
      KDFont::Size font = KDFont::Size::Large);
  const View* subLabelView() const override { return &m_subLabelView; }
  void setHighlighted(bool highlight) override;
  void setSubLabelMessage(I18n::Message messageBody);

 private:
  MessageTextView m_subLabelView;
};

}  // namespace Escher

#endif
