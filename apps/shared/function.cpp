#include "function.h"
#include "poincare_helpers.h"
#include "poincare/src/parsing/parser.h"
#include <ion/unicode/utf8_helper.h>
#include <ion/unicode/utf8_decoder.h>
#include <apps/i18n.h>
#include <string.h>
#include <cmath>
#include <assert.h>

using namespace Poincare;

namespace Shared {

bool Function::BaseNameCompliant(const char * baseName, NameNotCompliantError * error) {
  assert(baseName[0] != 0);

  UTF8Decoder decoder(baseName);
  CodePoint c = decoder.nextCodePoint();
  if (UTF8Helper::CodePointIsNumber(c)) {
    // The name cannot start with a number
    if (error != nullptr) {
      *error = NameNotCompliantError::NameCannotStartWithNumber;
    }
    return false;
  }

  // The name should only have allowed characters
  while (c != UCodePointNull) {
    if (!(UTF8Helper::CodePointIsUpperCaseLetter(c)
        || UTF8Helper::CodePointIsLowerCaseLetter(c)
        || UTF8Helper::CodePointIsNumber(c))
        || c == '_')
    {
      if (error != nullptr) {
        *error = NameNotCompliantError::CharacterNotAllowed;
      }
      return false;
    }
    c = decoder.nextCodePoint();
  }

  // The name should not be a reserved name
  if (Parser::IsReservedName(baseName, strlen(baseName))) {
    if (error != nullptr) {
      *error = NameNotCompliantError::ReservedName;
    }
    return false;
  }
  return true;
}

bool Function::isActive() const {
  return recordData()->isActive();
}

KDColor Function::color() const {
  return recordData()->color();
}

void Function::setActive(bool active) {
  recordData()->setActive(active);
}

int Function::printValue(double cursorT, double cursorX, double cursorY, char * buffer, int bufferSize, int precision, Poincare::Context * context) {
  return PoincareHelpers::ConvertFloatToText<double>(cursorY, buffer, bufferSize, precision);
}

I18n::Message Function::parameterMessageName() const {
  return I18n::Message::X;
}

int Function::nameWithArgument(char * buffer, size_t bufferSize) {
  const char * functionName = fullName();
  size_t result = SymbolAbstract::TruncateExtension(buffer, functionName, bufferSize);
  assert(result <= bufferSize);
  buffer[result++] = '(';
  assert(result <= bufferSize);
  assert(UTF8Decoder::CharSizeOfCodePoint(symbol()) <= 2);
  result += UTF8Decoder::CodePointToChars(symbol(), buffer+result, bufferSize-result);
  assert(result <= bufferSize);
  result += strlcpy(buffer+result, ")", bufferSize-result);
  return result;
}

Function::FunctionRecordDataBuffer * Function::recordData() const {
  assert(!isNull());
  Ion::Storage::Record::Data d = value();
  return reinterpret_cast<FunctionRecordDataBuffer *>(const_cast<void *>(d.buffer));
}

}
