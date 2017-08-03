#include <math.h>
#include <private/ieee754.h>

int isinfd(double x) {
  return (ieee754exp64(x) == 0x7FF && ieee754man64(x) == 0);
}
