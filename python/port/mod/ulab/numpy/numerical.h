
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2021 Zoltán Vörös
*/

#ifndef _NUMERICAL_
#define _NUMERICAL_

#include "../ulab.h"
#include "../ndarray.h"

// TODO: implement cumsum

#define RUN_ARGMIN1(ndarray, type, array, results, rarray, index, op)\
({\
    uint16_t best_index = 0;\
    type best_value = *((type *)(array));\
    if(((op) == NUMERICAL_MAX) || ((op) == NUMERICAL_ARGMAX)) {\
        for(uint16_t i=0; i < (ndarray)->shape[(index)]; i++) {\
            if(*((type *)(array)) > best_value) {\
                best_index = i;\
                best_value = *((type *)(array));\
            }\
            (array) += (ndarray)->strides[(index)];\
        }\
    } else {\
        for(uint16_t i=0; i < (ndarray)->shape[(index)]; i++) {\
            if(*((type *)(array)) < best_value) {\
                best_index = i;\
                best_value = *((type *)(array));\
            }\
            (array) += (ndarray)->strides[(index)];\
        }\
    }\
    if(((op) == NUMERICAL_ARGMAX) || ((op) == NUMERICAL_ARGMIN)) {\
        memcpy((rarray), &best_index, (results)->itemsize);\
    } else {\
        memcpy((rarray), &best_value, (results)->itemsize);\
    }\
    (rarray) += (results)->itemsize;\
})

#define RUN_SUM1(type, array, results, rarray, ss)\
({\
    type sum = 0;\
    for(size_t i=0; i < (ss).shape[0]; i++) {\
        sum += *((type *)(array));\
        (array) += (ss).strides[0];\
    }\
    memcpy((rarray), &sum, (results)->itemsize);\
    (rarray) += (results)->itemsize;\
})

// The mean could be calculated by simply dividing the sum by
// the number of elements, but that method is numerically unstable
#define RUN_MEAN1(type, array, rarray, ss)\
({\
    mp_float_t M = 0.0;\
    for(size_t i=0; i < (ss).shape[0]; i++) {\
        mp_float_t value = (mp_float_t)(*(type *)(array));\
        M = M + (value - M) / (mp_float_t)(i+1);\
        (array) += (ss).strides[0];\
    }\
    *(rarray)++ = M;\
})

// Instead of the straightforward implementation of the definition,
// we take the numerically stable Welford algorithm here
// https://www.johndcook.com/blog/2008/09/26/comparing-three-methods-of-computing-standard-deviation/
#define RUN_STD1(type, array, rarray, ss, div)\
({\
    mp_float_t M = 0.0, m = 0.0, S = 0.0;\
    for(size_t i=0; i < (ss).shape[0]; i++) {\
        mp_float_t value = (mp_float_t)(*(type *)(array));\
        m = M + (value - M) / (mp_float_t)(i+1);\
        S = S + (value - M) * (value - m);\
        M = m;\
        (array) += (ss).strides[0];\
    }\
    *(rarray)++ = MICROPY_FLOAT_C_FUN(sqrt)(S / (div));\
})

#define RUN_MEAN_STD1(type, array, rarray, ss, div, isStd)\
({\
    mp_float_t M = 0.0, m = 0.0, S = 0.0;\
    for(size_t i=0; i < (ss).shape[0]; i++) {\
        mp_float_t value = (mp_float_t)(*(type *)(array));\
        m = M + (value - M) / (mp_float_t)(i+1);\
        if(isStd) {\
            S += (value - M) * (value - m);\
        }\
        M = m;\
        (array) += (ss).strides[0];\
    }\
    *(rarray)++ = isStd ? MICROPY_FLOAT_C_FUN(sqrt)(S / (div)) : M;\
})

#define RUN_DIFF1(ndarray, type, array, results, rarray, index, stencil, N)\
({\
    for(size_t i=0; i < (results)->shape[ULAB_MAX_DIMS - 1]; i++) {\
        type sum = 0;\
        uint8_t *source = (array);\
        for(uint8_t d=0; d < (N)+1; d++) {\
            sum -= (stencil)[d] * *((type *)source);\
            source += (ndarray)->strides[(index)];\
        }\
        (array) += (ndarray)->strides[ULAB_MAX_DIMS - 1];\
        *(type *)(rarray) = sum;\
        (rarray) += (results)->itemsize;\
    }\
})

