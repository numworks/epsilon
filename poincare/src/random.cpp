#include <poincare/random.h>
#include <ion.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type Random::type() const {
  return Type::Random;
}

Expression * Random::clone() const {
  Random * a = new Random();
  return a;
}

Expression * Random::setSign(Sign s, Context & context, AngleUnit angleUnit) {
  assert(s == Sign::Positive);
  return this;
}

template<typename T> T Random::random() {
  if (sizeof(T) == sizeof(float)) {
    uint32_t r = Ion::random();
    return (float)r/(float)(0xFFFFFFFF);
  } else {
    assert(sizeof(T) == sizeof(double));
    uint64_t r;
    uint32_t * rAddress = (uint32_t *)&r;
    *rAddress = Ion::random();
    *(rAddress+1) = Ion::random();
    return (double)r/(double)(0xFFFFFFFFFFFFFFFF);
  }
}

template float Random::random();
template double Random::random();

}
