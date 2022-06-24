#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_EDITABLE_TEXT_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_EDITABLE_TEXT_H

#include <escher/editable_text_cell.h>
#include <escher/message_table_cell.h>
#include <escher/responder.h>
#include <poincare/print_float.h>

class MessageTableCellWithEditableText : public Responder, public MessageTableCell<> {
public:
  MessageTableCellWithEditableText(Responder * parentResponder = nullptr, InputEventHandlerDelegate * inputEventHandlerDelegate = nullptr, TextFieldDelegate * textFieldDelegate = nullptr, I18n::Message message = (I18n::Message)0);
  View * accessoryView() const override;
  TextField * textField() { return &m_textField; }
  const char * editedText() const;
  void didBecomeFirstResponder() override;
  bool isEditing();
  void setEditing(bool isEditing);
  void setHighlighted(bool highlight) override;
  Responder * responder() override {
    return this;
  }
  const char * text() const override {
    if (!m_textField.isEditing()) {
      return m_textField.text();
    }
    return nullptr;
  }
  void setAccessoryText(const char * text);
  void setTextColor(KDColor color) override;
private:
  void layoutSubviews(bool force = false) override;
  TextField m_textField;
  char m_textBody[Poincare::PrintFloat::k_maxFloatCharSize];
};

#endif
