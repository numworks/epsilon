#ifndef PERIODIC_DISPLAY_TYPE_H
#define PERIODIC_DISPLAY_TYPE_H

#include "elements_data_base.h"
#include "palette.h"

namespace Periodic {

class DisplayType {
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
  void setLegendForElement(AtomicNumber z, char * buffer, size_t bufferSize) const;

protected:
  virtual I18n::Message titleForElement(AtomicNumber z) const = 0;
  virtual size_t legendContentForElement(AtomicNumber z, char * buffer, size_t bufferSize) const { return buffer[0] = 0; }
};

class ContinuousDisplayType : public DisplayType {
public:
  ColorPair colorPairForElement(AtomicNumber z) const override;

protected:
  size_t legendContentForElement(AtomicNumber z, char * buffer, size_t bufferSize) const override;
  virtual double parameter(AtomicNumber z) const = 0;
  virtual ColorPair minimalColors() const = 0;
  virtual ColorPair maximalColors() const = 0;
  virtual const char * unit() const = 0;

private:
  uint8_t blendAlphaForContinuousParameter(AtomicNumber z) const;
};

class GroupsDisplayType : public DisplayType {
  ColorPair colorPairForElement(AtomicNumber z) const override;
  I18n::Message titleForElement(AtomicNumber z) const override;
};

class BlocksDisplayType : public DisplayType {
  ColorPair colorPairForElement(AtomicNumber z) const override { return ColorPair(); }
  I18n::Message titleForElement(AtomicNumber z) const override { return I18n::Message::Default; }
};

class MetalsDisplayType : public DisplayType {
  ColorPair colorPairForElement(AtomicNumber z) const override { return ColorPair(); }
  I18n::Message titleForElement(AtomicNumber z) const override { return I18n::Message::Default; }
};

class MassDisplayType : public ContinuousDisplayType {
  I18n::Message titleForElement(AtomicNumber z) const override { return I18n::Message::Default; }
  double parameter(AtomicNumber z) const override { return ElementsDataBase::MolarMass(z); }
  ColorPair minimalColors() const override { return ColorPair(); }
  ColorPair maximalColors() const override { return ColorPair(); }
  const char * unit() const override { return ElementsDataBase::MolarMassUnit(); };
};

class ElectronegativityDisplayType : public ContinuousDisplayType {
  I18n::Message titleForElement(AtomicNumber z) const override { return I18n::Message::Default; }
  double parameter(AtomicNumber z) const override { return ElementsDataBase::Electronegativity(z); }
  ColorPair minimalColors() const override { return ColorPair(); }
  ColorPair maximalColors() const override { return ColorPair(); }
  const char * unit() const override { return ElementsDataBase::ElectronegativityUnit(); };
};

class MeltingPointDisplayType : public ContinuousDisplayType {
  I18n::Message titleForElement(AtomicNumber z) const override { return I18n::Message::Default; }
  double parameter(AtomicNumber z) const override { return ElementsDataBase::MeltingPoint(z); }
  ColorPair minimalColors() const override { return ColorPair(); }
  ColorPair maximalColors() const override { return ColorPair(); }
  const char * unit() const override { return ElementsDataBase::TemperatureUnit(); };
};

class BoilingPointDisplayType : public ContinuousDisplayType {
  I18n::Message titleForElement(AtomicNumber z) const override { return I18n::Message::Default; }
  double parameter(AtomicNumber z) const override { return ElementsDataBase::BoilingPoint(z); }
  ColorPair minimalColors() const override { return ColorPair(); }
  ColorPair maximalColors() const override { return ColorPair(); }
  const char * unit() const override { return ElementsDataBase::TemperatureUnit(); };
};

class RadiusDisplayType : public ContinuousDisplayType {
  I18n::Message titleForElement(AtomicNumber z) const override { return I18n::Message::Default; }
  double parameter(AtomicNumber z) const override { return ElementsDataBase::Radius(z); }
  ColorPair minimalColors() const override { return ColorPair(); }
  ColorPair maximalColors() const override { return ColorPair(); }
  const char * unit() const override { return ElementsDataBase::RadiusUnit(); };
};

}

#endif
