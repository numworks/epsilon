#include "storage_function.h"
#include <poincare/symbol.h>
#include <string.h>
#include <cmath>
#include <assert.h>

using namespace Poincare;

namespace Shared {

char StorageFunction::k_parenthesedArgument[]("(x)");

uint32_t StorageFunction::checksum() {
  assert(!isNull());
  return checksum();
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
  k_parenthesedArgument[1] = arg;
  size_t baseNameLength = SymbolAbstract::TruncateExtension(buffer, functionName, bufferSize - k_parenthesedArgumentLength);
  return baseNameLength + strlcpy(&buffer[baseNameLength], k_parenthesedArgument, bufferSize-baseNameLength);
}

template<typename T>
T StorageFunction::templatedApproximateAtAbscissa(T x, Poincare::Context * context) const {
  const char unknownX[2] = {Poincare::Symbol::UnknownX, 0};
  return expression(context).approximateWithValueForSymbol(unknownX, x, *context, Preferences::sharedPreferences()->angleUnit());
}

StorageFunction::FunctionRecordData * StorageFunction::recordData() const {
  assert(!isNull());
  Ion::Storage::Record::Data d = value();
  return reinterpret_cast<FunctionRecordData *>(const_cast<void *>(d.buffer));
}

}

template float Shared::StorageFunction::templatedApproximateAtAbscissa<float>(float, Poincare::Context*) const;
template double Shared::StorageFunction::templatedApproximateAtAbscissa<double>(double, Poincare::Context*) const;
