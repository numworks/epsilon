#include <shared/drivers/trampoline.h>
#include <shared/drivers/config/board.h>
#include <string.h>

namespace Ion {
namespace Device {
namespace Trampoline {

uint32_t addressOfFunction(int index) {
  return Board::Config::BootloaderTrampolineAddress + sizeof(void *) * index;
}

}
}
}

#define TRAMPOLINE_INTERFACE(index, function, argsList, returnType, args...) \
  typedef returnType (*FunctionType##_##function)(args); \
  returnType function(args) { \
    FunctionType##_##function * trampolineFunction = reinterpret_cast<FunctionType##_##function *>(Ion::Device::Trampoline::addressOfFunction(index)); \
    return (*trampolineFunction)argsList; \
  } \

TRAMPOLINE_INTERFACE(TRAMPOLINE_MEMCMP, memcmp, (a,b,c), int, const void * a, const void * b, size_t c)
TRAMPOLINE_INTERFACE(TRAMPOLINE_MEMCPY, memcpy, (a,b,c), void *, void * a, const void * b, size_t c)
TRAMPOLINE_INTERFACE(TRAMPOLINE_MEMMOVE, memmove, (a,b,c), void *, void * a, const void * b, size_t c)
// Work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=51205
void * memset(void * b, int c, size_t len) __attribute__((externally_visible));
TRAMPOLINE_INTERFACE(TRAMPOLINE_MEMSET, memset, (a,b,c), void *, void * a, int b, size_t c)
TRAMPOLINE_INTERFACE(TRAMPOLINE_STRCHR, strchr, (a,b), char *, const char * a, int b)
TRAMPOLINE_INTERFACE(TRAMPOLINE_STRCMP, strcmp, (a,b), int, const char * a, const char * b)
TRAMPOLINE_INTERFACE(TRAMPOLINE_STRLCAT, strlcat, (a,b,c), size_t, char * a, const char * b, size_t c)
TRAMPOLINE_INTERFACE(TRAMPOLINE_STRLCPY, strlcpy, (a,b,c), size_t, char * a, const char * b, size_t c)
TRAMPOLINE_INTERFACE(TRAMPOLINE_STRLEN, strlen, (a), size_t, const char * a)
TRAMPOLINE_INTERFACE(TRAMPOLINE_STRNCMP, strncmp, (a,b,c), int, const char * a, const char * b, size_t c)

#if 0
typedef int (*MemcmpFunction)(const void *, const const void *, size_t);

int memcmp(const void * s1, const void * s2, size_t n) {
  MemcmpFunction * trampolineFunction = reinterpret_cast<MemcmpFunction *>(Ion::Device::Trampoline::addressOfFunction(TRAMPOLINE_MEMCMP));
  return (*trampolineFunction)(s1, s2, n);
}

typedef void * (*MemcpyFunction)(void *, const void *, size_t);

void * __attribute__((noinline)) memcpy(void * dst, const void * src, size_t n) {
  MemcpyFunction * trampolineFunction = reinterpret_cast<MemcpyFunction *>(Ion::Device::Trampoline::addressOfFunction(TRAMPOLINE_MEMCPY));
  return (*trampolineFunction)(dst, src, n);
}

void * memmove(void * dst, const void * src, size_t n) {
  MemcpyFunction * trampolineFunction = reinterpret_cast<MemcpyFunction *>(Ion::Device::Trampoline::addressOfFunction(TRAMPOLINE_MEMMOVE));
  return (*trampolineFunction)(dst, src, n);
}

typedef void * (*MemsetFunction)(void *, int, size_t);

void * __attribute__((noinline)) memset(void * des, int c, size_t n) {
  MemsetFunction * trampolineFunction = reinterpret_cast<MemsetFunction *>(Ion::Device::Trampoline::addressOfFunction(TRAMPOLINE_MEMSET));
  return (*trampolineFunction)(dst, c, n);
}

typedef void * (*MemsetFunction)(void *, int, size_t);

void * __attribute__((noinline)) memset(void * des, int c, size_t n) {
  MemsetFunction * trampolineFunction = reinterpret_cast<MemsetFunction *>(Ion::Device::Trampoline::addressOfFunction(TRAMPOLINE_MEMSET));
  return (*trampolineFunction)(dst, c, n);
}
#endif
