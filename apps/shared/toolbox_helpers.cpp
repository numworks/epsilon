#include "toolbox_helpers.h"
#include <ion/charset.h>
#include <string.h>

namespace Shared {
namespace ToolboxHelpers {

int CursorIndexInCommand(const char * text) {
  for (size_t i = 0; i < strlen(text); i++) {
    if (text[i] == '(' || text[i] == '\'') {
      return i + 1;
    }
  }
  return strlen(text);
}

void TextToInsertForCommandMessage(I18n::Message message, char * buffer) {
  const char * messageText = I18n::translate(message);
  TextToInsertForCommandText(messageText, buffer);
}

void TextToInsertForCommandText(const char * command, char * buffer) {
  int currentNewTextIndex = 0;
  int numberOfOpenBrackets = 0;
  bool insideQuote = false;
  size_t commandLength = strlen(command);
  for (size_t i = 0; i < commandLength; i++) {
    if (command[i] == ')') {
      numberOfOpenBrackets--;
    }
    if ((numberOfOpenBrackets == 0 || command[i] == ',') && (!insideQuote || command[i] == '\'')) {
      buffer[currentNewTextIndex++] = command[i];
    }
    if (command[i] == '(') {
      numberOfOpenBrackets++;
    }
    if (command[i] == '\'') {
      insideQuote = !insideQuote;
    }
  }
  buffer[currentNewTextIndex] = 0;
}

void TextToParseIntoLayoutForCommandMessage(I18n::Message message, char * buffer) {
  const char * messageText = I18n::translate(message);
  TextToInsertForCommandText(messageText, buffer);
  size_t bufferLength = strlen(buffer);
  for (size_t i = 0; i < bufferLength; i++) {
    if (buffer[i] == '(' || buffer[i] == ',') {
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
