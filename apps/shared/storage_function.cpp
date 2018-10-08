#include "storage_function.h"
#include <string.h>
#include <cmath>
#include <assert.h>

using namespace Poincare;

namespace Shared {

uint32_t StorageFunction::checksum() {
  assert(!record().isNull());
  return record().checksum();
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

void StorageFunction::setColor(KDColor color) {
  recordData()->setColor(color);
}

template<typename T>
T StorageFunction::templatedApproximateAtAbscissa(T x, Poincare::Context * context) const {
  return reducedExpression(context).approximateWithValueForSymbol(symbol(), x, *context, Preferences::sharedPreferences()->angleUnit());
}

StorageFunction::FunctionRecordData * StorageFunction::recordData() const {
  assert(!record().isNull());
  Ion::Storage::Record::Data d = record().value();
  return reinterpret_cast<FunctionRecordData *>(const_cast<void *>(d.buffer));
}

}

template float Shared::StorageFunction::templatedApproximateAtAbscissa<float>(float, Poincare::Context*) const;
template double Shared::StorageFunction::templatedApproximateAtAbscissa<double>(double, Poincare::Context*) const;
