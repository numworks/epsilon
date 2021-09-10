#ifndef APPS_PROBABILITY_HELPERS_H
#define APPS_PROBABILITY_HELPERS_H

#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <shared/poincare_helpers.h>

namespace Probability {

template <class T>
constexpr T constexpr_max(T a, T b) {
  return a < b ? b : a;
}

constexpr static int max_between(const int * begin, const int * end) {
  return begin + 1 == end ? *begin : constexpr_max(*begin, max_between(begin + 1, end));
}

template <int N>
constexpr static int arrayMax(const int (&data)[N]) {
  return max_between(data, data + N);
}

}  // namespace Probability

#endif /* APPS_PROBABILITY_HELPERS_H */
