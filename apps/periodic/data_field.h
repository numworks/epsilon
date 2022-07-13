#ifndef PERIODIC_DATA_FIELD
#define PERIODIC_DATA_FIELD

#include <apps/i18n.h>
#include <poincare/integer.h>
#include <poincare/layout_helper.h>
#include <stddef.h>

/* Methods taking an AtomicNumber expect a number from 1 to 118 (i.e. 1-based
 * numbering). */
typedef uint8_t AtomicNumber;

namespace Periodic {

// Abstract data fields

class DataField {
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

  virtual I18n::Message fieldLegend() const { return I18n::Message::Default; }
  virtual I18n::Message fieldSymbol() const { return I18n::Message::Default; }
  virtual Poincare::Layout fieldSymbolLayout() const { return Poincare::LayoutHelper::String(I18n::translate(fieldSymbol())); }

  virtual bool hasDouble(AtomicNumber z) const { return false; }
  virtual double getDouble(AtomicNumber z) const { return NAN; }
  virtual Poincare::Layout getLayout(AtomicNumber z, int significantDigits = k_defaultSignificantDigits) const = 0;
  I18n::Message getMessage(AtomicNumber z) const;
  virtual ColorPair getColors(AtomicNumber z) const { return ColorPair(); }

protected:
  constexpr static int k_defaultSignificantDigits = 4;

