#ifndef LIFE_H
#define LIFE_H

#include "display.h"

class Life {
public:
  Life() : m_x(0), m_y(Display::CommonVerticalMargin), m_broken(false) {}
  void setIndex(int index);
  void draw() const;
  void breaks();
private:
  static constexpr int k_size = 5;
  int m_x;
  int m_y;
  bool m_broken;
};

#endif
