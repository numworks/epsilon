#pragma once

#include <kandinsky/font.h>

#include "basic_color.h"

class KDGlyph {
 public:
  constexpr static float k_alignLeft = 0.;
  constexpr static float k_alignRight = 1.;
  constexpr static float k_alignCenter = 0.5;
  constexpr static float k_alignTop = 0.;
  constexpr static float k_alignBottom = 1.;

  struct Style {
    KDColor glyphColor = BasicColor::Black;
    KDColor backgroundColor = BasicColor::White;
    KDFont::Size font = KDFont::Size::Large;
  };

  struct Format {
    Style style;
    float horizontalAlignment = k_alignLeft;
    float verticalAlignment = k_alignCenter;
  };

};  // namespace KDGlyph
