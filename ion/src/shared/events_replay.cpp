#include <ion.h>

/* Use this implementation of Ion::Events::getEvent to simulate pre-recorded key
 * presses. This implementation doesn't wait at all, which can be useful when
 * profiling code execution. */

const int loopLength = 2;

const Ion::Events::Event sequence[] = {
  Ion::Events::Event::F1,
  Ion::Events::Event::DOWN_ARROW,
  Ion::Events::Event::DOWN_ARROW,
  Ion::Events::Event::UP_ARROW,
};

Ion::Events::Event Ion::Events::getEvent() {
  static int i = 0;
  int sequenceLength = sizeof(sequence)/sizeof(sequence[0]);
  if (i == sequenceLength) {
    i = sequenceLength - loopLength;
  }
  if (i > sequenceLength) {
    while (1) {
    }
  }
  return sequence[i++];
}
