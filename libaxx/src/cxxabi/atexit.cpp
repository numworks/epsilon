/* C++ expects the __dso_handle symbol to be defined to some unique value in
 * each dynamic shared object. Even though we're not using dynamic loading,
 * we still have to define __dso_handle. */

void *__dso_handle = nullptr;

/* The __cxa_atexit function registers a function to be called when the program
 * exits or when a shared library is unloaded.
 * We don't support shared libraries and our program should never exit, so we
 * can simply do nothing and return zero. */

extern "C" int __cxa_atexit(void (*dtor)(void *), void *object, void *handle) {
  return 0;
}
