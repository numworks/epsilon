#include <math.h>
#include <private/ieee754.h>

int isinff(float x) {
  return (ieee754exp32(x) == 0xFF && ieee754man32(x) == 0);
}
