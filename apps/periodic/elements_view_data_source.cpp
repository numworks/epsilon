#include "elements_view_data_source.h"
#include <assert.h>

namespace Periodic {

ElementsViewDataSource::ColorPair ElementsViewDataSource::colorPairForElement(AtomicNumber z) const {
  /* TODO Some elements may appear grey depending on m_textFilter. */
  switch (m_coloring) {
  case Coloring::Groups:
    return colorPairForGroup(ElementsDataBase::Group(z));
  case Coloring::Blocks:
    return colorPairForBlock(ElementsDataBase::Block(z));
  case Coloring::Metals:
    return colorPairForMetal(ElementsDataBase::Group(z));
  case Coloring::Mass:
    return ColorPair(blendAlphaForContinuousParameter(z, &ElementsDataBase::MolarMass), k_massForegroundMin, k_massForegroundMax, k_massBackgroundMin, k_massBackgroundMax);
  case Coloring::Electronegativity:
    return ColorPair(blendAlphaForContinuousParameter(z, &ElementsDataBase::Electronegativity), k_electronegativityForegroundMin, k_electronegativityForegroundMax, k_electronegativityBackgroundMin, k_electronegativityBackgroundMax);
  case Coloring::MeltingPoint:
    return ColorPair(blendAlphaForContinuousParameter(z, &ElementsDataBase::MeltingPoint), k_meltingPointForegroundMin, k_meltingPointForegroundMax, k_meltingPointBackgroundMin, k_meltingPointBackgroundMax);
  case Coloring::BoilingPoint:
    return ColorPair(blendAlphaForContinuousParameter(z, &ElementsDataBase::BoilingPoint), k_boilingPointForegroundMin, k_boilingPointForegroundMax, k_boilingPointBackgroundMin, k_boilingPointBackgroundMax);
  default:
    assert(m_coloring == Coloring::Radius);
    return ColorPair(blendAlphaForContinuousParameter(z, &ElementsDataBase::Radius), k_radiusForegroundMin, k_radiusForegroundMax, k_radiusBackgroundMin, k_radiusBackgroundMax);
  }
}

ElementsViewDataSource::ColorPair ElementsViewDataSource::colorPairForGroup(ElementData::Group group) {
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
  assert(static_cast<uint8_t>(group) < sizeof(k_colors) / sizeof(k_colors[0]));
  return k_colors[static_cast<uint8_t>(group)];
}

ElementsViewDataSource::ColorPair ElementsViewDataSource::colorPairForBlock(ElementData::Block block) {
  /* FIXME Color palette is undefined for this feature. */
  return ColorPair();
}

ElementsViewDataSource::ColorPair ElementsViewDataSource::colorPairForMetal(ElementData::Group group) {
  /* FIXME Color palette is undefined for this feature. */
  return ColorPair();
}

uint8_t ElementsViewDataSource::blendAlphaForContinuousParameter(AtomicNumber z, DoubleParameterGetter get) {
  /* FIXME We will recompute alpha for each element when redrawing the whole
   * table. We could memoize the sorted indexe to speed up the process. */
  float numberOfLowerValues = 0;
  double v = get(z);
  for (AtomicNumber z2 = 1; z2 <= ElementsDataBase::k_numberOfElements; z2++) {
    if (get(z2) < v) {
      numberOfLowerValues++;
    }
  }
  return static_cast<uint8_t>(numberOfLowerValues / (ElementsDataBase::k_numberOfElements - 1.f) * ((1 << 8) - 1.f));

}

}
