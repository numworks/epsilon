#include "view.h"
#include "converter.h"
#include <algorithm>
#include <string.h>

constexpr EADK::Color View::k_textColor;
constexpr EADK::Color View::k_backgroundColor;

void View::drawLayout() {
  EADK::Display::pushRectUniform(EADK::Screen::Rect, k_backgroundColor);
}

void View::drawStore(Store* store) {
  for (int i = 0; i < Store::k_maxNumberOfStoredValues; i++) {
    // Erase previous value
    EADK::Rect storeRect(k_horizontalMargin, i * k_verticalMargin, std::min(k_maxNumberOfCharacters * k_characterWidth, (int)EADK::Screen::Width), k_characterHeight);
    EADK::Display::pushRectUniform(storeRect, k_backgroundColor);
    // Draw new value
    char text[k_maxNumberOfCharacters];
    Converter::Serialize(store->value(i), text, k_maxNumberOfCharacters);
    EADK::Display::drawString(text,
                              EADK::Point(k_horizontalMargin, i * k_verticalMargin),
                              true, k_textColor, k_backgroundColor);
  }
}

void View::drawInputField(const char* input) {
  // Erase previous field
  EADK::Rect fieldRect(k_horizontalMargin, Store::k_maxNumberOfStoredValues * k_verticalMargin, std::min((int)EADK::Screen::Width, k_maxNumberOfCharacters * k_characterWidth), k_characterHeight);
  EADK::Display::pushRectUniform(fieldRect, k_backgroundColor);
  // Draw new field
  EADK::Display::drawString(
      input,
      EADK::Point(fieldRect.x(), fieldRect.y()),
      true, k_textColor, k_backgroundColor);
  // Draw the cursor
  constexpr int k_textWidth = 10;
  constexpr int k_textHeight = 18;
  EADK::Rect cursorRect(fieldRect.x() + strlen(input) * k_textWidth + 1, fieldRect.y(), 1, k_textHeight);
  EADK::Display::pushRectUniform(cursorRect, k_textColor);
}
