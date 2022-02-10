#ifndef ESCHER_STACK_H
#define ESCHER_STACK_H

namespace Escher {

// All implementations are in header to avoid having to specify templates

template<typename T, int N>
class Stack {
public:
  Stack() : m_depth(0) {}

  void push(T element) {
    assert(m_depth < N);
    m_stack[m_depth++] = element;
  }

  T * elementAtIndex(int index) {
    assert(index >= 0 && index < m_depth);
    return &m_stack[index];
  }

  T pop() {
    assert(m_depth > 0);
    return m_stack[m_depth-- - 1];
  }

  int depth() { return m_depth; }

  void resetStack() { m_depth = 0; }

private:
  T m_stack[N];
  int m_depth;
};

}

#endif
