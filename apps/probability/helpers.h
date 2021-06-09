#ifndef APPS_PROBABILITY_HELPERS_H
#define APPS_PROBABILITY_HELPERS_H

#include <apps/i18n.h>
#include <apps/shared/text_field_delegate_app.h>
#include <assert.h>
#include <escher/container.h>

namespace Probability {


constexpr static int max(int a, int b) {
  return a > b ? a : b;
}

constexpr static int max_between(const int * begin, const int * end) {
  return begin + 1 == end ? *begin : max_between(begin + 1, end);
}

template<int N>
constexpr static int arrayMax(const int (&data)[N]) {
  return max_between(data, data + N);
}

}  // namespace Probability

#endif /* APPS_PROBABILITY_HELPERS_H */
