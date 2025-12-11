#include <stdlib.h>

extern "C" {
__attribute__((noreturn)) void abort() { exit(1); }
}