#define HEAPSORT1(type, array, increment, N)\
({\
    type *_array = (type *)array;\
    type tmp;\
    size_t c, q = (N), p, r = (N) >> 1;\
    for (;;) {\
        if (r > 0) {\
            tmp = _array[(--r)*(increment)];\
        } else {\
            q--;\
            if(q == 0) {\
                break;\
            }\
            tmp = _array[q*(increment)];\
            _array[q*(increment)] = _array[0];\
        }\
        p = r;\
        c = r + r + 1;\
        while (c < q) {\
            if((c + 1 < q)  &&  (_array[(c+1)*(increment)] > _array[c*(increment)])) {\
                c++;\
            }\
            if(_array[c*(increment)] > tmp) {\
                _array[p*(increment)] = _array[c*(increment)];\
                p = c;\
                c = p + p + 1;\
            } else {\
                break;\
            }\
        }\
        _array[p*(increment)] = tmp;\
    }\
})

#define HEAP_ARGSORT1(type, array, increment, N, iarray, iincrement)\
({\
    type *_array = (type *)array;\
    type tmp;\
    uint16_t itmp, c, q = (N), p, r = (N) >> 1;\
    for (;;) {\
        if (r > 0) {\
            r--;\
            itmp = (iarray)[r*(iincrement)];\
            tmp = _array[itmp*(increment)];\
        } else {\
            q--;\
            if(q == 0) {\
                break;\
            }\
            itmp = (iarray)[q*(iincrement)];\
            tmp = _array[itmp*(increment)];\
            (iarray)[q*(iincrement)] = (iarray)[0];\
        }\
        p = r;\
        c = r + r + 1;\
        while (c < q) {\
            if((c + 1 < q)  &&  (_array[(iarray)[(c+1)*(iincrement)]*(increment)] > _array[(iarray)[c*(iincrement)]*(increment)])) {\
                c++;\
            }\
            if(_array[(iarray)[c*(iincrement)]*(increment)] > tmp) {\
                (iarray)[p*(iincrement)] = (iarray)[c*(iincrement)];\
                p = c;\
                c = p + p + 1;\
            } else {\
                break;\
            }\
        }\
        (iarray)[p*(iincrement)] = itmp;\
    }\
})

#if ULAB_MAX_DIMS == 1
#define RUN_SUM(type, array, results, rarray, ss) do {\
    RUN_SUM1(type, (array), (results), (rarray), (ss));\
} while(0)

#define RUN_MEAN(type, array, rarray, ss) do {\
    RUN_MEAN1(type, (array), (rarray), (ss));\
} while(0)

#define RUN_STD(type, array, rarray, ss, div) do {\
    RUN_STD1(type, (array), (results), (rarray), (ss), (div));\
} while(0)

#define RUN_MEAN_STD(type, array, rarray, ss, div, isStd) do {\
    RUN_MEAN_STD1(type, (array), (rarray), (ss), (div), (isStd));\
} while(0)

#define RUN_ARGMIN(ndarray, type, array, results, rarray, shape, strides, index, op) do {\
    RUN_ARGMIN1((ndarray), type, (array), (results), (rarray), (index), (op));\
} while(0)

#define RUN_DIFF(ndarray, type, array, results, rarray, shape, strides, index, stencil, N) do {\
    RUN_DIFF1((ndarray), type, (array), (results), (rarray), (index), (stencil), (N));\
} while(0)

#define HEAPSORT(ndarray, type, array, shape, strides, index, increment, N) do {\
    HEAPSORT1(type, (array), (increment), (N));\
} while(0)

#define HEAP_ARGSORT(ndarray, type, array, shape, strides, index, increment, N, iarray, istrides, iincrement) do {\
    HEAP_ARGSORT1(type, (array), (increment), (N), (iarray), (iincrement));\
} while(0)

#endif

#if ULAB_MAX_DIMS == 2
#define RUN_SUM(type, array, results, rarray, ss) do {\
    size_t l = 0;\
    do {\
        RUN_SUM1(type, (array), (results), (rarray), (ss));\
        (array) -= (ss).strides[0] * (ss).shape[0];\
        (array) += (ss).strides[ULAB_MAX_DIMS - 1];\
        l++;\
    } while(l < (ss).shape[ULAB_MAX_DIMS - 1]);\
} while(0)

