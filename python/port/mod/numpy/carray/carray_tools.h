
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Zoltán Vörös
*/

#ifndef _CARRAY_TOOLS_
#define _CARRAY_TOOLS_

void raise_complex_NotImplementedError(void);

#if ULAB_SUPPORTS_COMPLEX
    #define NOT_IMPLEMENTED_FOR_COMPLEX() raise_complex_NotImplementedError();
    #define COMPLEX_DTYPE_NOT_IMPLEMENTED(dtype) if((dtype) == NDARRAY_COMPLEX) raise_complex_NotImplementedError();
#else
    #define NOT_IMPLEMENTED_FOR_COMPLEX() // do nothing
    #define COMPLEX_DTYPE_NOT_IMPLEMENTED(dtype) // do nothing
#endif

#endif
