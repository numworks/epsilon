#ifndef APPS_PROBABILITY_HELPERS_H
#define APPS_PROBABILITY_HELPERS_H

#include <apps/i18n.h>
#include <apps/shared/text_field_delegate_app.h>
#include <assert.h>
#include <escher/container.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <shared/poincare_helpers.h>

namespace Probability {

constexpr static int max(int a, int b) {
  return a > b ? a : b;
}

constexpr static int max_between(const int * begin, const int * end) {
  return begin + 1 == end ? *begin : max_between(begin + 1, end);
}

template <int N>
constexpr static int arrayMax(const int (&data)[N]) {
  return max_between(data, data + N);
}

inline float fminf(float x, float y) {
  // TODO export to math.h
  return (x < y ? x : y);
}

inline int clipped(int a, int n) {
  return a < n ? a : n;
}

inline Poincare::Layout layoutFromText(const char * text) {
  Poincare::HorizontalLayout layout = Poincare::HorizontalLayout::Builder();
  for (int i = 0; i < strlen(text); i++) {
    Poincare::CodePointLayout l = Poincare::CodePointLayout::Builder(CodePoint(text[i]));
    layout.addChildAtIndex(l, i, i, nullptr);
  }
  return layout;
}

}  // namespace Probability

#endif /* APPS_PROBABILITY_HELPERS_H */
