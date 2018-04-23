#include "toolbox_helpers.h"
#include <ion/charset.h>
#include <apps/i18n.h>
#include <string.h>
#include <assert.h>

namespace Shared {
namespace ToolboxHelpers {

int CursorIndexInCommandText(const char * text) {
  size_t textLength = strlen(text);
  for (size_t i = 0; i < textLength; i++) {
    if (text[i] == '(' || text[i] == '\'') {
      return i + 1;
    }
    if (text[i] == ']') {
      return i;
    }
  }
  return textLength;
}

void TextToInsertForCommandMessage(I18n::Message message, char * buffer, int bufferSize, bool replaceArgsWithEmptyChar) {
  TextToInsertForCommandText(I18n::translate(message), buffer, bufferSize, replaceArgsWithEmptyChar);
}

void TextToInsertForCommandText(const char * command, char * buffer, int bufferSize, bool replaceArgsWithEmptyChar) {
  int currentNewTextIndex = 0;
  int numberOfOpenParentheses = 0;
  int numberOfOpenBrackets = 0;
  bool insideQuote = false;
  bool argumentAlreadyReplaced = false;
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
      if (argumentAlreadyReplaced) {
        argumentAlreadyReplaced = false;
      }
      buffer[currentNewTextIndex++] = command[i];
    } else {
      if (replaceArgsWithEmptyChar && !argumentAlreadyReplaced) {
        buffer[currentNewTextIndex++] = Ion::Charset::Empty;
        argumentAlreadyReplaced = true;
      }
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

}
}
