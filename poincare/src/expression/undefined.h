#ifndef POINCARE_EXPRESSION_UNDEFINED_H
#define POINCARE_EXPRESSION_UNDEFINED_H

#include <stdint.h>

#include <complex>

namespace Poincare::Internal {

class Tree;

class Undefined {
 public:
  static bool ShallowBubbleUpUndef(Tree* e);
  static bool CanBeUndefined(const Tree* e);
  static bool CanHaveUndefinedChild(const Tree* e, int childIndex);
  template <typename T>
  static bool IsUndefined(std::complex<T> c) {
    return std::isnan(c.real()) || std::isnan(c.imag());
  };
};

}  // namespace Poincare::Internal

#endif
