#include <escher/even_odd_editable_text_cell.h>
#include <escher/container.h>
#include <assert.h>

namespace Escher {

EvenOddEditableTextCell::EvenOddEditableTextCell(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * delegate, KDFont::Size font, float horizontalAlignment, float verticalAlignment, KDCoordinate topMargin, KDCoordinate rightMargin, KDCoordinate bottomMargin, KDCoordinate leftMargin) :
  EvenOddCell(),
  Responder(parentResponder),
  m_editableCell(this, inputEventHandlerDelegate, delegate, font, horizontalAlignment, verticalAlignment, KDColorBlack, KDColorWhite, topMargin, rightMargin, bottomMargin, leftMargin)
{
}

EditableTextCell * EvenOddEditableTextCell::editableTextCell() {
  return &m_editableCell;
}

void EvenOddEditableTextCell::updateSubviewsBackgroundAfterChangingState() {
  m_editableCell.textField()->setBackgroundColor(backgroundColor());
}

int EvenOddEditableTextCell::numberOfSubviews() const {
  return 1;
}

View * EvenOddEditableTextCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_editableCell;
}

void EvenOddEditableTextCell::layoutSubviews(bool force) {
  m_editableCell.setFrame(bounds(), force);
}

void EvenOddEditableTextCell::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_editableCell);
}

}