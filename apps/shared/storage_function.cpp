#include "storage_function.h"
#include <poincare/symbol.h>
#include "poincare/src/parsing/parser.h"
#include <string.h>
#include <cmath>
#include <assert.h>

using namespace Poincare;

namespace Shared {

constexpr char StorageFunction::k_parenthesedArgument[];

bool StorageFunction::BaseNameCompliant(const char * baseName, NameNotCompliantError * error) {
  assert(baseName[0] != 0);
  if (baseName[0] >= '0' && baseName[0] <= '9') {
    // The name cannot start with a number
    if (error != nullptr) {
      *error = NameNotCompliantError::NameCannotStartWithNumber;
    }
    return false;
  }

  const char * currentChar = baseName;

  // The name should only have allowed characters
  while (*currentChar != 0) {
    if (!((*currentChar >= 'A' && *currentChar <= 'Z')
        || (*currentChar >= 'a' && *currentChar <= 'z')
        || (*currentChar >= '0' && *currentChar <= '9')
        || *currentChar == '_'))
    {
      if (error != nullptr) {
        *error = NameNotCompliantError::CharacterNotAllowed;
      }
      return false;
    }
    currentChar++;
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
T StorageFunction::templatedApproximateAtAbscissa(T x, Poincare::Context * context) const {
  if (isCircularlyDefined(context)) {
    return NAN;
  }
  const char unknownX[2] = {Poincare::Symbol::UnknownX, 0};
  return expressionReduced(context).approximateWithValueForSymbol(unknownX, x, *context, Preferences::sharedPreferences()->angleUnit());
}

StorageFunction::FunctionRecordData * StorageFunction::recordData() const {
  assert(!isNull());
  Ion::Storage::Record::Data d = value();
  return reinterpret_cast<FunctionRecordData *>(const_cast<void *>(d.buffer));
}

}

template float Shared::StorageFunction::templatedApproximateAtAbscissa<float>(float, Poincare::Context*) const;
template double Shared::StorageFunction::templatedApproximateAtAbscissa<double>(double, Poincare::Context*) const;