  virtual I18n::Message protectedGetMessage(AtomicNumber z) const { return I18n::Message::Default; }
};

class EnumDataField : public DataField {
public:
  Poincare::Layout getLayout(AtomicNumber z, int significantDigits) const { return Poincare::LayoutHelper::String(I18n::translate(getMessage(z))); }
};

class DoubleDataField : public DataField {
public:
  bool hasDouble(AtomicNumber z) const override;
  Poincare::Layout getLayout(AtomicNumber z, int significantDigits = k_defaultSignificantDigits) const override;
  ColorPair getColors(AtomicNumber z) const override;

private:
  virtual const char * fieldUnit() const = 0;
  virtual ColorPair minColors() const { return ColorPair(); }
  virtual ColorPair maxColors() const { return ColorPair(); }
  uint8_t blendAlphaForContinuousParameter(AtomicNumber z) const;
};

class DoubleDataFieldWithSubscriptSymbol : public DoubleDataField {
public:
  Poincare::Layout fieldSymbolLayout() const override;

private:
  virtual I18n::Message fieldSubscript() const = 0;
};

// Concrete data fields

class ZDataField : public DataField {
public:
  I18n::Message fieldLegend() const override { return I18n::Message::PeriodicZLegend; }
  I18n::Message fieldSymbol() const override { return I18n::Message::PeriodicZSymbol; }
  Poincare::Layout getLayout(AtomicNumber z, int significantDigits) const override { return Poincare::Integer(z).createLayout(); }
};

class ADataField : public DataField {
public:
  I18n::Message fieldLegend() const override { return I18n::Message::PeriodicALegend; }
  I18n::Message fieldSymbol() const override { return I18n::Message::PeriodicASymbol; }
  Poincare::Layout getLayout(AtomicNumber z, int significantDigits) const override;
};

class ConfigurationDataField : public DataField {
public:
  I18n::Message fieldLegend() const override { return I18n::Message::PeriodicConfigurationLegend; }
  I18n::Message fieldSymbol() const override { return I18n::Message::PeriodicConfigurationSymbol; }
  Poincare::Layout getLayout(AtomicNumber z, int significantDigits) const override;
};

class GroupDataField : public EnumDataField {
public:
  I18n::Message fieldLegend() const override { return I18n::Message::PeriodicGroupLegend; }
  I18n::Message fieldSymbol() const override { return I18n::Message::PeriodicGroupSymbol; }
  I18n::Message protectedGetMessage(AtomicNumber z) const override;
  ColorPair getColors(AtomicNumber z) const override;
};

class BlockDataField : public EnumDataField {
public:
  I18n::Message fieldLegend() const override { return I18n::Message::PeriodicBlockLegend; }
  I18n::Message protectedGetMessage(AtomicNumber z) const override;
  // TODO ColorPair getColors(AtomicNumber z) const override;
};

class MetalDataField : public EnumDataField {
public:
  I18n::Message fieldLegend() const override { return I18n::Message::PeriodicMetalLegend; }
  I18n::Message protectedGetMessage(AtomicNumber z) const override;
  // TODO ColorPair getColors(AtomicNumber z) const override;

};

class StateDataField : public EnumDataField {
public:
  I18n::Message fieldSymbol() const override { return I18n::Message::PeriodicStateSymbol; }
  I18n::Message protectedGetMessage(AtomicNumber z) const override;
  // TODO ColorPair getColors(AtomicNumber z) const override;
};

class MassDataField : public DoubleDataField {
public:
  I18n::Message fieldLegend() const override { return I18n::Message::PeriodicMassLegend; }
  I18n::Message fieldSymbol() const override { return I18n::Message::PeriodicMassSymbol; }
  double getDouble(AtomicNumber z) const override;
  const char * fieldUnit() const override { return "_g*_mol^\u0012-1\u0013"; }
};

class ElectronegativityDataField : public DoubleDataField {
public:
  I18n::Message fieldLegend() const override { return I18n::Message::PeriodicElectronegativityLegend; }
  I18n::Message fieldSymbol() const override { return I18n::Message::PeriodicElectronegativitySymbol; }
  double getDouble(AtomicNumber z) const override;
  const char * fieldUnit() const override { return ""; }
};

class RadiusDataField : public DoubleDataField {
public:
  I18n::Message fieldLegend() const override { return I18n::Message::PeriodicRadiusLegend; }
  I18n::Message fieldSymbol() const override { return I18n::Message::PeriodicRadiusSymbol; }
  double getDouble(AtomicNumber z) const override;
  const char * fieldUnit() const override { return "_pm"; }
};

class MeltingPointDataField : public DoubleDataFieldWithSubscriptSymbol {
public:
  I18n::Message fieldLegend() const override { return I18n::Message::PeriodicMeltingPointLegend; }
  I18n::Message fieldSymbol() const override { return I18n::Message::PeriodicMeltingPointSymbol; }
  double getDouble(AtomicNumber z) const override;
  const char * fieldUnit() const override { return "_°C"; }
  I18n::Message fieldSubscript() const override { return I18n::Message::PeriodicMeltingPointSubscript; }
};

class BoilingPointDataField : public DoubleDataFieldWithSubscriptSymbol {
public:
  I18n::Message fieldLegend() const override { return I18n::Message::PeriodicBoilingPointLegend; }
  I18n::Message fieldSymbol() const override { return I18n::Message::PeriodicBoilingPointSymbol; }
  double getDouble(AtomicNumber z) const override;
  const char * fieldUnit() const override { return "_°C"; }
  I18n::Message fieldSubscript() const override { return I18n::Message::PeriodicBoilingPointSubscript; }
};

class DensityDataField : public DoubleDataField {
public:
  I18n::Message fieldLegend() const override { return I18n::Message::PeriodicDensityLegend; }
  I18n::Message fieldSymbol() const override { return I18n::Message::PeriodicDensitySymbol; }
  double getDouble(AtomicNumber z) const override;
  const char * fieldUnit() const override { return "_g*_cm^\u0012-3\u0013"; }
};

class AffinityDataField : public DoubleDataFieldWithSubscriptSymbol {
public:
  I18n::Message fieldLegend() const override { return I18n::Message::PeriodicAffinityLegend; }
  I18n::Message fieldSymbol() const override { return I18n::Message::PeriodicAffinitySymbol; }
  double getDouble(AtomicNumber z) const override;
  const char * fieldUnit() const override { return "_kJ*_mol^\u0012-1\u0013"; }
  I18n::Message fieldSubscript() const override { return I18n::Message::PeriodicAffinitySubscript; }
};

class IonisationDataField : public DoubleDataFieldWithSubscriptSymbol {
public:
  I18n::Message fieldLegend() const override { return I18n::Message::PeriodicIonisationLegend; }
  I18n::Message fieldSymbol() const override { return I18n::Message::PeriodicIonisationSymbol; }
  double getDouble(AtomicNumber z) const override;
  const char * fieldUnit() const override { return "_kJ*_mol^\u0012-1\u0013"; }
  I18n::Message fieldSubscript() const override { return I18n::Message::PeriodicIonisationSubscript; }
};



}

#endif
