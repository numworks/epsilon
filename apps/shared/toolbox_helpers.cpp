#include "toolbox_helpers.h"
#include <string.h>

namespace Shared {
namespace ToolboxHelpers {

int CursorIndexInCommand(const char * text) {
  for (size_t i = 0; i < strlen(text); i++) {
    if (text[i] == '(') {
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
  size_t commandLength = strlen(command);
  for (size_t i = 0; i < commandLength; i++) {
    if (command[i] == ')') {
      numberOfOpenBrackets--;
    }
    if (numberOfOpenBrackets == 0 || command[i] == ',')
    {
      buffer[currentNewTextIndex] = command[i];
      buffer[currentNewTextIndex + 1] = 0;
      currentNewTextIndex++;
    }
    if (command[i] == '(') {
      numberOfOpenBrackets++;
    }
  }
}

}
}
