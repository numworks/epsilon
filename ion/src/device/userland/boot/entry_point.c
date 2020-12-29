#include "entry_point.h"

typedef void (*EntryPoint)(void);

EntryPoint entry[1] __attribute__((section(".entry_point"), used)) = { start };