#define RUN_MEAN(type, array, rarray, ss) do {\
    size_t l = 0;\
    do {\
        RUN_MEAN1(type, (array), (rarray), (ss));\
        (array) -= (ss).strides[0] * (ss).shape[0];\
        (array) += (ss).strides[ULAB_MAX_DIMS - 1];\
        l++;\
    } while(l < (ss).shape[ULAB_MAX_DIMS - 1]);\
} while(0)

#define RUN_STD(type, array, rarray, ss, div) do {\
    size_t l = 0;\
    do {\
        RUN_STD1(type, (array), (rarray), (ss), (div));\
        (array) -= (ss).strides[0] * (ss).shape[0];\
        (array) += (ss).strides[ULAB_MAX_DIMS - 1];\
        l++;\
    } while(l < (ss).shape[ULAB_MAX_DIMS - 1]);\
} while(0)

#define RUN_MEAN_STD(type, array, rarray, ss, div, isStd) do {\
    size_t l = 0;\
    do {\
        RUN_MEAN_STD1(type, (array), (rarray), (ss), (div), (isStd));\
        (array) -= (ss).strides[0] * (ss).shape[0];\
        (array) += (ss).strides[ULAB_MAX_DIMS - 1];\
        l++;\
    } while(l < (ss).shape[ULAB_MAX_DIMS - 1]);\
} while(0)


#define RUN_ARGMIN(ndarray, type, array, results, rarray, shape, strides, index, op) do {\
    size_t l = 0;\
    do {\
        RUN_ARGMIN1((ndarray), type, (array), (results), (rarray), (index), (op));\
        (array) -= (ndarray)->strides[(index)] * (ndarray)->shape[(index)];\
        (array) += (strides)[ULAB_MAX_DIMS - 1];\
        l++;\
    } while(l < (shape)[ULAB_MAX_DIMS - 1]);\
} while(0)

#define RUN_DIFF(ndarray, type, array, results, rarray, shape, strides, index, stencil, N) do {\
    size_t l = 0;\
    do {\
        RUN_DIFF1((ndarray), type, (array), (results), (rarray), (index), (stencil), (N));\
        (array) -= (ndarray)->strides[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS - 1];\
        (array) += (ndarray)->strides[ULAB_MAX_DIMS - 2];\
        (rarray) -= (results)->strides[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS - 1];\
        (rarray) += (results)->strides[ULAB_MAX_DIMS - 2];\
        l++;\
    } while(l < (results)->shape[ULAB_MAX_DIMS - 2]);\
} while(0)

#define HEAPSORT(ndarray, type, array, shape, strides, index, increment, N) do {\
    size_t l = 0;\
    do {\
        HEAPSORT1(type, (array), (increment), (N));\
        (array) += (strides)[ULAB_MAX_DIMS - 1];\
        l++;\
    } while(l < (shape)[ULAB_MAX_DIMS - 1]);\
} while(0)

#define HEAP_ARGSORT(ndarray, type, array, shape, strides, index, increment, N, iarray, istrides, iincrement) do {\
    size_t l = 0;\
    do {\
        HEAP_ARGSORT1(type, (array), (increment), (N), (iarray), (iincrement));\
        (array) += (strides)[ULAB_MAX_DIMS - 1];\
        (iarray) += (istrides)[ULAB_MAX_DIMS - 1];\
        l++;\
    } while(l < (shape)[ULAB_MAX_DIMS - 1]);\
} while(0)

#endif

#if ULAB_MAX_DIMS == 3
#define RUN_SUM(type, array, results, rarray, ss) do {\
    size_t k = 0;\
    do {\
        size_t l = 0;\
        do {\
            RUN_SUM1(type, (array), (results), (rarray), (ss));\
            (array) -= (ss).strides[0] * (ss).shape[0];\
            (array) += (ss).strides[ULAB_MAX_DIMS - 1];\
            l++;\
        } while(l < (ss).shape[ULAB_MAX_DIMS - 1]);\
        (array) -= (ss).strides[ULAB_MAX_DIMS - 1] * (ss).shape[ULAB_MAX_DIMS - 1];\
        (array) += (ss).strides[ULAB_MAX_DIMS - 2];\
        k++;\
    } while(k < (ss).shape[ULAB_MAX_DIMS - 2]);\
} while(0)

