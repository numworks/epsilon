#ifndef ESCHER_EXPRESSION_TABLE_CELL_WITH_MESSAGE_H
#define ESCHER_EXPRESSION_TABLE_CELL_WITH_MESSAGE_H

#include <escher/expression_table_cell.h>
#include <escher/message_text_view.h>
#include <escher/i18n.h>

namespace Escher {
class ExpressionTableCellWithMessage : public ExpressionTableCell {
public:
  ExpressionTableCellWithMessage(Responder * parentResponder = nullptr, I18n::Message message = (I18n::Message)0) :
  ExpressionTableCell(parentResponder),
  m_subLabelView(KDFont::SmallFont, (I18n::Message)0, 0.0f, 0.5f, Palette::GrayDark)
  {}
  View * subLabelView() const override {
    if (strlen(m_subLabelView.text()) == 0) {
      return nullptr;
    }
    return (View *)&m_subLabelView;
  }
  void setHighlighted(bool highlight) override {
    ExpressionTableCell::setHighlighted(highlight);
    KDColor backgroundColor = isHighlighted()? Palette::Select : KDColorWhite;
    m_subLabelView.setBackgroundColor(backgroundColor);
  }
  void setSubLabelMessage(I18n::Message textBody) {
    m_subLabelView.setMessage(textBody);
    reloadCell();
  }
private:
  MessageTextView m_subLabelView;
};

}

#endif
