#if USE_LIBA
#include <liba.h>
#endif

#include <ion.h>
#include <src/hello.h>

void boot() {
#if USE_LIBA
  liba_init();
#endif
  ion_init();
  hello();
}