#define RUN_MEAN(type, array, rarray, ss) do {\
    size_t k = 0;\
    do {\
        size_t l = 0;\
        do {\
            RUN_MEAN1(type, (array), (rarray), (ss));\
            (array) -= (ss).strides[0] * (ss).shape[0];\
            (array) += (ss).strides[ULAB_MAX_DIMS - 1];\
            l++;\
        } while(l < (ss).shape[ULAB_MAX_DIMS - 1]);\
        (array) -= (ss).strides[ULAB_MAX_DIMS - 1] * (ss).shape[ULAB_MAX_DIMS - 1];\
        (array) += (ss).strides[ULAB_MAX_DIMS - 2];\
        k++;\
    } while(k < (ss).shape[ULAB_MAX_DIMS - 2]);\
} while(0)

#define RUN_STD(type, array, rarray, ss, div) do {\
    size_t k = 0;\
    do {\
        size_t l = 0;\
        do {\
            RUN_STD1(type, (array), (rarray), (ss), (div));\
            (array) -= (ss).strides[0] * (ss).shape[0];\
            (array) += (ss).strides[ULAB_MAX_DIMS - 1];\
            l++;\
        } while(l < (ss).shape[ULAB_MAX_DIMS - 1]);\
        (array) -= (ss).strides[ULAB_MAX_DIMS - 1] * (ss).shape[ULAB_MAX_DIMS - 1];\
        (array) += (ss).strides[ULAB_MAX_DIMS - 2];\
        k++;\
    } while(k < (ss).shape[ULAB_MAX_DIMS - 2]);\
} while(0)

#define RUN_MEAN_STD(type, array, rarray, ss, div, isStd) do {\
    size_t k = 0;\
    do {\
        size_t l = 0;\
        do {\
            RUN_MEAN_STD1(type, (array), (rarray), (ss), (div), (isStd));\
            (array) -= (ss).strides[0] * (ss).shape[0];\
            (array) += (ss).strides[ULAB_MAX_DIMS - 1];\
            l++;\
        } while(l < (ss).shape[ULAB_MAX_DIMS - 1]);\
        (array) -= (ss).strides[ULAB_MAX_DIMS - 1] * (ss).shape[ULAB_MAX_DIMS - 1];\
        (array) += (ss).strides[ULAB_MAX_DIMS - 2];\
        k++;\
    } while(k < (ss).shape[ULAB_MAX_DIMS - 2]);\
} while(0)

#define RUN_ARGMIN(ndarray, type, array, results, rarray, shape, strides, index, op) do {\
    size_t k = 0;\
    do {\
        size_t l = 0;\
        do {\
            RUN_ARGMIN1((ndarray), type, (array), (results), (rarray), (index), (op));\
            (array) -= (ndarray)->strides[(index)] * (ndarray)->shape[(index)];\
            (array) += (strides)[ULAB_MAX_DIMS - 1];\
            l++;\
        } while(l < (shape)[ULAB_MAX_DIMS - 1]);\
        (array) -= (strides)[ULAB_MAX_DIMS - 1] * (shape)[ULAB_MAX_DIMS-1];\
        (array) += (strides)[ULAB_MAX_DIMS - 2];\
        k++;\
    } while(k < (shape)[ULAB_MAX_DIMS - 2]);\
} while(0)

#define RUN_DIFF(ndarray, type, array, results, rarray, shape, strides, index, stencil, N) do {\
    size_t k = 0;\
    do {\
        size_t l = 0;\
        do {\
            RUN_DIFF1((ndarray), type, (array), (results), (rarray), (index), (stencil), (N));\
            (array) -= (ndarray)->strides[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS - 1];\
			(array) += (ndarray)->strides[ULAB_MAX_DIMS - 2];\
            (rarray) -= (results)->strides[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS - 1];\
            (rarray) += (results)->strides[ULAB_MAX_DIMS - 2];\
            l++;\
        } while(l < (shape)[ULAB_MAX_DIMS - 2]);\
        (array) -= (ndarray)->strides[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS-2];\
        (array) += (ndarray)->strides[ULAB_MAX_DIMS - 3];\
        (rarray) -= (results)->strides[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS - 2];\
        (rarray) += (results)->strides[ULAB_MAX_DIMS - 3];\
        k++;\
    } while(k < (shape)[ULAB_MAX_DIMS - 3]);\
} while(0)

