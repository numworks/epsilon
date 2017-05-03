#include <iostream>
#include <cassert>
#include <ion/events.h>

int main(int argc, char * argv[]) {
  unsigned char c = 0;
  while (std::cin >> c) {
    Ion::Events::Event event(c);
    if (event.isDefined()) {
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
