#ifndef POINCARE_TEST_FLOAT_HELPER_H
#define POINCARE_TEST_FLOAT_HELPER_H

#include <omg/float.h>

// TODO find more distinctive names
template <typename T>
bool relatively_equal(T observed, T expected, T relativeThreshold);

/* Return true if observed and expected are approximately equal, according to
 * threshold and acceptNAN parameters. */
template <typename T>
bool roughly_equal(T observed, T expected,
                   T threshold = OMG::Float::Epsilon<T>(),
                   bool acceptNAN = false, T nullExpectedThreshold = NAN);

template <typename T>
void assert_roughly_equal(T observed, T expected,
                          T threshold = OMG::Float::Epsilon<T>(),
                          bool acceptNAN = false,
                          T nullExpectedThreshold = NAN);
#endif
