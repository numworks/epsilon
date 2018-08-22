#ifndef POINCARE_EXCEPTION_NODE_H
#define POINCARE_EXCEPTION_NODE_H

#include <stddef.h>

namespace Poincare {

template <typename T>
class ExceptionNode : public T {
public:
  using T::T;

  // TreeNode
  size_t size() const override { return sizeof(ExceptionNode<T>); }
  int numberOfChildren() const override { return 0; }
};

}

#endif
