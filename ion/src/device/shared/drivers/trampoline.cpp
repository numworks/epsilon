#include <shared/drivers/trampoline.h>
#include <config/board.h>
#include <string.h>

namespace Ion {
namespace Device {
namespace Trampoline {

uint32_t addressOfFunction(int index) {
  return Board::Config::TrampolineOrigin + sizeof(void *) * index;
}

}
}
}

// Work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=51205
void * memset(void *, int, size_t) __attribute__((externally_visible));
void * memcpy(void *, const void *, size_t) __attribute__((externally_visible));

TRAMPOLINE_INTERFACE(TRAMPOLINE_MEMCMP, memcmp, (a,b,c), int, const void * a, const void * b, size_t c)
TRAMPOLINE_INTERFACE(TRAMPOLINE_MEMCPY, memcpy, (a,b,c), void *, void * a, const void * b, size_t c)
TRAMPOLINE_INTERFACE(TRAMPOLINE_MEMMOVE, memmove, (a,b,c), void *, void * a, const void * b, size_t c)
TRAMPOLINE_INTERFACE(TRAMPOLINE_MEMSET, memset, (a,b,c), void *, void * a, int b, size_t c)
TRAMPOLINE_INTERFACE(TRAMPOLINE_STRCHR, strchr, (a,b), char *, const char * a, int b)
TRAMPOLINE_INTERFACE(TRAMPOLINE_STRCMP, strcmp, (a,b), int, const char * a, const char * b)
TRAMPOLINE_INTERFACE(TRAMPOLINE_STRLCAT, strlcat, (a,b,c), size_t, char * a, const char * b, size_t c)
TRAMPOLINE_INTERFACE(TRAMPOLINE_STRLCPY, strlcpy, (a,b,c), size_t, char * a, const char * b, size_t c)
TRAMPOLINE_INTERFACE(TRAMPOLINE_STRLEN, strlen, (a), size_t, const char * a)
TRAMPOLINE_INTERFACE(TRAMPOLINE_STRNCMP, strncmp, (a,b,c), int, const char * a, const char * b, size_t c)
