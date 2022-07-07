#include "coloring.h"
#include "assert.h"

namespace Periodic {

// ContinuousColoring

Coloring::ColorPair ContinuousColoring::colorPairForElement(AtomicNumber z) const {
  ColorPair min = minimalColors();
  ColorPair max = maximalColors();
  return ColorPair(blendAlphaForContinuousParameter(z), min.fg(), max.fg(), min.bg(), max.bg());
}

uint8_t ContinuousColoring::blendAlphaForContinuousParameter(AtomicNumber z) const {
  /* FIXME We will recompute alpha for each element when redrawing the whole
   * table. We could memoize the sorted indexe to speed up the process. */
  float numberOfLowerValues = 0;
  double v = parameter(z);
  for (AtomicNumber z2 = 1; z2 <= ElementsDataBase::k_numberOfElements; z2++) {
    if (parameter(z2) < v) {
      numberOfLowerValues++;
    }
  }
  return static_cast<uint8_t>(numberOfLowerValues / (ElementsDataBase::k_numberOfElements - 1.f) * ((1 << 8) - 1.f));
}

// GroupsColoring

Coloring::ColorPair GroupsColoring::colorPairForElement(AtomicNumber z) const {
  constexpr ColorPair k_colors[] = {
    ColorPair(Palette::ElementOrangeDark, Palette::ElementOrangeLight),
    ColorPair(Palette::ElementCeladonDark, Palette::ElementCeladonLight),
    ColorPair(Palette::ElementBlueDark, Palette::ElementBlueLight),
    ColorPair(Palette::ElementYellowDark, Palette::ElementYellowLight),
    ColorPair(Palette::ElementPinkDark, Palette::ElementPinkLight),
    ColorPair(Palette::ElementGreenDark, Palette::ElementGreenLight),
    ColorPair(Palette::ElementTurquoiseDark, Palette::ElementTurquoiseLight),
    ColorPair(Palette::ElementRedDark, Palette::ElementRedLight),
    ColorPair(Palette::ElementGrassDark, Palette::ElementGrassLight),
    ColorPair(Palette::ElementPurpleDark, Palette::ElementPurpleLight),
    ColorPair(Palette::ElementGreyDark, Palette::ElementGreyLight),
  };
  ElementData::Group group = ElementsDataBase::Group(z);
  assert(static_cast<uint8_t>(group) < sizeof(k_colors) / sizeof(k_colors[0]));
  return k_colors[static_cast<uint8_t>(group)];
}

}
