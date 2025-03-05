#ifndef LIFE_H
#define LIFE_H

#include "display.h"
#include "eadkpp.h"

class Life {
public:
  Life() : m_x(0), m_y(Display::CommonVerticalMargin), m_broken(false) {}
  void setIndex(int index);
  void draw() const;
  void breaks();
  static constexpr int k_width = 11;
  static constexpr int k_height = 9;
private:
  int m_x;
  int m_y;
  bool m_broken;
};

#endif
