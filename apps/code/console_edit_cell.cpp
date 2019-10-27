#include "console_edit_cell.h"
#include "console_controller.h"
#include <escher/app.h>
#include <apps/i18n.h>
#include <assert.h>

namespace Code {

ConsoleEditCell::ConsoleEditCell(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * delegate) :
  HighlightCell(),
  Responder(parentResponder),
  m_promptView(ConsoleController::k_font, nullptr, 0, 0.5),
  m_textField(this, nullptr, TextField::maxBufferSize(), TextField::maxBufferSize(), inputEventHandlerDelegate, delegate, ConsoleController::k_font)
{
}

int ConsoleEditCell::numberOfSubviews() const {
  return 2;
}

View * ConsoleEditCell::subviewAtIndex(int index) {
  assert(index == 0 || index ==1);
  if (index == 0) {
   return &m_promptView;
  } else {
   return &m_textField;
  }
}

void ConsoleEditCell::layoutSubviews() {
  KDSize promptSize = m_promptView.minimalSizeForOptimalDisplay();
  m_promptView.setFrame(KDRect(KDPointZero, promptSize.width(), bounds().height()));
  m_textField.setFrame(KDRect(KDPoint(promptSize.width(), KDCoordinate(0)), bounds().width() - promptSize.width(), bounds().height()));
}

void ConsoleEditCell::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_textField);
}

void ConsoleEditCell::setEditing(bool isEditing) {
  m_textField.setEditing(isEditing);
}

void ConsoleEditCell::setText(const char * text) {
  m_textField.setText(text);
}

void ConsoleEditCell::setPrompt(const char * prompt) {
  m_promptView.setText(prompt);
  layoutSubviews();
}

bool ConsoleEditCell::insertText(const char * text) {
  return m_textField.handleEventWithText(text);
}

}
