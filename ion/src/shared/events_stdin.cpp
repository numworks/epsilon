#include <ion/events.h>
#include <stdio.h>
#include <stdlib.h>

/* Use this implementation of Ion::Events::getEvent to simulate pre-recorded key
 * presses. It streams event recorded to a file and played back from standard
 * input. */

Ion::Events::Event Ion::Events::getEvent(int * timeout) {
  int c = getchar();
  if (c == EOF) {
    exit(0);
  } else {
    return Ion::Events::Event(c);
  }
}
