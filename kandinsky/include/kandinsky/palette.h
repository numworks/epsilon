#ifndef KANDINSKY_PALETTE_H
#define KANDINSKY_PALETTE_H

#include <kandinsky/color.h>
#include <assert.h>

template <int S>
class KDPalette {
public:
  static KDPalette Gradient(KDColor fromColor, KDColor toColor) {
    KDPalette p;
    for (int i=0; i<S; i++) {
      p.m_colors[i] = KDColor::Blend(fromColor, toColor, (0xFF*i)/(S-1));
    }
    return p;
  }
  KDColor colorAtIndex(int i) const {
    assert(i>=0 && i<S);
    return m_colors[i];
  }
private:
  KDPalette() {}
  KDColor m_colors[S];
};

#endif
