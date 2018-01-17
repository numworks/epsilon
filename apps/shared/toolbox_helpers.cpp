#include "toolbox_helpers.h"
#include <ion/charset.h>
#include <apps/i18n.h>
#include <string.h>
#include <assert.h>

namespace Shared {
namespace ToolboxHelpers {

int CursorIndexInCommand(const char * text) {
  for (size_t i = 0; i < strlen(text); i++) {
    if (text[i] == '(' || text[i] == '\'') {
      return i + 1;
    }
    if (text[i] == ']') {
      return (i - 1) > 0 ? i - 1 : 0;
    }
  }
  return strlen(text);
}

void TextToInsertForCommandMessage(I18n::Message message, char * buffer, int bufferSize) {
  const char * messageText = I18n::translate(message);
  TextToInsertForCommandText(messageText, buffer, bufferSize);
}

void TextToInsertForCommandText(const char * command, char * buffer, int bufferSize) {
  int currentNewTextIndex = 0;
  int numberOfOpenParentheses = 0;
  int numberOfOpenBrackets = 0;
  bool insideQuote = false;
  size_t commandLength = strlen(command);
  for (size_t i = 0; i < commandLength; i++) {
    if (command[i] == ')') {
      numberOfOpenParentheses--;
    }
    if (command[i] == ']') {
      numberOfOpenBrackets--;
    }
    if (((numberOfOpenParentheses == 0 && numberOfOpenBrackets == 0)
          || command[i] == ','
          || (numberOfOpenBrackets > 0 && (command[i] == ',' || command[i] == '[' || command[i] == ']')))
        && (!insideQuote || command[i] == '\'')) {
      assert(currentNewTextIndex < bufferSize);
      buffer[currentNewTextIndex++] = command[i];
    }
    if (command[i] == '(') {
      numberOfOpenParentheses++;
    }
    if (command[i] == '[') {
      numberOfOpenBrackets++;
    }
    if (command[i] == '\'') {
      insideQuote = !insideQuote;
    }
  }
  buffer[currentNewTextIndex] = 0;
}

void TextToParseIntoLayoutForCommandMessage(I18n::Message message, char * buffer, int bufferSize) {
  if (message == I18n::Message::MatrixCommandWithArg) {
    assert(bufferSize >= 6);
    // Handle a new matrix command.
    buffer[0] = '[';
    buffer[1] = '[';
    buffer[2] = Ion::Charset::Empty;
    buffer[3] = ']';
    buffer[4] = ']';
    buffer[5] = 0;
    return;
  }
  const char * messageText = I18n::translate(message);
  TextToInsertForCommandText(messageText, buffer, bufferSize);
  size_t bufferLength = strlen(buffer);
  for (size_t i = 0; i < bufferLength; i++) {
    if (buffer[i] == '(' || buffer[i] == '[' || buffer[i] == ',') {
      // Shift the buffer to make room for the new char. Use memmove to avoid
      // overwritting.
      memmove(&buffer[i+2], &buffer[i+1], bufferLength - (i+1) + 1);
      bufferLength++;
      i++;
      buffer[i] = Ion::Charset::Empty;
    }
  }
}

}
}
