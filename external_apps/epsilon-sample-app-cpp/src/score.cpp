#include "score.h"
#include "life.h"
#include "palette.h"

void Score::draw() const {
  char buffer[k_maximalScoreNumberOfDigits + 1];
  char * c = buffer + k_maximalScoreNumberOfDigits;
  *c = 0;
  int score = m_value;
  while (c > buffer) {
    c--;
    *c = '0' + score % 10;
    score /= 10;
  }
  EADK::Display::drawString(buffer, EADK::Point(EADK::Screen::Width - 10 * k_maximalScoreNumberOfDigits - Display::CommonHorizontalMargin, 2 * Display::CommonVerticalMargin + Life::k_height), true, White, Black);
}

void Score::increment() {
  m_value++;
  if (m_value == k_maximalScore) {
    EADK::Display::pushRectUniform(EADK::Rect(0, 0, EADK::Screen::Width, EADK::Screen::Height), Yellow);
    EADK::Display::drawString("Well done", EADK::Point((EADK::Screen::Width - 9 * 10) / 2, (EADK::Screen::Height - 18) / 2), true, Black, Yellow);
    while(1) {}
  }
}
