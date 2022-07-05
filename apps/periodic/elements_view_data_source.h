#ifndef PERIODIC_ELEMENTS_VIEW_DATA_SOURCE_H
#define PERIODIC_ELEMENTS_VIEW_DATA_SOURCE_H

#include "elements_data_base.h"
#include "palette.h"

namespace Periodic {

class ElementsViewDataSource {
public:
  enum class Coloring : uint8_t {
    Groups,
    Blocks,
    Metals,
    Mass,
    Electronegativity,
    MeltingPoint,
    BoilingPoint,
    Radius,
  };

  ElementsViewDataSource() : m_selectedElement(1), m_coloring(Coloring::Groups), m_textFilter(nullptr) {}

  AtomicNumber selectedElement() const { return m_selectedElement; }
  void setSelectedElement(AtomicNumber z) { m_selectedElement = z; }
  void setColoring(Coloring coloring) { m_coloring = coloring; }
  void setTextFilter(const char * filter) { m_textFilter = filter; }

  class ColorPair {
  public:
    constexpr ColorPair(KDColor fg = KDColorBlack, KDColor bg = KDColorWhite) : m_fg(fg), m_bg(bg) {}
    ColorPair(uint8_t alpha, KDColor fgMin, KDColor fgMax, KDColor bgMin, KDColor bgMax) :
      m_fg(KDColor::blend(fgMax, fgMin, alpha)),
      m_bg(KDColor::blend(bgMax, bgMin, alpha))
    {}
    KDColor fg() const { return m_fg; }
    KDColor bg() const { return m_bg; }
  private:
    KDColor m_fg;
    KDColor m_bg;
  };
  ColorPair colorPairForElement(AtomicNumber z) const;

private:
  /* FIXME Colors have not been determined for this feature. */
  constexpr static KDColor k_massForegroundMin = KDColorBlack;
  constexpr static KDColor k_massForegroundMax = KDColorBlack;
  constexpr static KDColor k_massBackgroundMin = KDColorBlack;
  constexpr static KDColor k_massBackgroundMax = KDColorBlack;
  constexpr static KDColor k_electronegativityForegroundMin = KDColorBlack;
  constexpr static KDColor k_electronegativityForegroundMax = KDColorBlack;
  constexpr static KDColor k_electronegativityBackgroundMin = KDColorBlack;
  constexpr static KDColor k_electronegativityBackgroundMax = KDColorBlack;
  constexpr static KDColor k_meltingPointForegroundMin = KDColorBlack;
  constexpr static KDColor k_meltingPointForegroundMax = KDColorBlack;
  constexpr static KDColor k_meltingPointBackgroundMin = KDColorBlack;
  constexpr static KDColor k_meltingPointBackgroundMax = KDColorBlack;
  constexpr static KDColor k_boilingPointForegroundMin = KDColorBlack;
  constexpr static KDColor k_boilingPointForegroundMax = KDColorBlack;
  constexpr static KDColor k_boilingPointBackgroundMin = KDColorBlack;
  constexpr static KDColor k_boilingPointBackgroundMax = KDColorBlack;
  constexpr static KDColor k_radiusForegroundMin = KDColorBlack;
  constexpr static KDColor k_radiusForegroundMax = KDColorBlack;
  constexpr static KDColor k_radiusBackgroundMin = KDColorBlack;
  constexpr static KDColor k_radiusBackgroundMax = KDColorBlack;

  static ColorPair colorPairForGroup(ElementData::Group group);
  static ColorPair colorPairForBlock(ElementData::Block block);
  static ColorPair colorPairForMetal(ElementData::Group group);

  typedef double (*DoubleParameterGetter)(AtomicNumber z);
  static uint8_t blendAlphaForContinuousParameter(AtomicNumber z, DoubleParameterGetter get);

  AtomicNumber m_selectedElement;
  Coloring m_coloring;
  const char * m_textFilter;
};

}

#endif
