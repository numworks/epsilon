#include "toolbox_helpers.h"
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

}
}
