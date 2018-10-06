#ifndef KANDINSKY_PALETTE_H
#define KANDINSKY_PALETTE_H

#include <kandinsky/color.h>

template <int S>
class KDPalette {
public:
  static KDPalette Gradient(KDColor fromColor, KDColor toColor) {
    KDPalette p;
    for (int i=0; i<S; i++) {
      p.m_colors[i] = KDColor::blend(fromColor, toColor, (0xFF*i)/S);
    }
    return p;
  }
  KDColor colorAtIndex(int i) const {
    return m_colors[i];
  }
private:
  KDPalette() {}
  KDColor m_colors[S];
};

#endif
