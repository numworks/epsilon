#include "svcall.h"
#include <assert.h>

namespace Ion {
namespace Device {
namespace SVCall {

#define SVC_LOAD_ARG(reg, argv) asm volatile ("mov %0," reg :"=r"(argv):);

void getSvcallArgs(int argc, void * argv[]) {
  if (argc > 0) {
    SVC_LOAD_ARG(SVC_ARGS_REGISTER_0, argv[0]);
    if (argc > 1) {
      SVC_LOAD_ARG(SVC_ARGS_REGISTER_1, argv[1]);
    }
  }
}

}
}
}
