#include "storage_function.h"
#include <poincare/symbol.h>
#include <string.h>
#include <cmath>
#include <assert.h>

using namespace Poincare;

namespace Shared {

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
  size_t index = 0;
  const char ofXSring[4] = {'(',arg,')', 0};
  size_t ofXSringLength = strlen(ofXSring);
  while (*functionName != Ion::Storage::k_dotChar
      && index < bufferSize - ofXSringLength - 1)
  {
    // We keep room to write the final "(x)" //TODO should we?
    assert(functionName < fullName() + strlen(fullName()));
    buffer[index++] = *functionName++;
  }
  return index + strlcpy(&buffer[index], ofXSring, bufferSize-index);
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
