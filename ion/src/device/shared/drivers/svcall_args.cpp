#include "svcall_args.h"
#include "svcall.h"
#include <ion/timing.h>

/* STORE and LOAD are performed within the same fonction to ensure they are in
 * the same routine as the one calling svc().
 * TODO HUGO : Ensure that it is the case, and that it is relevant */

void getSvcallArgs(int argc, void * argv[]) {
  if (argc > 0) {
    SVC_LOAD_ARG(SVC_ARGS_REGISTER_0, argv[0])
    if (argc > 1) {
      SVC_LOAD_ARG(SVC_ARGS_REGISTER_1, argv[1])
    }
  }
}

void svcall(unsigned int svcNumber, int argc, void * argv[]) {
  // TODO HUGO : remove this msleep. without it, the compiler reorder svc call
  Ion::Timing::msleep(1);
  if (argc > 0) {
    SVC_STORE_ARG(SVC_ARGS_REGISTER_0, argv[0])
    if (argc > 1) {
      SVC_STORE_ARG(SVC_ARGS_REGISTER_1, argv[1])
    }
  }
  svc(svcNumber);
}
