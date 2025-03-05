#include "life.h"
#include "palette.h"

void Life::setIndex(int index) {
  m_x = EADK::Screen::Width - (index + 1) * Display::CommonHorizontalMargin;
}

constexpr EADK::Color k_heart[Life::k_height*Life::k_width] = {
 EADK::Color(0x000000), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0x000000), EADK::Color(0x000000), EADK::Color(0x000000), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0x000000),
 EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0x000000), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81),
 EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81),
 EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81),
 EADK::Color(0x000000), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0x000000),
 EADK::Color(0x000000), EADK::Color(0x000000), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0x000000), EADK::Color(0x000000),
 EADK::Color(0x000000), EADK::Color(0x000000), EADK::Color(0x000000), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0x000000), EADK::Color(0x000000), EADK::Color(0x000000),
 EADK::Color(0x000000), EADK::Color(0x000000), EADK::Color(0x000000), EADK::Color(0x000000), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0xFC7F81), EADK::Color(0x000000), EADK::Color(0x000000), EADK::Color(0x000000), EADK::Color(0x000000),
 EADK::Color(0x000000), EADK::Color(0x000000), EADK::Color(0x000000), EADK::Color(0x000000), EADK::Color(0x000000), EADK::Color(0xFC7F81), EADK::Color(0x000000), EADK::Color(0x000000), EADK::Color(0x000000), EADK::Color(0x000000), EADK::Color(0x000000)
};

void Life::draw() const {
  if (m_broken) {
    EADK::Display::pushRectUniform(EADK::Rect(m_x - k_width/2, m_y - k_height/2, k_width, k_height), Black);
  } else {
    EADK::Display::pushRect(EADK::Rect(m_x - k_width/2, m_y - k_height/2, k_width, k_height), k_heart);
  }
}

void Life::breaks() {
  m_broken = true;
  draw();
}
