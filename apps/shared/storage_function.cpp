#include "storage_function.h"
#include <string.h>
#include <cmath>
#include <assert.h>

using namespace Poincare;

namespace Shared {

uint32_t StorageFunction::checksum() {
  char data[k_dataLengthInBytes/sizeof(char)] = {};
  strlcpy(data, text(), TextField::maxBufferSize());
  data[k_dataLengthInBytes-2] = m_name != nullptr ? m_name[0] : 0;
  data[k_dataLengthInBytes-1] = m_active ? 1 : 0;
  return Ion::crc32((uint32_t *)data, k_dataLengthInBytes/sizeof(uint32_t));
}

template<typename T>
T StorageFunction::templatedApproximateAtAbscissa(T x, Poincare::Context * context) const {
  return expression(context).approximateWithValueForSymbol(symbol(), x, *context, Preferences::sharedPreferences()->angleUnit());
}

}

template float Shared::StorageFunction::templatedApproximateAtAbscissa<float>(float, Poincare::Context*) const;
template double Shared::StorageFunction::templatedApproximateAtAbscissa<double>(double, Poincare::Context*) const;
