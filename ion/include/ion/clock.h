#ifndef ION_CLOCK_H
#define ION_CLOCK_H

#include <stdint.h>

namespace Ion {
namespace Clock {

void setClock(int hours, int mins);
void clock(int *hours, int *mins);

}
}

#endif
