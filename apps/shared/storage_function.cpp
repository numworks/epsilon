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
  return true; //TODO
}

KDColor StorageFunction::color() const {
  return KDColorRed; //TODO
}

void StorageFunction::setActive(bool active) {
  //TODO
}

void StorageFunction::setColor(KDColor color) {
  //TODO
}

template<typename T>
T StorageFunction::templatedApproximateAtAbscissa(T x, Poincare::Context * context) const {
  return reducedExpression(context).approximateWithValueForSymbol(symbol(), x, *context, Preferences::sharedPreferences()->angleUnit());
}

}

template float Shared::StorageFunction::templatedApproximateAtAbscissa<float>(float, Poincare::Context*) const;
template double Shared::StorageFunction::templatedApproximateAtAbscissa<double>(double, Poincare::Context*) const;
