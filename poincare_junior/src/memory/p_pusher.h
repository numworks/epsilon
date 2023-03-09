#ifndef POINCARE_MEMORY_P_PUSHER_H
#define POINCARE_MEMORY_P_PUSHER_H

// Helper to count VA_ARGS
// https://stackoverflow.com/questions/2124339/c-preprocessor-va-args-number-of-arguments

#define _PP_NARG(...) _PP_NARG_(__VA_ARGS__, _PP_RSEQ_N())
#define _PP_NARG_(...) _PP_ARG_N(__VA_ARGS__)
#define _PP_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define _PP_RSEQ_N() 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define _BINARY_PUSHER(TAG, A, B)                                \
  ([&]() {                                                \
    EditionReference ref = EditionReference::Push<TAG>(); \
    A;                                                    \
    B;                                                    \
    return ref;                                           \
  }())

#define _NARY_PUSHER(TAG, ...)                                                        \
  ([&]() {                                                                     \
    EditionReference ref = EditionReference::Push<TAG>(_PP_NARG(__VA_ARGS__)); \
    __VA_ARGS__;                                                               \
    return ref;                                                                \
  }())

#endif
