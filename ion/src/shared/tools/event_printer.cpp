#include <ion/events.h>

#include <cassert>
#include <iostream>

// TODO : Update this script to handle state file headers

int main(int argc, char* argv[]) {
  unsigned char c = 0;
  while (std::cin >> c) {
    if (Ion::Events::isDefined(c)) {
      Ion::Events::Event event(c);
      if (event.name() == nullptr) {
        std::cerr << "*** UNNAMED(" << (int)c << ") ***" << std::endl;
      } else {
        std::cout << event.name() << std::endl;
      }
    } else {
      std::cerr << "*** INVALID(" << (int)c << ") ***" << std::endl;
    }
  }
}
