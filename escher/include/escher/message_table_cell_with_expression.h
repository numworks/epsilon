#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_EXPRESSION_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_EXPRESSION_H

#include <escher/expression_view.h>
#include <escher/message_table_cell.h>

namespace Escher {

class MessageTableCellWithExpression : public MessageTableCell {
 public:
  MessageTableCellWithExpression(I18n::Message message = (I18n::Message)0);
  const View* subLabelView() const override { return &m_subtitleView; }
  void setHighlighted(bool highlight) override;
  void setLayout(Poincare::Layout layout);
  void setFont(KDFont::Size font) { m_subtitleView.setFont(font); }

 private:
  ExpressionView m_subtitleView;
};

}  // namespace Escher

#endif
