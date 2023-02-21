#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_CHEVRON_AND_EXPRESSION_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_CHEVRON_AND_EXPRESSION_H

#include <escher/expression_view.h>
#include <escher/message_table_cell_with_chevron.h>

namespace Escher {

class MessageTableCellWithChevronAndExpression
    : public MessageTableCellWithChevron {
 public:
  MessageTableCellWithChevronAndExpression(
      I18n::Message message = (I18n::Message)0);
  const View* subLabelView() const override { return &m_subtitleView; }
  void setHighlighted(bool highlight) override;
  void setLayout(Poincare::Layout layoutR);

 private:
  ExpressionView m_subtitleView;
};

}  // namespace Escher

#endif
