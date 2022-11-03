#include "console_edit_cell.h"
#include "console_controller.h"
#include <escher/app.h>
#include <apps/i18n.h>
#include <apps/global_preferences.h>
#include <assert.h>
#include <algorithm>

using namespace Escher;

namespace Code {

ConsoleEditCell::ConsoleEditCell(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * delegate) :
  HighlightCell(),
  Responder(parentResponder),
  m_promptView(GlobalPreferences::sharedGlobalPreferences()->font(), nullptr, KDContext::k_alignLeft, KDContext::k_alignCenter),
  m_textField(this, nullptr, TextField::MaxBufferSize(), TextField::MaxBufferSize(), inputEventHandlerDelegate, delegate, GlobalPreferences::sharedGlobalPreferences()->font())
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

void ConsoleEditCell::layoutSubviews(bool force) {
  KDSize promptSize = m_promptView.minimalSizeForOptimalDisplay();
  m_promptView.setFrame(KDRect(KDPointZero, promptSize.width(), bounds().height()), force);
  m_textField.setFrame(KDRect(KDPoint(promptSize.width(), KDCoordinate(0)), bounds().width() - promptSize.width(), bounds().height()), force);
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

void ConsoleEditCell::clearAndReduceSize() {
  setText("");
  size_t previousBufferSize = m_textField.draftTextBufferSize();
  assert(previousBufferSize > 1);
  m_textField.setDraftTextBufferSize(previousBufferSize - 1);
}

const char * ConsoleEditCell::shiftCurrentTextAndClear() {
  size_t previousBufferSize = m_textField.draftTextBufferSize();
  m_textField.setDraftTextBufferSize(previousBufferSize + 1);
  char * textFieldBuffer = const_cast<char *>(m_textField.text());
  char * newTextPosition = textFieldBuffer + 1;
  assert(previousBufferSize > 0);
  size_t copyLength = std::min(previousBufferSize - 1, strlen(textFieldBuffer));
  memmove(newTextPosition, textFieldBuffer, copyLength);
  newTextPosition[copyLength] = 0;
  textFieldBuffer[0] = 0;
  return newTextPosition;
}

}
