#include <escher/editable_text_cell.h>
#include <escher/app.h>
#include <escher/palette.h>
#include <assert.h>

EditableTextCell::EditableTextCell(Responder * parentResponder, TextFieldDelegate * delegate) :
  TableViewCell(),
  Responder(parentResponder),
  m_textField(TextField(this, m_textBody, m_draftTextBody, 255, delegate))
{
}

void EditableTextCell::setDelegate(TextFieldDelegate * delegate) {
  m_textField.setTextFieldDelegate(delegate);
}

void EditableTextCell::reloadCell() {
  TableViewCell::reloadCell();
  KDColor backgroundColor = isHighlighted()? Palette::FocusCellBackgroundColor : Palette::CellBackgroundColor;
  m_textField.setBackgroundColor(backgroundColor);
}

const char * EditableTextCell::text() const {
  return m_textField.text();
}

void EditableTextCell::setText(const char * text) {
  m_textField.setText(text);
}

int EditableTextCell::numberOfSubviews() const {
  return 1;
}

View * EditableTextCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_textField;
}

void EditableTextCell::layoutSubviews() {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  m_textField.setFrame(KDRect(k_separatorThickness, (height - k_textHeight)/2, width - k_separatorThickness, k_textHeight));
}

void EditableTextCell::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_textField);
}

void EditableTextCell::setEditing(bool isEditing) {
  m_textField.setEditing(isEditing);
}

