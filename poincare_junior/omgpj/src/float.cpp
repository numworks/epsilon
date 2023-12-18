#include <omgpj/float.h>

#include <cmath>

bool OMG::Float::IsGreater(float xI, float xJ, bool nanIsGreatest) {
  if (std::isnan(xI)) {
    return nanIsGreatest;
  }
  if (std::isnan(xJ)) {
    return !nanIsGreatest;
  }
  return xI > xJ;
}
