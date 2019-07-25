#include "function.h"
#include "poincare/src/parsing/parser.h"
#include <ion/unicode/utf8_helper.h>
#include <ion/unicode/utf8_decoder.h>
#include <string.h>
#include <cmath>
#include <assert.h>

using namespace Poincare;

namespace Shared {

constexpr char Function::k_parenthesedArgument[];

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

int Function::nameWithArgument(char * buffer, size_t bufferSize) {
  const char * functionName = fullName();
  size_t baseNameLength = SymbolAbstract::TruncateExtension(buffer, functionName, bufferSize - k_parenthesedArgumentLength);
  assert(baseNameLength <= bufferSize);
  size_t result = baseNameLength + strlcpy(&buffer[baseNameLength], k_parenthesedArgument, bufferSize-baseNameLength);
  int bufferRemainingSize = bufferSize - (baseNameLength+1);
  if (bufferRemainingSize > 0) {
    assert(UTF8Decoder::CharSizeOfCodePoint(symbol()) == 1);
    UTF8Decoder::CodePointToChars(symbol(), buffer+baseNameLength+1, bufferRemainingSize);
  }
  return result;
}

Function::FunctionRecordDataBuffer * Function::recordData() const {
  assert(!isNull());
  Ion::Storage::Record::Data d = value();
  return reinterpret_cast<FunctionRecordDataBuffer *>(const_cast<void *>(d.buffer));
}

}
