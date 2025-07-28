#include "toolbox_helpers.h"

#include <apps/i18n.h>
#include <assert.h>
#include <omg/utf8_decoder.h>
#include <omg/utf8_helper.h>
#include <string.h>

namespace Shared {
namespace ToolboxHelpers {

void TextToInsertForCommandMessage(I18n::Message message, char* buffer,
                                   size_t bufferSize,
                                   bool replaceArgsWithEmptyChar) {
  TextToInsertForCommandText(I18n::translate(message), -1, buffer, bufferSize,
                             replaceArgsWithEmptyChar);
}

void TextToInsertForCommandText(const char* command, int commandLength,
                                char* buffer, size_t bufferSize,
                                bool replaceArgsWithEmptyChar) {
  size_t length = 0;
  buffer[length] = 0;
  int numberOfOpenParentheses = 0;
  int numberOfOpenBrackets = 0;
  bool insideQuote = false;
  bool argumentAlreadyReplaced = false;

  UTF8Decoder decoder(command);
  CodePoint codePoint = decoder.nextCodePoint();
  while (codePoint != UCodePointNull &&
         length + UTF8Decoder::CharSizeOfCodePoint(codePoint) < bufferSize &&
         (commandLength < 0 ||
          (decoder.stringPosition() - command <= commandLength))) {
    if (codePoint == ')') {
      numberOfOpenParentheses--;
    } else if (codePoint == ']') {
      numberOfOpenBrackets--;
    }
    if ((!insideQuote || codePoint == '\'') &&
        ((numberOfOpenParentheses == 0 && numberOfOpenBrackets == 0) ||
         codePoint == ',' ||
         (numberOfOpenBrackets > 0 &&
          (codePoint == ',' || codePoint == '[' || codePoint == ']')))) {
      assert(length < bufferSize);
      if (argumentAlreadyReplaced) {
        argumentAlreadyReplaced = false;
      }
      length += UTF8Helper::WriteCodePoint(buffer + length, bufferSize - length,
                                           codePoint);
    } else {
      if (replaceArgsWithEmptyChar && !argumentAlreadyReplaced) {
        assert(length < bufferSize);
        length += UTF8Helper::WriteCodePoint(
            buffer + length, bufferSize - length, UCodePointEmpty);
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
  assert(length < bufferSize);
  assert(buffer[length] == 0);
}

}  // namespace ToolboxHelpers
}  // namespace Shared
