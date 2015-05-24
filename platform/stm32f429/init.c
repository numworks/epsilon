#include "init_lcd.h"
#include "init_heap.h"
#include "init_kbd.h"
#include <src/hello.h>

void init() {
  init_kbd();
  init_lcd();
  init_heap();
  hello();
}
