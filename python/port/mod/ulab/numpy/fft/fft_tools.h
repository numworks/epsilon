/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2021 Zoltán Vörös
*/

#ifndef _FFT_TOOLS_
#define _FFT_TOOLS_

enum FFT_TYPE {
    FFT_FFT,
    FFT_IFFT,
    FFT_SPECTROGRAM,
};

void fft_kernel(mp_float_t *, mp_float_t *, size_t , int );
mp_obj_t fft_fft_ifft_spectrogram(size_t , mp_obj_t , mp_obj_t , uint8_t );

#endif /* _FFT_TOOLS_ */
