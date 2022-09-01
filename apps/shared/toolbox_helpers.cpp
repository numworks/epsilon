#include "toolbox_helpers.h"
#include <apps/i18n.h>
#include <ion/unicode/utf8_decoder.h>
#include <string.h>
#include <assert.h>

namespace Shared {
namespace ToolboxHelpers {

void TextToInsertForCommandMessage(I18n::Message message, char * buffer, int bufferSize, bool replaceArgsWithEmptyChar) {
  TextToInsertForCommandText(I18n::translate(message), -1, buffer, bufferSize, replaceArgsWithEmptyChar);
}

void TextToInsertForCommandText(const char * command, int commandLength, char * buffer, int bufferSize, bool replaceArgsWithEmptyChar) {
  int index = 0;
  int numberOfOpenParentheses = 0;
  int numberOfOpenBrackets = 0;
  bool insideQuote = false;
  bool argumentAlreadyReplaced = false;

  UTF8Decoder decoder(command);
  CodePoint codePoint = decoder.nextCodePoint();
  while (codePoint != UCodePointNull &&
         index + static_cast<int>(UTF8Decoder::CharSizeOfCodePoint(codePoint)) < bufferSize &&
         (commandLength < 0 ||
          (decoder.stringPosition() - command <= commandLength))) {
    if (codePoint == ')') {
      numberOfOpenParentheses--;
    } else if (codePoint == ']') {
      numberOfOpenBrackets--;
    }
    if ((!insideQuote || codePoint == '\'')
        && ((numberOfOpenParentheses == 0 && numberOfOpenBrackets == 0)
          || codePoint == ','
          || (numberOfOpenBrackets > 0
            && (codePoint == ','
              || codePoint == '['
              || codePoint == ']'))))
    {
      assert(index < bufferSize);
      if (argumentAlreadyReplaced) {
        argumentAlreadyReplaced = false;
      }
      index += UTF8Decoder::CodePointToChars(codePoint, buffer + index, bufferSize - index - 1);
    } else {
      if (replaceArgsWithEmptyChar && !argumentAlreadyReplaced) {
        assert(index < bufferSize);
        index += UTF8Decoder::CodePointToChars(UCodePointEmpty, buffer + index, bufferSize - index - 1);
        argumentAlreadyReplaced = true;
      }
    }
    if (codePoint == '(') {
      numberOfOpenParentheses++;
    } else if (codePoint == '[') {
      numberOfOpenBrackets++;
    } else if (codePoint == '\'') {
      insideQuote = !insideQuote;
    }
    codePoint = decoder.nextCodePoint();
  }
  assert(index < bufferSize);
  buffer[index] = 0;
}

}
}
