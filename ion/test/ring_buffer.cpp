#include <ion/ring_buffer.h>
#include <quiz.h>
#include <string.h>

using namespace Ion;

constexpr int k_ringSize = 6;

void assert_ring_buffer_is(RingBuffer<int, k_ringSize>& ringBuffer, int* state,
                           size_t stateLength) {
  quiz_assert(ringBuffer.length() == stateLength);
  for (size_t i = 0; i < stateLength; i++) {
    quiz_assert(*ringBuffer.elementAtIndex(i) == state[i]);
  }
}

void fill_ring(RingBuffer<int, k_ringSize>* ringBuffer) {
  for (int i = 0; i < k_ringSize; i++) {
    ringBuffer->push(i);
  }
}

QUIZ_CASE(escher_ring_buffer) {
  RingBuffer<int, k_ringSize> ringBuffer;

  fill_ring(&ringBuffer);
  int state[k_ringSize] = {0, 1, 2, 3, 4, 5};
  for (int i = k_ringSize - 1; i >= 0; i--) {
    assert_ring_buffer_is(ringBuffer, state, i + 1);
    quiz_assert(ringBuffer.stackPop() == i);
  }
  quiz_assert(ringBuffer.length() == 0);

  fill_ring(&ringBuffer);
  for (int i = 0; i < k_ringSize; i++) {
    assert_ring_buffer_is(ringBuffer, state + i, k_ringSize - i);
    quiz_assert(ringBuffer.queuePop() == i);
  }
  quiz_assert(ringBuffer.length() == 0);

  fill_ring(&ringBuffer);
  quiz_assert(ringBuffer.length() == k_ringSize);
  ringBuffer.reset();
  quiz_assert(ringBuffer.length() == 0);
}
