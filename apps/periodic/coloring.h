#ifndef PERIODIC_COLORING_H
#define PERIODIC_COLORING_H

#include "elements_data_base.h"
#include "palette.h"

namespace Periodic {

class Coloring {
public:
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

  virtual ColorPair colorPairForElement(AtomicNumber z) const = 0;
};

class ContinuousColoring : public Coloring {
public:
  ColorPair colorPairForElement(AtomicNumber z) const override;

protected:
  virtual double parameter(AtomicNumber z) const = 0;
  virtual ColorPair minimalColors() const = 0;
  virtual ColorPair maximalColors() const = 0;

private:
  uint8_t blendAlphaForContinuousParameter(AtomicNumber z) const;
};

class GroupsColoring : public Coloring {
  ColorPair colorPairForElement(AtomicNumber z) const override;
};

class BlocksColoring : public Coloring {
  ColorPair colorPairForElement(AtomicNumber z) const override { return ColorPair(); }
};

class MetalsColoring : public Coloring {
  ColorPair colorPairForElement(AtomicNumber z) const override { return ColorPair(); }
};

class MassColoring : public ContinuousColoring {
  double parameter(AtomicNumber z) const { return ElementsDataBase::MolarMass(z); }
  ColorPair minimalColors() const { return ColorPair(); }
  ColorPair maximalColors() const { return ColorPair(); }
};

class ElectronegativityColoring : public ContinuousColoring {
  double parameter(AtomicNumber z) const { return ElementsDataBase::Electronegativity(z); }
  ColorPair minimalColors() const { return ColorPair(); }
  ColorPair maximalColors() const { return ColorPair(); }
};

class MeltingPointColoring : public ContinuousColoring {
  double parameter(AtomicNumber z) const { return ElementsDataBase::MeltingPoint(z); }
  ColorPair minimalColors() const { return ColorPair(); }
  ColorPair maximalColors() const { return ColorPair(); }
};

class BoilingPointColoring : public ContinuousColoring {
  double parameter(AtomicNumber z) const { return ElementsDataBase::BoilingPoint(z); }
  ColorPair minimalColors() const { return ColorPair(); }
  ColorPair maximalColors() const { return ColorPair(); }
};

class RadiusColoring : public ContinuousColoring {
  double parameter(AtomicNumber z) const { return ElementsDataBase::Radius(z); }
  ColorPair minimalColors() const { return ColorPair(); }
  ColorPair maximalColors() const { return ColorPair(); }
};

}

#endif
