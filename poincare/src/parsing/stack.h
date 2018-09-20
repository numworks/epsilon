#ifndef POINCARE_PARSING_STACK_H
#define POINCARE_PARSING_STACK_H

#include <assert.h>

namespace Poincare {

template <class T, int maxSize>
class Stack {
public:
  Stack() : m_nextItem(m_stack) {};
  int push(T item) {
    assert(m_nextItem >= m_stack);
    if (m_nextItem >= m_stack + maxSize) {
      return -1;
    }
    *m_nextItem = item;
    m_nextItem++;
    return 0;
  }
  T pop() {
    assert(m_nextItem > m_stack);
    m_nextItem--;
    return *m_nextItem;
  }
  T top() const {
    assert(m_nextItem > m_stack);
    assert(m_nextItem < m_stack + maxSize);
    return *(m_nextItem-1);
  }
  bool isEmpty() const { return (m_stack == m_nextItem); };
  int size() const { return m_nextItem - m_stack; }
private:
  T m_stack[maxSize];
  T * m_nextItem;
};

}

#endif
