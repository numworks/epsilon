#ifndef ION_LOG_H
#define ION_LOG_H

#include <stdint.h>

namespace Ion {
namespace Log {

void print(const char * message);
void print(uint32_t number);

}
}

#endif
