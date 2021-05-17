#include "life.h"
#include "svc.h"

void Life::setIndex(int index) {
  m_x = Display::Width - (index + 1) * Display::CommonHorizontalMargin;
}

constexpr Color k_heart[Life::k_height*Life::k_width] = {
 Color::RGB24(0x000000), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0x000000), Color::RGB24(0x000000), Color::RGB24(0x000000), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0x000000),
 Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0x000000), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81),
 Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81),
 Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81),
 Color::RGB24(0x000000), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0x000000),
 Color::RGB24(0x000000), Color::RGB24(0x000000), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0x000000), Color::RGB24(0x000000),
 Color::RGB24(0x000000), Color::RGB24(0x000000), Color::RGB24(0x000000), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0x000000), Color::RGB24(0x000000), Color::RGB24(0x000000),
 Color::RGB24(0x000000), Color::RGB24(0x000000), Color::RGB24(0x000000), Color::RGB24(0x000000), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0xFC7F81), Color::RGB24(0x000000), Color::RGB24(0x000000), Color::RGB24(0x000000), Color::RGB24(0x000000),
 Color::RGB24(0x000000), Color::RGB24(0x000000), Color::RGB24(0x000000), Color::RGB24(0x000000), Color::RGB24(0x000000), Color::RGB24(0xFC7F81), Color::RGB24(0x000000), Color::RGB24(0x000000), Color::RGB24(0x000000), Color::RGB24(0x000000), Color::RGB24(0x000000)
};

void Life::draw() const {
  if (m_broken) {
    Ion::Display::pushRectUniform(Rect(m_x - k_width/2, m_y - k_height/2, k_width, k_height), Black);
  } else {
    Ion::Display::pushRect(Rect(m_x - k_width/2, m_y - k_height/2, k_width, k_height), k_heart);
  }
}

void Life::breaks() {
  m_broken = true;
  draw();
}
