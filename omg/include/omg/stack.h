#ifndef OMG_STACK_H
#define OMG_STACK_H

#include <assert.h>
#include <stddef.h>

namespace OMG {

// All implementations are in header to avoid having to specify templates

template <typename T>
class AbstractStack {
 public:
  virtual T* elementAtIndex(int i) = 0;

  void push(T element) { *elementAtIndex(m_size++) = element; }

  T pop() {
    assert(m_size > 0);
    return *elementAtIndex(--m_size);
  }

  size_t size() const { return m_size; }
  void reset() { m_size = 0; }

 private:
  int m_size = 0;
};

template <typename T, int N>
class Stack : public AbstractStack<T> {
  static_assert(N > 0);

 public:
  constexpr static int k_maxSize = N;

  virtual T* elementAtIndex(int i) {
    assert(i < N);
    assert(i >= 0);
    return &m_content[i];
  }

 private:
  T m_content[N];
};

}  // namespace OMG

#endif
