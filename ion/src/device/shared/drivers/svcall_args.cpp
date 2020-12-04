#include "svcall_args.h"
#include "svcall.h"
#include <assert.h>
#include <ion/timing.h>

void setSvcallArg(int argId, const char * argv) {
  // TODO : remove this msleep. without it, the compiler reorder setArg calls
  Ion::Timing::msleep(1);
  switch (argId) {
    case 0:
      asm volatile ("mov r7,%0"::"r"(argv));
      return;
    case 1:
      asm volatile ("mov r8,%0"::"r"(argv));
      return;
    default:
      assert(false);
    }
}

const char * getSvcallArg(int argId) {

  const char * argv;
  switch (argId) {
    case 0:
      asm volatile ("mov %0,r7":"=r"(argv):);
      break;
    case 1:
      asm volatile ("mov %0,r8":"=r"(argv):);
      break;
    default:
      assert(false);
    }
  return argv;
}

void setSvcallArgs(int argc, const char * argv[]) {
  for (int argId = 0; argId < argc; ++argId) {
    setSvcallArg(argId, argv[argId]);
  }
}

void getSvcallArgs(int argc, const char * argv[]) {
  for (int argId = 0; argId < argc; ++argId) {
    argv[argId] = getSvcallArg(argId);
  }
}

void svcall(unsigned int svcNumber, int argc, const char * argv[]) {
  if (argc > 0) {
    setSvcallArgs(argc, argv);
  }
  svc(svcNumber);
}