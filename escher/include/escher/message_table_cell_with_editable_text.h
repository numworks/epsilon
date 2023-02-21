#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_EDITABLE_TEXT_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_EDITABLE_TEXT_H

#include <escher/container.h>
#include <escher/editable_text_cell.h>
#include <escher/message_table_cell.h>
#include <escher/responder.h>
#include <poincare/print_float.h>

namespace Escher {

class MessageTableCellWithEditableText : public Responder,
                                         public MessageTableCell {
 public:
  MessageTableCellWithEditableText(
      Responder* parentResponder = nullptr,
      InputEventHandlerDelegate* inputEventHandlerDelegate = nullptr,
      TextFieldDelegate* textFieldDelegate = nullptr,
      I18n::Message message = (I18n::Message)0);
  const View* accessoryView() const override { return &m_textField; }
  TextField* textField() { return &m_textField; }
  const char* editedText() const { return m_textField.text(); }
  void didBecomeFirstResponder() override {
    Container::activeApp()->setFirstResponder(&m_textField);
  }
  bool isEditing() { return m_textField.isEditing(); }
  void setEditing(bool isEditing) { m_textField.setEditing(isEditing); }
  void setHighlighted(bool highlight) override;
  Responder* responder() override { return this; }
  const char* text() const override {
    return !m_textField.isEditing() ? m_textField.text() : nullptr;
  }
  void setAccessoryText(const char* text);
  bool shouldAlignSublabelRight() const override { return false; }
  bool forceAlignLabelAndAccessory() const override { return true; }
  bool giveAccessoryAllWidth() const override { return true; }
  KDCoordinate accessoryMinimalWidthOverridden() const override {
    return Poincare::PrintFloat::glyphLengthForFloatWithPrecision(
               Poincare::Preferences::ShortNumberOfSignificantDigits + 1) *
               KDFont::GlyphWidth(m_textField.font()) +
           TextCursorView::k_width;
  }
  virtual void setDelegates(
      InputEventHandlerDelegate* inputEventHandlerDelegate,
      TextFieldDelegate* textFieldDelegate);

 private:
  TextField m_textField;
  char m_textBody[Poincare::PrintFloat::k_maxFloatCharSize];
};

}  // namespace Escher

#endif
