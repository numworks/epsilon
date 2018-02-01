#include <escher/editable_text_cell.h>
#include <escher/app.h>
#include <escher/palette.h>
#include <assert.h>

EditableTextCell::EditableTextCell(Responder * parentResponder, TextFieldDelegate * delegate, char * draftTextBuffer,
   KDText::FontSize size, float horizontalAlignment, float verticalAlignment, KDColor textColor, KDColor backgroundColor, KDCoordinate topMargin, KDCoordinate rightMargin, KDCoordinate bottomMargin, KDCoordinate leftMargin) :
  HighlightCell(),
  Responder(parentResponder),
  m_textField(this, m_textBody, draftTextBuffer, TextField::maxBufferSize(), delegate, true, size, horizontalAlignment, verticalAlignment, textColor, backgroundColor),
  m_topMargin(topMargin),
  m_rightMargin(rightMargin),
  m_bottomMargin(bottomMargin),
  m_leftMargin(leftMargin)
{
  m_textBody[0] = 0;
}

void EditableTextCell::setMargins(KDCoordinate topMargin, KDCoordinate rightMargin, KDCoordinate bottomMargin, KDCoordinate leftMargin) {
  m_topMargin = topMargin;
  m_rightMargin = rightMargin;
  m_bottomMargin = bottomMargin;
  m_leftMargin = leftMargin;
}

TextField * EditableTextCell::textField() {
  return &m_textField;
}

void EditableTextCell::setHighlighted(bool highlight) {
  HighlightCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Palette::Select : KDColorWhite;
  m_textField.setBackgroundColor(backgroundColor);
}

int EditableTextCell::numberOfSubviews() const {
  return 1;
}

View * EditableTextCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_textField;
}

void EditableTextCell::layoutSubviews() {
  KDRect cellBounds = bounds();
  m_textField.setFrame(KDRect(cellBounds.x() + m_leftMargin,
        cellBounds.y() + m_topMargin,
        cellBounds.width() - m_leftMargin - m_rightMargin,
        cellBounds.height() - m_topMargin - m_bottomMargin));
}

void EditableTextCell::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_textField);
}

KDSize EditableTextCell::minimalSizeForOptimalDisplay() const {
  return m_textField.minimalSizeForOptimalDisplay();
}
