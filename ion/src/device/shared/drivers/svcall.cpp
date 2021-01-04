#include "svcall.h"
#include <assert.h>

namespace Ion {
namespace Device {
namespace SVCall {

#define SVC_LOAD_ARG(reg, argv) asm volatile ("mov %0," reg :"=r"(argv):);

void getSvcallArgs(void * argv[]) {
  if (argv) {
    SVC_LOAD_ARG(SVC_ARGS_REGISTER, argv);
  }
}

}
}
}
