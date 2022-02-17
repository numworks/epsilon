#ifndef ESCHER_RING_BUFFER_H
#define ESCHER_RING_BUFFER_H

#include <assert.h>

namespace Escher {

// All implementations are in header to avoid having to specify templates

template<typename T, int N>
class RingBuffer {
public:
  // N is a special value used for empty ring buffer
  RingBuffer() : m_start(N), m_end(N) {}

  void push(T element) {
    assert(length() < N);
    m_start = m_start % N;
    m_stack[m_end % N] = element;
    m_end = (m_end + 1) % N;
  }

  T * elementAtIndex(int index) {
    assert(index >= 0 && index < length());
    return &m_stack[(m_start + index) % N];
  }

  T stackPop() {
    assert(length() > 0);
    m_end = (m_end + N - 1) % N;
    resetIfEmpy();
    return m_stack[m_end];
  }

  T queuePop() {
    assert(length() > 0);
    T res = m_stack[m_start];
    m_start = (m_start + 1) % N;
    resetIfEmpy();
    return res;
  }

  int length() {
    if (m_start == N) {
      assert(m_end == N);
      return 0;
    }
    return m_start < m_end ? m_end - m_start : m_end + N - m_start; }

  void reset() { m_start = N; m_end = N; }

private:
  void resetIfEmpy() {
    if (m_start == m_end) {
      reset();
    }
  }
  T m_stack[N];
  int m_start;
  int m_end;
};

}

#endif
