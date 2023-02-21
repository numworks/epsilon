#include <ion.h>

/* Use this implementation of Ion::Events::getEvent to simulate pre-recorded key
 * presses. This implementation doesn't wait at all, which can be useful when
 * profiling code execution. */

const int loopLength = 12;

using namespace Ion::Events;

const Event sequence[] = {Right, Right, Right, OK, Down, Down, Down, Down,
                          Down,  Down,  Up,    Up, Up,   Up,   Up,   Up};

Event Ion::Events::getEvent(int* timeout) {
  static int i = 0;
  int sequenceLength = sizeof(sequence) / sizeof(sequence[0]);
  if (i == sequenceLength) {
    i = sequenceLength - loopLength;
  }
  if (i > sequenceLength) {
    while (1) {
    }
  }
  return sequence[i++];
}
