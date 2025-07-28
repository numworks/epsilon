#include <omg/float.h>

#include <cmath>

namespace OMG::Float {

bool IsGreater(float xI, float xJ, bool nanIsGreatest) {
  if (std::isnan(xI)) {
    return nanIsGreatest;
  }
  if (std::isnan(xJ)) {
    return !nanIsGreatest;
  }
  return xI > xJ;
}

}  // namespace OMG::Float
