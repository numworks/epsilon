#ifndef POINCARE_MEMORY_P_PUSHER_H
#define POINCARE_MEMORY_P_PUSHER_H

// Helper to count VA_ARGS
// https://stackoverflow.com/questions/2124339/c-preprocessor-va-args-number-of-arguments

// Returns the number of arguments in __VA_ARGS__
#define _PP_NARG(...) _PP_ARG_N(_0, ## __VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
// Returns the 11th argument it has been called with
#define _PP_ARG_N(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N

/* TODO : Fix _PP_NARG being confused by args having bigger templates like
 *        push<CodePointLayout, CodePoint>('0')) */

#define _UNARY_PUSHER(TAG, A)                             \
  ([&]() {                                                \
    EditionReference ref = EditionReference::Push<TAG>(); \
    A;                                                    \
    return ref;                                           \
  }())

#define _BINARY_PUSHER(TAG, A, B)                         \
  ([&]() {                                                \
    EditionReference ref = EditionReference::Push<TAG>(); \
    A;                                                    \
    B;                                                    \
    return ref;                                           \
  }())

#define _NARY_PUSHER(TAG, ...)                                                 \
  ([&]() {                                                                     \
    EditionReference ref = EditionReference::Push<TAG>(_PP_NARG(__VA_ARGS__)); \
    __VA_ARGS__;                                                               \
    return ref;                                                                \
  }())

#endif
