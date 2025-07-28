
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Zoltán Vörös
*/

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/misc.h"

#include "../../ulab.h"
#include "../../ndarray.h"

#if ULAB_SUPPORTS_COMPLEX

void raise_complex_NotImplementedError(void) {
    mp_raise_NotImplementedError(translate("not implemented for complex dtype"));
}

#endif