#define HEAPSORT(ndarray, type, array, shape, strides, index, increment, N) do {\
    size_t k = 0;\
    do {\
        size_t l = 0;\
        do {\
            HEAPSORT1(type, (array), (increment), (N));\
            (array) += (strides)[ULAB_MAX_DIMS - 1];\
            l++;\
        } while(l < (shape)[ULAB_MAX_DIMS - 1]);\
        (array) -= (strides)[ULAB_MAX_DIMS - 1] * (shape)[ULAB_MAX_DIMS-1];\
        (array) += (strides)[ULAB_MAX_DIMS - 2];\
        k++;\
    } while(k < (shape)[ULAB_MAX_DIMS - 2]);\
} while(0)

#define HEAP_ARGSORT(ndarray, type, array, shape, strides, index, increment, N, iarray, istrides, iincrement) do {\
    size_t k = 0;\
    do {\
        size_t l = 0;\
        do {\
            HEAP_ARGSORT1(type, (array), (increment), (N), (iarray), (iincrement));\
            (array) += (strides)[ULAB_MAX_DIMS - 1];\
            (iarray) += (istrides)[ULAB_MAX_DIMS - 1];\
            l++;\
        } while(l < (shape)[ULAB_MAX_DIMS - 1]);\
        (iarray) -= (istrides)[ULAB_MAX_DIMS - 1] * (shape)[ULAB_MAX_DIMS-1];\
        (iarray) += (istrides)[ULAB_MAX_DIMS - 2];\
        (array) -= (strides)[ULAB_MAX_DIMS - 1] * (shape)[ULAB_MAX_DIMS-1];\
        (array) += (strides)[ULAB_MAX_DIMS - 2];\
        k++;\
    } while(k < (shape)[ULAB_MAX_DIMS - 2]);\
} while(0)

#endif

#if ULAB_MAX_DIMS == 4
#define RUN_SUM(type, array, results, rarray, ss) do {\
    size_t j = 0;\
    do {\
        size_t k = 0;\
        do {\
            size_t l = 0;\
            do {\
                RUN_SUM1(type, (array), (results), (rarray), (ss));\
                (array) -= (ss).strides[0] * (ss).shape[0];\
                (array) += (ss).strides[ULAB_MAX_DIMS - 1];\
                l++;\
            } while(l < (ss).shape[ULAB_MAX_DIMS - 1]);\
            (array) -= (ss).strides[ULAB_MAX_DIMS - 1] * (ss).shape[ULAB_MAX_DIMS - 1];\
            (array) += (ss).strides[ULAB_MAX_DIMS - 2];\
            k++;\
        } while(k < (ss).shape[ULAB_MAX_DIMS - 2]);\
        (array) -= (ss).strides[ULAB_MAX_DIMS - 2] * (ss).shape[ULAB_MAX_DIMS - 2];\
        (array) += (ss).strides[ULAB_MAX_DIMS - 3];\
        j++;\
    } while(j < (ss).shape[ULAB_MAX_DIMS - 3]);\
} while(0)

#define RUN_MEAN(type, array, rarray, ss) do {\
    size_t j = 0;\
    do {\
        size_t k = 0;\
        do {\
            size_t l = 0;\
            do {\
                RUN_MEAN1(type, (array), (rarray), (ss));\
                (array) -= (ss).strides[0] * (ss).shape[0];\
                (array) += (ss).strides[ULAB_MAX_DIMS - 1];\
                l++;\
            } while(l < (ss).shape[ULAB_MAX_DIMS - 1]);\
            (array) -= (ss).strides[ULAB_MAX_DIMS - 1] * (ss).shape[ULAB_MAX_DIMS - 1];\
            (array) += (ss).strides[ULAB_MAX_DIMS - 2];\
            k++;\
        } while(k < (ss).shape[ULAB_MAX_DIMS - 2]);\
        (array) -= (ss).strides[ULAB_MAX_DIMS - 2] * (ss).shape[ULAB_MAX_DIMS - 2];\
        (array) += (ss).strides[ULAB_MAX_DIMS - 3];\
        j++;\
    } while(j < (ss).shape[ULAB_MAX_DIMS - 3]);\
} while(0)

