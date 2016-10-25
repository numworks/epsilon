#include <math.h>
#include <private/ieee754.h>

int isnanf(float x) {
  return (ieee754exp(x) == 0xFF && ieee754man(x) != 0);
}
