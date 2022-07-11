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

  virtual ColorPair colorPairForElement(AtomicNumber z) const { return ColorPair(); }
  void setLegendForElement(AtomicNumber z, char * buffer, size_t bufferSize) const;

protected:
  virtual I18n::Message titleForElement(AtomicNumber z) const { return I18n::Message::Default; }
  virtual size_t legendContentForElement(AtomicNumber z, char * buffer, size_t bufferSize) const { return buffer[0] = 0; }
};

class ContinuousDisplayType : public DisplayType {
public:
  ColorPair colorPairForElement(AtomicNumber z) const override;
  size_t printValueWithUnitForElement(AtomicNumber z, char * buffer, size_t bufferSize) const;

protected:
  size_t legendContentForElement(AtomicNumber z, char * buffer, size_t bufferSize) const override;
  virtual double parameter(AtomicNumber z) const = 0;
  virtual ColorPair minimalColors() const { return ColorPair(); }
  virtual ColorPair maximalColors() const { return ColorPair(); }
  virtual const char * unit() const = 0;

private:
  uint8_t blendAlphaForContinuousParameter(AtomicNumber z) const;
};

class GroupsDisplayType : public DisplayType {
public:
  ColorPair colorPairForElement(AtomicNumber z) const override;
  I18n::Message titleForElement(AtomicNumber z) const override;
};

class BlocksDisplayType : public DisplayType {
};

class MetalsDisplayType : public DisplayType {
};

class StatesDisplayType : public DisplayType {
public:
  I18n::Message titleForElement(AtomicNumber z) const override;
};

class MassDisplayType : public ContinuousDisplayType {
  virtual I18n::Message titleForElement(AtomicNumber z) const override { return I18n::Message::MassTitle; }
  double parameter(AtomicNumber z) const override { return ElementsDataBase::MolarMass(z); }
  const char * unit() const override { return ElementsDataBase::MolarMassUnit(); };
};

class ElectronegativityDisplayType : public ContinuousDisplayType {
  virtual I18n::Message titleForElement(AtomicNumber z) const override { return I18n::Message::ElectronegativityTitle; }
  double parameter(AtomicNumber z) const override { return ElementsDataBase::Electronegativity(z); }
  const char * unit() const override { return ElementsDataBase::ElectronegativityUnit(); };
};

class MeltingPointDisplayType : public ContinuousDisplayType {
  virtual I18n::Message titleForElement(AtomicNumber z) const override { return I18n::Message::MeltingPointTitle; }
  double parameter(AtomicNumber z) const override { return ElementsDataBase::MeltingPoint(z); }
  const char * unit() const override { return ElementsDataBase::TemperatureUnit(); };
};

class BoilingPointDisplayType : public ContinuousDisplayType {
  virtual I18n::Message titleForElement(AtomicNumber z) const override { return I18n::Message::BoilingPointTitle; }
  double parameter(AtomicNumber z) const override { return ElementsDataBase::BoilingPoint(z); }
  const char * unit() const override { return ElementsDataBase::TemperatureUnit(); };
};

class RadiusDisplayType : public ContinuousDisplayType {
  virtual I18n::Message titleForElement(AtomicNumber z) const override { return I18n::Message::RadiusTitle; }
  double parameter(AtomicNumber z) const override { return ElementsDataBase::Radius(z); }
  const char * unit() const override { return ElementsDataBase::RadiusUnit(); };
};

class DensityDisplayType : public ContinuousDisplayType {
  double parameter(AtomicNumber z) const override { return ElementsDataBase::Density(z); }
  const char * unit() const override { return ElementsDataBase::DensityUnit(); };
};

class AffinityDisplayType : public ContinuousDisplayType {
  double parameter(AtomicNumber z) const override { return ElementsDataBase::Affinity(z); }
  const char * unit() const override { return ElementsDataBase::EnergyUnit(); };
};

class IonisationDisplayType : public ContinuousDisplayType {
  double parameter(AtomicNumber z) const override { return ElementsDataBase::EnergyOfIonisation(z); }
  const char * unit() const override { return ElementsDataBase::EnergyUnit(); };
};

}

#endif
