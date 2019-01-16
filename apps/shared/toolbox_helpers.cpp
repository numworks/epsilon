#include "toolbox_helpers.h"
#include <apps/i18n.h>
#include <kandinsky/unicode/utf8_decoder.h>
#include <string.h>
#include <assert.h>

namespace Shared {
namespace ToolboxHelpers {

int CursorIndexInCommandText(const char * text) {
  UTF8Decoder decoder(text);
  size_t index = 0;
  const char * currentPointer = text;
  const char * nextPointer = decoder.nextCodePointPointer();
  CodePoint codePoint = decoder.nextCodePoint();
  while (codePoint != KDCodePointNull) {
    if (codePoint == '(' || codePoint == '\'') {
      return index + 1;
    }
    if (codePoint == '[') {
      return index;
    }
    index+= nextPointer - currentPointer;
    currentPointer = nextPointer;
    nextPointer = decoder.nextCodePointPointer();
    codePoint = decoder.nextCodePoint();
  }
  return index;
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
        currentNewTextIndex += UTF8Decoder::CodePointToChars(KDCodePointEmpty, buffer + currentNewTextIndex, bufferSize - currentNewTextIndex);
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
