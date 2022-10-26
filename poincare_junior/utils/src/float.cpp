#include <utils/float.h>
#include <cmath>

bool Float::IsGreater(float xI, float xJ, bool nanIsGreatest) {
  if (std::isnan(xI)) {
    return nanIsGreatest;
  }
  if (std::isnan(xJ)) {
    return !nanIsGreatest;
  }
  return xI > xJ;
}
