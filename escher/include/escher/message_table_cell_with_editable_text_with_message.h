#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_EDITABLE_TEXT_WITH_MESSAGE_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_EDITABLE_TEXT_WITH_MESSAGE_H

#include <escher/message_table_cell_with_editable_text.h>

namespace Escher {

class MessageTableCellWithEditableTextWithMessage : public MessageTableCellWithEditableText {
public:
  MessageTableCellWithEditableTextWithMessage(Responder * parentResponder = nullptr, InputEventHandlerDelegate * inputEventHandlerDelegate = nullptr, TextFieldDelegate * textFieldDelegate = nullptr, I18n::Message message = (I18n::Message)0) :
    MessageTableCellWithEditableText(parentResponder,inputEventHandlerDelegate, textFieldDelegate, message),
    m_subLabelView(KDFont::SmallFont, (I18n::Message)0, 0.0f, 0.5f, Palette::GrayDark) {}
  const View * subLabelView() const override { return m_subLabelView.text()[0] == 0 ? nullptr : &m_subLabelView; }
  void setHighlighted(bool highlight) override;
  void setSubLabelMessage(I18n::Message textBody);
protected:
  MessageTextView m_subLabelView;
};

}

#endif
