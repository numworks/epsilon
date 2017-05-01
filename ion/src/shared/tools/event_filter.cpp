#include <iostream>
#include <ion/events.h>

int main(int argc, char * argv[]) {
  char c;
  while (std::cin.get(c)) {
    Ion::Events::Event e(c);
    if (e.isValid()) {
      std::cout.put(c);
    }
  }
}
