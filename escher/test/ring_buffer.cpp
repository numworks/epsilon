#include <quiz.h>
#include <string.h>
#include <escher/ring_buffer.h>

using namespace Escher;

constexpr int k_ringSize = 6;

void assert_ring_buffer_is(RingBuffer<int, k_ringSize> & ringBuffer, int * state, size_t stateLength) {
  for (int i = 0; i < stateLength; i++) {
    quiz_assert(*ringBuffer.elementAtIndex(i) == state[i]);
  }
}

QUIZ_CASE(escher_ring_buffer) {
  RingBuffer<int, k_ringSize> ringBuffer;
  for (int i = 0; i < k_ringSize; i++) {
    ringBuffer.push(i);
  }
  int state1[k_ringSize] = {0, 1, 2, 3, 4, 5};
  assert_ring_buffer_is(ringBuffer, state1, k_ringSize);

  quiz_assert(ringBuffer.stackPop() == 5);
  int state2[k_ringSize - 1] = {0, 1, 2, 3, 4};
  assert_ring_buffer_is(ringBuffer, state2, k_ringSize - 1);

  quiz_assert(ringBuffer.queuePop() == 0);
  int state3[k_ringSize - 2] = {1, 2, 3, 4};
  assert_ring_buffer_is(ringBuffer, state3, k_ringSize - 2);

  ringBuffer.push(6);
  int state4[k_ringSize - 1] = {1, 2, 3, 4, 6};
  assert_ring_buffer_is(ringBuffer, state4, k_ringSize - 1);

  quiz_assert(ringBuffer.length() == 5);
  ringBuffer.reset();
  quiz_assert(ringBuffer.length() == 0);
}

