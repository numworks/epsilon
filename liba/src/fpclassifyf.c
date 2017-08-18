#include <math.h>
#include <private/ieee754.h>

int __fpclassifyf(float x) {
  if (ieee754exp32(x) == 0) {
    if (ieee754man32(x) == 0x0) {
      return FP_ZERO;
    } else {
      return FP_SUBNORMAL;
    }
  }
  if (ieee754exp32(x) == 0xFF) {
    if (ieee754man32(x) == 0) {
      return FP_INFINITE;
    } else {
      return FP_NAN;
    }
  }
  return FP_NORMAL;
}
