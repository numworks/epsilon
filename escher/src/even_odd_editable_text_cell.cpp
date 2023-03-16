#include <assert.h>
#include <escher/container.h>
#include <escher/even_odd_editable_text_cell.h>

namespace Escher {

EvenOddEditableTextCell::EvenOddEditableTextCell(
    Responder* parentResponder,
    InputEventHandlerDelegate* inputEventHandlerDelegate,
    TextFieldDelegate* delegate, KDFont::Size font, float horizontalAlignment,
    float verticalAlignment)
    : EvenOddCell(),
      Responder(parentResponder),
      m_editableCell(this, inputEventHandlerDelegate, delegate, font,
                     horizontalAlignment, verticalAlignment, KDColorBlack,
                     KDColorWhite) {}

EditableTextCell* EvenOddEditableTextCell::editableTextCell() {
  return &m_editableCell;
}

void EvenOddEditableTextCell::updateSubviewsBackgroundAfterChangingState() {
  m_editableCell.textField()->setBackgroundColor(backgroundColor());
}

int EvenOddEditableTextCell::numberOfSubviews() const { return 1; }

View* EvenOddEditableTextCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_editableCell;
}

void EvenOddEditableTextCell::layoutSubviews(bool force) {
  setChildFrame(&m_editableCell,
                KDRect(bounds().left(), bounds().top(),
                       bounds().width() - k_rightMargin, bounds().height()),
                force);
}

void EvenOddEditableTextCell::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_editableCell);
}

}  // namespace Escher
