#include "../../../omg/include/omg/global_box.h"

// Valid cases
extern int a;         // No (.bss) symbol is generated at this point
constexpr int b = 0;  // This should go in .rodata
class c {
  static int d;  // No symbol is generated at this point
};
OMG::GlobalBox<int> e;

// Invalid cases
int a;
int c::d;
int f;
static int g;
void h() { static int i; }
