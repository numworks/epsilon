/* See the "Run-time ABI for the ARM Architecture", Section 4.4.5 */

int __cxa_atexit(void (* dtor)(void * this), void * object, void * handle);

int  __aeabi_atexit(void * object, void (*destroyer)(void *), void * dso_handle) {
  return __cxa_atexit(destroyer, object, dso_handle);
}