#define RUN_STD(type, array, rarray, ss, div) do {\
    size_t j = 0;\
    do {\
        size_t k = 0;\
        do {\
            size_t l = 0;\
            do {\
                RUN_STD1(type, (array), (rarray), (ss), (div));\
                (array) -= (ss).strides[0] * (ss).shape[0];\
                (array) += (ss).strides[ULAB_MAX_DIMS - 1];\
                l++;\
            } while(l < (ss).shape[ULAB_MAX_DIMS - 1]);\
            (array) -= (ss).strides[ULAB_MAX_DIMS - 1] * (ss).shape[ULAB_MAX_DIMS - 1];\
            (array) += (ss).strides[ULAB_MAX_DIMS - 2];\
            k++;\
        } while(k < (ss).shape[ULAB_MAX_DIMS - 2]);\
        (array) -= (ss).strides[ULAB_MAX_DIMS - 2] * (ss).shape[ULAB_MAX_DIMS - 2];\
        (array) += (ss).strides[ULAB_MAX_DIMS - 3];\
        j++;\
    } while(j < (ss).shape[ULAB_MAX_DIMS - 3]);\
} while(0)

#define RUN_MEAN_STD(type, array, rarray, ss, div, isStd) do {\
    size_t j = 0;\
    do {\
        size_t k = 0;\
        do {\
            size_t l = 0;\
            do {\
                RUN_MEAN_STD1(type, (array), (rarray), (ss), (div), (isStd));\
                (array) -= (ss).strides[0] * (ss).shape[0];\
                (array) += (ss).strides[ULAB_MAX_DIMS - 1];\
                l++;\
            } while(l < (ss).shape[ULAB_MAX_DIMS - 1]);\
            (array) -= (ss).strides[ULAB_MAX_DIMS - 1] * (ss).shape[ULAB_MAX_DIMS - 1];\
            (array) += (ss).strides[ULAB_MAX_DIMS - 2];\
            k++;\
        } while(k < (ss).shape[ULAB_MAX_DIMS - 2]);\
        (array) -= (ss).strides[ULAB_MAX_DIMS - 2] * (ss).shape[ULAB_MAX_DIMS - 2];\
        (array) += (ss).strides[ULAB_MAX_DIMS - 3];\
        j++;\
    } while(j < (ss).shape[ULAB_MAX_DIMS - 3]);\
} while(0)

#define RUN_ARGMIN(ndarray, type, array, results, rarray, shape, strides, index, op) do {\
    size_t j = 0;\
    do {\
        size_t k = 0;\
        do {\
            size_t l = 0;\
            do {\
                RUN_ARGMIN1((ndarray), type, (array), (results), (rarray), (index), (op));\
                (array) -= (ndarray)->strides[(index)] * (ndarray)->shape[(index)];\
                (array) += (strides)[ULAB_MAX_DIMS - 1];\
                l++;\
            } while(l < (shape)[ULAB_MAX_DIMS - 1]);\
            (array) -= (strides)[ULAB_MAX_DIMS - 1] * (shape)[ULAB_MAX_DIMS-1];\
            (array) += (strides)[ULAB_MAX_DIMS - 2];\
            k++;\
        } while(k < (shape)[ULAB_MAX_DIMS - 2]);\
        (array) -= (strides)[ULAB_MAX_DIMS - 2] * (shape)[ULAB_MAX_DIMS-2];\
        (array) += (strides)[ULAB_MAX_DIMS - 3];\
        j++;\
    } while(j < (shape)[ULAB_MAX_DIMS - 3]);\
} while(0)

