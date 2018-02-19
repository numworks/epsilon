#include "console_edit_cell.h"
#include "console_controller.h"
#include <escher/app.h>
#include <apps/i18n.h>
#include <assert.h>

namespace Code {

ConsoleEditCell::ConsoleEditCell(Responder * parentResponder, TextFieldDelegate * delegate) :
  HighlightCell(),
  Responder(parentResponder),
  m_textBuffer{0},
  m_promptView(ConsoleController::k_fontSize, nullptr, 0, 0.5),
  m_textField(this, m_textBuffer, m_textBuffer, TextField::maxBufferSize(), delegate, false, ConsoleController::k_fontSize)
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
  app()->setFirstResponder(&m_textField);
}

void ConsoleEditCell::setEditing(bool isEditing,  bool reinitDraftBuffer) {
  m_textField.setEditing(isEditing, reinitDraftBuffer);
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