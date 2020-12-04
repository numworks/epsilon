#include <drivers/svcall_args.h>

void setArg(int argc, const char * argv) {
  switch (argc) {
    case 0:
      asm volatile ("mov r7,%0"::"r"(argv));
      return;
    case 1:
      asm volatile ("mov r8,%0"::"r"(argv));
      return;
    }
}

const char * getArg(int argc) {
  const char * argv;
  switch (argc) {
    case 0:
      asm volatile ("mov %0,r7":"=r"(argv):);
      break;
    case 1:
      asm volatile ("mov %0,r8":"=r"(argv):);
      break;
    }
  return argv;
}

void svcArgs(int argc, const char * argv[]) {
  for (int argId = 0; argId < argc; ++argId) {
    setArg(argId, argv[argId]);
  }
}

void svcGetArgs(int argc, const char * argv[]) {
  for (int argId = 0; argId < argc; ++argId) {
    argv[argId] = getArg(argId);
  }
}