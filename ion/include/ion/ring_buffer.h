#ifndef ION_RING_BUFFER_H
#define ION_RING_BUFFER_H

#include <assert.h>
#include <stddef.h>

namespace Ion {

// All implementations are in header to avoid having to specify templates

template<typename T, int N>
class RingBuffer {
public:
  RingBuffer() : m_start(0), m_length(0) {}

  void push(T element) {
    m_stack[nextElementIndex()] = element;
    if (length() < N) {
      m_length++;
    } else {
      m_start = (m_start + 1) % N;
    }
  }

  T stackPop() {
    assert(length() > 0);
    m_length--;
    T res = m_stack[nextElementIndex()];
    return res;
  }

  T queuePop() {
    assert(length() > 0);
    T res = m_stack[m_start];
    m_start = (m_start + 1) % N;
    m_length--;
    return res;
  }

  T * elementAtIndex(int index) {
    assert(index >= 0 && index < static_cast<int>(length()));
    return &m_stack[(m_start + index) % N];
  }

  size_t length() const {
    assert(0 <= m_start && m_start < N);
    assert(0 <= m_length && m_length <= N);
    return m_length;
  }

  bool isEmpty() const { return m_length == 0; }

  void reset() { m_length = 0; }

private:
  int nextElementIndex() const {
    return (m_start + length()) % N;
  }

  T m_stack[N];
  int m_start;
  int m_length;
};

}

#endif
