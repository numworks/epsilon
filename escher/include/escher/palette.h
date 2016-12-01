#ifndef ESCHER_PALETTE_H
#define ESCHER_PALETTE_H

#include <kandinsky/color.h>

class Palette {
public:
  constexpr static KDColor LineColor = KDColor::RGB24(0xB4B7B9);
  constexpr static KDColor BackgroundColor = KDColor::RGB24(0xF0F3F5);
  constexpr static KDColor CellBackgroundColor = KDColor::RGB24(0xFCFCFC);
  constexpr static KDColor FocusCellBackgroundColor = KDColor::RGB24(0xDDDFE6);
  constexpr static KDColor DesactiveTextColor = KDColor::RGB24(0x646464);
  constexpr static KDColor BoxTitleBackgroundColor = KDColor::RGB24(0x656976);
};

#endif
