#include <iostream>
#include <ion/events.h>

int main(int argc, char * argv[]) {
  unsigned char c = 0;
  while (std::cin >> c) {
    Ion::Events::Event e(c);
    if (e.isDefined() && e != Ion::Events::Termination) {
      std::cout << c;
    }
  }
}
