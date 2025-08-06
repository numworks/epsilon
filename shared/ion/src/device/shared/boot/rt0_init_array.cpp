#include "rt0.h"

typedef void (*cxx_constructor)();

extern "C" {
extern cxx_constructor _init_array_start;
extern cxx_constructor _init_array_end;
}

namespace Ion {
namespace Device {
namespace Init {

void configureGlobalVariables() {
  /* Call static C++ object constructors
   * The C++ compiler creates an initialization function for each static object.
   * The linker then stores the address of each of those functions consecutively
   * between _init_array_start and _init_array_end. So to initialize all C++
   * static objects we just have to iterate between theses two addresses and
   * call the pointed function. */
  for (cxx_constructor* c = &_init_array_start; c < &_init_array_end; c++) {
    (*c)();
  }
}

}  // namespace Init
}  // namespace Device
}  // namespace Ion