#define RUN_DIFF(ndarray, type, array, results, rarray, shape, strides, index, stencil, N) do {\
    size_t j = 0;\
    do {\
        size_t k = 0;\
        do {\
            size_t l = 0;\
            do {\
                RUN_DIFF1((ndarray), type, (array), (results), (rarray), (index), (stencil), (N));\
                (array) -= (ndarray)->strides[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS - 1];\
                (array) += (ndarray)->strides[ULAB_MAX_DIMS - 2];\
                (rarray) -= (results)->strides[ULAB_MAX_DIMS - 1] * (results)->shape[ULAB_MAX_DIMS - 1];\
                (rarray) += (results)->strides[ULAB_MAX_DIMS - 2];\
                l++;\
            } while(l < (shape)[ULAB_MAX_DIMS - 2]);\
            (array) -= (strides)[ULAB_MAX_DIMS - 2] * (shape)[ULAB_MAX_DIMS-2];\
            (array) += (strides)[ULAB_MAX_DIMS - 3];\
            (rarray) -= (results)->strides[ULAB_MAX_DIMS - 2] * (results)->shape[ULAB_MAX_DIMS - 2];\
            (rarray) += (results)->strides[ULAB_MAX_DIMS - 3];\
            k++;\
        } while(k < (shape)[ULAB_MAX_DIMS - 3]);\
        (array) -= (strides)[ULAB_MAX_DIMS - 3] * (shape)[ULAB_MAX_DIMS-3];\
        (array) += (strides)[ULAB_MAX_DIMS - 4];\
        (rarray) -= (results)->strides[ULAB_MAX_DIMS - 3] * (results)->shape[ULAB_MAX_DIMS - 3];\
        (rarray) += (results)->strides[ULAB_MAX_DIMS - 4];\
        j++;\
    } while(j < (shape)[ULAB_MAX_DIMS - 4]);\
} while(0)

#define HEAPSORT(ndarray, type, array, shape, strides, index, increment, N) do {\
    size_t j = 0;\
    do {\
        size_t k = 0;\
        do {\
            size_t l = 0;\
            do {\
                HEAPSORT1(type, (array), (increment), (N));\
                (array) += (strides)[ULAB_MAX_DIMS - 1];\
                l++;\
            } while(l < (shape)[ULAB_MAX_DIMS - 1]);\
            (array) -= (strides)[ULAB_MAX_DIMS - 1] * (shape)[ULAB_MAX_DIMS-1];\
            (array) += (strides)[ULAB_MAX_DIMS - 2];\
            k++;\
        } while(k < (shape)[ULAB_MAX_DIMS - 2]);\
        (array) -= (strides)[ULAB_MAX_DIMS - 2] * (shape)[ULAB_MAX_DIMS-2];\
        (array) += (strides)[ULAB_MAX_DIMS - 3];\
        j++;\
    } while(j < (shape)[ULAB_MAX_DIMS - 3]);\
} while(0)

#define HEAP_ARGSORT(ndarray, type, array, shape, strides, index, increment, N, iarray, istrides, iincrement) do {\
    size_t j = 0;\
    do {\
        size_t k = 0;\
        do {\
            size_t l = 0;\
            do {\
                HEAP_ARGSORT1(type, (array), (increment), (N), (iarray), (iincrement));\
                (array) += (strides)[ULAB_MAX_DIMS - 1];\
                (iarray) += (istrides)[ULAB_MAX_DIMS - 1];\
                l++;\
            } while(l < (shape)[ULAB_MAX_DIMS - 1]);\
            (iarray) -= (istrides)[ULAB_MAX_DIMS - 1] * (shape)[ULAB_MAX_DIMS-1];\
            (iarray) += (istrides)[ULAB_MAX_DIMS - 2];\
            (array) -= (strides)[ULAB_MAX_DIMS - 1] * (shape)[ULAB_MAX_DIMS-1];\
            (array) += (strides)[ULAB_MAX_DIMS - 2];\
            k++;\
        } while(k < (shape)[ULAB_MAX_DIMS - 2]);\
        (iarray) -= (istrides)[ULAB_MAX_DIMS - 2] * (shape)[ULAB_MAX_DIMS-2];\
        (iarray) += (istrides)[ULAB_MAX_DIMS - 3];\
        (array) -= (strides)[ULAB_MAX_DIMS - 2] * (shape)[ULAB_MAX_DIMS-2];\
        (array) += (strides)[ULAB_MAX_DIMS - 3];\
        j++;\
    } while(j < (shape)[ULAB_MAX_DIMS - 3]);\
} while(0)

#endif

MP_DECLARE_CONST_FUN_OBJ_KW(numerical_all_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(numerical_any_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(numerical_argmax_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(numerical_argmin_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(numerical_argsort_obj);
MP_DECLARE_CONST_FUN_OBJ_2(numerical_cross_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(numerical_diff_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(numerical_flip_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(numerical_max_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(numerical_mean_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(numerical_median_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(numerical_min_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(numerical_roll_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(numerical_std_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(numerical_sum_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(numerical_sort_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(numerical_sort_inplace_obj);

#endif
