#include "storage_function.h"
#include "poincare_helpers.h"
#include <poincare/serialization_helper.h>
#include "poincare/src/parsing/parser.h"
#include <ion/unicode/utf8_helper.h>
#include <ion/unicode/utf8_decoder.h>
#include <string.h>
#include <cmath>
#include <assert.h>

using namespace Poincare;

namespace Shared {

constexpr char StorageFunction::k_parenthesedArgument[];

bool StorageFunction::BaseNameCompliant(const char * baseName, NameNotCompliantError * error) {
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

bool StorageFunction::isActive() const {
  return recordData()->isActive();
}

KDColor StorageFunction::color() const {
  return recordData()->color();
}

void StorageFunction::setActive(bool active) {
  recordData()->setActive(active);
}

int StorageFunction::nameWithArgument(char * buffer, size_t bufferSize, char arg) {
  const char * functionName = fullName();
  size_t baseNameLength = SymbolAbstract::TruncateExtension(buffer, functionName, bufferSize - k_parenthesedArgumentLength);
  int result = baseNameLength + strlcpy(&buffer[baseNameLength], k_parenthesedArgument, bufferSize-baseNameLength);
  if (baseNameLength+1 < bufferSize - 1) {
    buffer[baseNameLength+1] = arg;
  }
  return result;
}

template<typename T>
T StorageFunction::templatedApproximateAtAbscissa(T x, Poincare::Context * context, char unknownSymbol) const {
  if (isCircularlyDefined(context)) {
    return NAN;
  }
  constexpr int bufferSize = CodePoint::MaxCodePointCharLength + 1;
  char unknownX[bufferSize];
  Poincare::SerializationHelper::CodePoint(unknownX, bufferSize, unknownSymbol);
  return PoincareHelpers::ApproximateWithValueForSymbol(expressionReduced(context), unknownX, x, *context);
}

StorageFunction::FunctionRecordData * StorageFunction::recordData() const {
  assert(!isNull());
  Ion::Storage::Record::Data d = value();
  return reinterpret_cast<FunctionRecordData *>(const_cast<void *>(d.buffer));
}

}

template float Shared::StorageFunction::templatedApproximateAtAbscissa<float>(float, Poincare::Context*, char) const;
template double Shared::StorageFunction::templatedApproximateAtAbscissa<double>(double, Poincare::Context*, char) const;
