#include <assert.h>
#include <escher/container.h>
#include <escher/editable_text_cell.h>
#include <escher/palette.h>
#include <poincare/print_float.h>

namespace Escher {

EditableTextCell::EditableTextCell(
    Responder* parentResponder,
    InputEventHandlerDelegate* inputEventHandlerDelegate,
    TextFieldDelegate* delegate, KDFont::Size font, float horizontalAlignment,
    float verticalAlignment, KDColor textColor, KDColor backgroundColor)
    : HighlightCell(),
      Responder(parentResponder),
      m_textField(this, m_textBody, Poincare::PrintFloat::k_maxFloatCharSize,
                  inputEventHandlerDelegate, delegate, font,
                  horizontalAlignment, verticalAlignment, textColor,
                  backgroundColor) {
  m_textBody[0] = 0;
}

TextField* EditableTextCell::textField() { return &m_textField; }

void EditableTextCell::setHighlighted(bool highlight) {
  HighlightCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight ? Palette::Select : KDColorWhite;
  m_textField.setBackgroundColor(backgroundColor);
}

int EditableTextCell::numberOfSubviews() const { return 1; }

View* EditableTextCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_textField;
}

void EditableTextCell::layoutSubviews(bool force) {
  KDRect cellBounds = bounds();
  setChildFrame(&m_textField, cellBounds, force);
}

void EditableTextCell::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_textField);
}

KDSize EditableTextCell::minimalSizeForOptimalDisplay() const {
  return m_textField.minimalSizeForOptimalDisplay();
}

}  // namespace Escher
