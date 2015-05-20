#include "init_lcd.h"
#include "init_heap.h"
#include <src/hello.h>

void init() {
  init_lcd();
  init_heap();
  hello();
}
