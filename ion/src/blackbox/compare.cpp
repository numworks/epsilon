/* Compare two Epsilon versions
 *
 * git checkout first_hash
 * make -j8 PLATFORM=blackbox clean libepsilon_first.dylib
 * git checkout second_hash
 * make -j8 PLATFORM=blackbox clean libepsilon_second.dylib
 * make PLATFORM=blackbox compare
 */

#undef EPSILON_LIB_PREFIX
#define EPSILON_LIB_PREFIX first
#include "library.h"
#undef EPSILON_LIB_PREFIX
#define EPSILON_LIB_PREFIX second
#include "library.h"

#include <ion.h>
#include <thread>

int main(int argc, char * argv[]) {
  std::thread first(first_epsilon_main);
  std::thread second(second_epsilon_main);

  const KDColor * firstFrameBuffer = first_epsilon_frame_buffer();
  const KDColor * secondFrameBuffer = second_epsilon_frame_buffer();

  int index = 0;
  while (true) {
    int e = getchar();
    index++;

    first_epsilon_send_event(e);
    second_epsilon_send_event(e);

    if (e == EOF) {
      break;
    }

    first_epsilon_wait_event_processed();
    second_epsilon_wait_event_processed();

    if (memcmp(firstFrameBuffer, secondFrameBuffer, sizeof(KDColor) * Ion::Display::Width * Ion::Display::Height) != 0) {
      printf("Framebuffer mismatch at index %d\n", index);
      first_epsilon_write_frame_buffer_to_file("epsilon_first.png");
      second_epsilon_write_frame_buffer_to_file("epsilon_second.png");
      exit(-1);
    }
  }

  first.join();
  second.join();
}
