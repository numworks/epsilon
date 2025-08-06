#include "rt0.h"

namespace Ion {
namespace Device {
namespace Init {

void configureGlobalVariables() {
  /* In practice, static initialized objects are a terrible idea. Since the init
   * order is not specified, most often than not this yields the dreaded static
   * init order fiasco. The linker asserts that the .init_array section is
   * empty. */
}

}  // namespace Init
}  // namespace Device
}  // namespace Ion
