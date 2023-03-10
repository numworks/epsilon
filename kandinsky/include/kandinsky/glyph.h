#ifndef KANDINSKY_GLYPHS_H
#define KANDINSKY_GLYPHS_H

#include <kandinsky/color.h>
#include <kandinsky/font.h>

class KDGlyph {
 public:
  constexpr static float k_alignLeft = 0.;
  constexpr static float k_alignRight = 1.;
  constexpr static float k_alignCenter = 0.5;
  constexpr static float k_alignTop = 0.;
  constexpr static float k_alignBottom = 1.;

  struct Style {
    KDColor glyphColor = KDColorBlack;
    KDColor backgroundColor = KDColorWhite;
    KDFont::Size font = KDFont::Size::Large;
  };

  struct Format {
    Style style;
    float horizontalAlignment = k_alignLeft;
    float verticalAlignment = k_alignCenter;
  };

};  // namespace KDGlyph

#endif
