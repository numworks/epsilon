#include <stdlib.h>

// See the C++ standard, section 3.7.4 for those definitions

void* operator new(size_t size) { return malloc(size); }

void operator delete(void* ptr) noexcept { free(ptr); }

void* operator new[](size_t size) { return ::operator new(size); }

void operator delete[](void* ptr) noexcept { ::operator delete(ptr); }
