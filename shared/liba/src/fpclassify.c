#include <math.h>
#include <private/ieee754.h>

int __fpclassify(double x) {
  if (ieee754exp64(x) == 0) {
    if (ieee754man64(x) == 0x0) {
      return FP_ZERO;
    } else {
      return FP_SUBNORMAL;
    }
  }
  if (ieee754exp64(x) == 0x7FF) {
    if (ieee754man64(x) == 0) {
      return FP_INFINITE;
    } else {
      return FP_NAN;
    }
  }
  return FP_NORMAL;
}
