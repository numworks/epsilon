#include "console_edit_cell.h"
#include <escher/app.h>
#include <assert.h>

namespace Code {

ConsoleEditCell::ConsoleEditCell(Responder * parentResponder, TextFieldDelegate * delegate) :
  HighlightCell(),
  Responder(parentResponder),
  m_textField(this, m_textBuffer, m_draftTextBuffer, TextField::maxBufferSize(), delegate)
{
}

int ConsoleEditCell::numberOfSubviews() const {
  return 1;
}

View * ConsoleEditCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_textField;
}

void ConsoleEditCell::layoutSubviews() {
  m_textField.setFrame(bounds());
}

void ConsoleEditCell::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_textField);
}

void ConsoleEditCell::setEditing(bool isEditing) {
  m_textField.setEditing(isEditing);
}

}
