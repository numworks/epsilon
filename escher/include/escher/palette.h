#ifndef ESCHER_PALETTE_H
#define ESCHER_PALETTE_H

#include <kandinsky/color.h>

class Palette {
public:
  constexpr static KDColor LineColor = KDColor(0xB4B7B9);
  constexpr static KDColor BackgroundColor = KDColor(0xF0F3F5);
  constexpr static KDColor CellBackgroundColor = KDColor(0xFCFCFC);
  constexpr static KDColor FocusCellBackgroundColor = KDColor(0xBFD3EB);
  constexpr static KDColor DesactiveTextColor = KDColor(0x646464);
  constexpr static KDColor BoxTitleBackgroundColor = KDColor(0x656976);
};

#endif