#include <ion.h>
#include <stdlib.h>

uint32_t Ion::random() {
    static uint32_t seed = 42;
    seed = seed * 1103515245 + 12345;
    return seed;
}
