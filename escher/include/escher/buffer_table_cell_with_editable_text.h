#ifndef ESCHER_BUFFER_TABLE_CELL_WITH_EDITABLE_TEXT_H
#define ESCHER_BUFFER_TABLE_CELL_WITH_EDITABLE_TEXT_H

#include <escher/buffer_table_cell.h>
#include <escher/container.h>
#include <escher/editable_text_cell.h>
#include <escher/responder.h>
#include <poincare/print_float.h>

namespace Escher {

class BufferTableCellWithEditableText : public Responder,
                                        public BufferTableCell {
 public:
  BufferTableCellWithEditableText(
      Responder* parentResponder = nullptr,
      InputEventHandlerDelegate* inputEventHandlerDelegate = nullptr,
      TextFieldDelegate* textFieldDelegate = nullptr);
  const View* accessoryView() const override { return &m_textField; }
  TextField* textField() { return &m_textField; }
  const char* editedText() const { return m_textField.text(); }
  void didBecomeFirstResponder() override {
    if (m_editable) {
      Container::activeApp()->setFirstResponder(&m_textField);
    }
  }
  bool isEditing() { return m_textField.isEditing(); }
  void setEditing(bool isEditing) {
    m_textField.setEditing(m_editable && isEditing);
  }
  void setEditable(bool isEditable) { m_editable = isEditable; }
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
  bool m_editable;
  char m_textBody[Poincare::PrintFloat::k_maxFloatCharSize];
};

}  // namespace Escher

#endif
