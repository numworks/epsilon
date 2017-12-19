#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_EDITABLE_TEXT_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_EDITABLE_TEXT_H

#include <escher/editable_text_cell.h>
#include <escher/message_table_cell.h>
#include <escher/responder.h>

class MessageTableCellWithEditableText : public Responder, public MessageTableCell {
public:
  MessageTableCellWithEditableText(Responder * parentResponder, TextFieldDelegate * textFieldDelegate, char * draftTextBuffer, I18n::Message message = (I18n::Message)0);
  View * accessoryView() const override;
  const char * editedText() const;
  void didBecomeFirstResponder() override;
  bool isEditing();
  void setEditing(bool isEditing);
  void setHighlighted(bool highlight) override;
  Responder * responder() override {
    return this;
  }
  void setAccessoryText(const char * text);
  void setTextColor(KDColor color) override;
  constexpr static int k_bufferLength = TextField::maxBufferSize();
private:
  void layoutSubviews() override;
  constexpr static int k_maxNumberOfEditableCharacters = 14;
  TextField m_textField;
  char m_textBody[k_bufferLength];
};

#endif
