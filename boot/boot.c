#if USE_LIBA
#include <liba.h>
#endif

#if USE_ION
#include <ion.h>
#endif

#include <src/hello.h>

void boot() {
#if USE_LIBA
  liba_init();
#endif
#if USE_ION
  ion_ini();
#endif

  hello();
}
