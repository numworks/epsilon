#include <ion.h>

const Ion::Events::Event kEventForKeyDown[] = {
  Ion::Events::Event::F1, Ion::Events::Event::F2, Ion::Events::Event::F3, Ion::Events::Event::F4, Ion::Events::Event::F5, Ion::Events::Event::SECOND, Ion::Events::Event::SHIFT, Ion::Events::Event::ESC, Ion::Events::Event::LEFT_ARROW, Ion::Events::Event::UP_ARROW, Ion::Events::Event::DIAMOND, Ion::Events::Event::ALPHA, Ion::Events::Event::APPS, Ion::Events::Event::DOWN_ARROW, Ion::Events::Event::RIGHT_ARROW, Ion::Events::Event::HOME, Ion::Events::Event::MODE, Ion::Events::Event::CATALOG, Ion::Events::Event::DELETE, Ion::Events::Event::CLEAR, Ion::Events::Event::LOWER_CASE_X, Ion::Events::Event::LOWER_CASE_Y, Ion::Events::Event::LOWER_CASE_Z, Ion::Events::Event::LOWER_CASE_T, Ion::Events::Event::POWER, Ion::Events::Event::EQUAL, Ion::Events::Event::LEFT_PARENTHESIS, Ion::Events::Event::RIGHT_PARENTHESIS, Ion::Events::Event::COMMA, Ion::Events::Event::DIVISION, Ion::Events::Event::DOT, Ion::Events::Event::SEVEN, Ion::Events::Event::EIGHT, Ion::Events::Event::NINE, Ion::Events::Event::PRODUCT, Ion::Events::Event::UPPER_CASE_E, Ion::Events::Event::FOUR, Ion::Events::Event::FIVE, Ion::Events::Event::SIX, Ion::Events::Event::MINUS, Ion::Events::Event::DOT, Ion::Events::Event::ONE, Ion::Events::Event::TWO, Ion::Events::Event::THREE, Ion::Events::Event::PLUS, Ion::Events::Event::DOT, Ion::Events::Event::ZERO, Ion::Events::Event::DOT, Ion::Events::Event::MINUS, Ion::Events::Event::ENTER
};

// Debouncing, qnd change to get_key event.
Ion::Events::Event Ion::Events::getEvent() {
  // Let's start by saving which keys we've seen up
  bool key_seen_up[Ion::Keyboard::NumberOfKeys];
  for (int k=0; k<Ion::Keyboard::NumberOfKeys; k++) {
    key_seen_up[k] = !Ion::Keyboard::keyDown((Ion::Keyboard::Key)k);
  }

  // Wait a little to debounce the button.
  msleep(10);

  /* Let's discard the keys we previously saw up but which aren't anymore: those
   * were probably bouncing! */
  for (int k=0; k<Ion::Keyboard::NumberOfKeys; k++) {
    key_seen_up[k] &= !Ion::Keyboard::keyDown((Ion::Keyboard::Key)k);
  }

  while (1) {
    for (int k=0; k<Ion::Keyboard::NumberOfKeys; k++) {
      if (Ion::Keyboard::keyDown((Ion::Keyboard::Key)k)) {
        if (key_seen_up[k]) {
          return kEventForKeyDown[k];
        }
      } else {
        key_seen_up[k] = true;
      }
    }
    msleep(10);
  }
}
