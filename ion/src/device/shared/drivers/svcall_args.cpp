#include "svcall_args.h"
#include "svcall.h"

/* STORE and LOAD are performed within the same function to ensure they are in
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
  if (argc > 0) {
    SVC_STORE_ARG(SVC_ARGS_REGISTER_0, argv[0]);
    if (argc > 1) {
      SVC_STORE_ARG(SVC_ARGS_REGISTER_1, argv[1]);
    }
  }
  /* TODO Hugo : with DEBUG=1 :
   * svc(svcNumber); -> asm operand 0 probably doesn't match constraints */
  if (svcNumber == SVC_PUSH_RECT_UNIFORM) {
    svc(SVC_PUSH_RECT_UNIFORM);
  } else if (svcNumber == SVC_PUSH_RECT) {
    svc(SVC_PUSH_RECT);
  }
}
