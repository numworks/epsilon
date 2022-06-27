#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_EXPRESSION_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_EXPRESSION_H

#include <escher/message_table_cell.h>
#include <escher/expression_view.h>

namespace Escher {

class MessageTableCellWithExpression : public MessageTableCell {
public:
  MessageTableCellWithExpression(I18n::Message message = (I18n::Message)0);
  const View * subLabelView() const override { return &m_subtitleView; }
  void setHighlighted(bool highlight) override;
  void setLayout(Poincare::Layout layout);
  void setFont(const KDFont * font) { m_subtitleView.setFont(font); }
private:
  ExpressionView m_subtitleView;
};

}

#endif
