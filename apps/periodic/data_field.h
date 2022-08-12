#ifndef PERIODIC_DATA_FIELD
#define PERIODIC_DATA_FIELD

#include "palette.h"
#include <apps/i18n.h>
#include <poincare/integer.h>
#include <poincare/layout_helper.h>
#include <stddef.h>

namespace Periodic {

/* Methods taking an AtomicNumber expect a number from 1 to 118 (i.e. 1-based
 * numbering). */
typedef uint8_t AtomicNumber;

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
  Poincare::Layout getLayout(AtomicNumber z, int significantDigits = k_defaultSignificantDigits) const override { return Poincare::LayoutHelper::String(I18n::translate(getMessage(z))); }
};

class DoubleDataField : public DataField {
public:
  bool hasDouble(AtomicNumber z) const override;
  Poincare::Layout getLayout(AtomicNumber z, int significantDigits = k_defaultSignificantDigits) const override;
  ColorPair getColors(AtomicNumber z) const override;

private:
  virtual const char * rawUnit() const = 0;
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

private:
  static int DisplacedElectrons(AtomicNumber z, int * n, int * l);
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
  ColorPair getColors(AtomicNumber z) const override;

private:
  static bool DeferToGroupDataField(AtomicNumber z);
};

class StateDataField : public EnumDataField {
public:
  I18n::Message fieldSymbol() const override { return I18n::Message::PeriodicStateSymbol; }
  I18n::Message protectedGetMessage(AtomicNumber z) const override;
};

class MassDataField : public DoubleDataField {
public:
  I18n::Message fieldLegend() const override { return I18n::Message::PeriodicMassLegend; }
  I18n::Message fieldSymbol() const override { return I18n::Message::PeriodicMassSymbol; }
  double getDouble(AtomicNumber z) const override;
  const char * rawUnit() const override { return "_g×_mol^\x12-1\x13"; }
  ColorPair minColors() const override { return ColorPair(Palette::ScaleTextBlueLight, Palette::ScaleBackBlueLight); }
  ColorPair maxColors() const override { return ColorPair(Palette::ScaleTextBlueDark, Palette::ScaleBackBlueDark); }
};

class ElectronegativityDataField : public DoubleDataField {
public:
  I18n::Message fieldLegend() const override { return I18n::Message::PeriodicElectronegativityLegend; }
  I18n::Message fieldSymbol() const override { return I18n::Message::PeriodicElectronegativitySymbol; }
  double getDouble(AtomicNumber z) const override;
  const char * rawUnit() const override { return ""; }
  ColorPair minColors() const override { return ColorPair(Palette::ScaleTextYellowLight, Palette::ScaleBackYellowLight); }
  ColorPair maxColors() const override { return ColorPair(Palette::ScaleTextYellowDark, Palette::ScaleBackYellowDark); }
};

class RadiusDataField : public DoubleDataField {
public:
  I18n::Message fieldLegend() const override { return I18n::Message::PeriodicRadiusLegend; }
  I18n::Message fieldSymbol() const override { return I18n::Message::PeriodicRadiusSymbol; }
  double getDouble(AtomicNumber z) const override;
  const char * rawUnit() const override { return "_pm"; }
  ColorPair minColors() const override { return ColorPair(Palette::ScaleTextGreenLight, Palette::ScaleBackGreenLight); }
  ColorPair maxColors() const override { return ColorPair(Palette::ScaleTextGreenDark, Palette::ScaleBackGreenDark); }
};

class MeltingPointDataField : public DoubleDataFieldWithSubscriptSymbol {
public:
  I18n::Message fieldLegend() const override { return I18n::Message::PeriodicMeltingPointLegend; }
  I18n::Message fieldSymbol() const override { return I18n::Message::PeriodicMeltingPointSymbol; }
  double getDouble(AtomicNumber z) const override;
  const char * rawUnit() const override { return "_°C"; }
  I18n::Message fieldSubscript() const override { return I18n::Message::PeriodicMeltingPointSubscript; }
  ColorPair minColors() const override { return ColorPair(Palette::ScaleTextPurpleLight, Palette::ScaleBackPurpleLight); }
  ColorPair maxColors() const override { return ColorPair(Palette::ScaleTextPurpleDark, Palette::ScaleBackPurpleDark); }
};

class BoilingPointDataField : public DoubleDataFieldWithSubscriptSymbol {
public:
  I18n::Message fieldLegend() const override { return I18n::Message::PeriodicBoilingPointLegend; }
  I18n::Message fieldSymbol() const override { return I18n::Message::PeriodicBoilingPointSymbol; }
  double getDouble(AtomicNumber z) const override;
  const char * rawUnit() const override { return "_°C"; }
  I18n::Message fieldSubscript() const override { return I18n::Message::PeriodicBoilingPointSubscript; }
  ColorPair minColors() const override { return ColorPair(Palette::ScaleTextRedLight, Palette::ScaleBackRedLight); }
  ColorPair maxColors() const override { return ColorPair(Palette::ScaleTextRedDark, Palette::ScaleBackRedDark); }
};

class DensityDataField : public DoubleDataField {
public:
  I18n::Message fieldLegend() const override { return I18n::Message::PeriodicDensityLegend; }
  I18n::Message fieldSymbol() const override { return I18n::Message::PeriodicDensitySymbol; }
  double getDouble(AtomicNumber z) const override;
  const char * rawUnit() const override { return "_g×_cm^\x12-3\x13"; }
};

class AffinityDataField : public DoubleDataFieldWithSubscriptSymbol {
public:
  I18n::Message fieldLegend() const override { return I18n::Message::PeriodicAffinityLegend; }
  I18n::Message fieldSymbol() const override { return I18n::Message::PeriodicAffinitySymbol; }
  double getDouble(AtomicNumber z) const override;
  Poincare::Layout getLayout(AtomicNumber z, int significantDigits = k_defaultSignificantDigits) const override;
  const char * rawUnit() const override { return "_kJ×_mol^\x12-1\x13"; }
  I18n::Message fieldSubscript() const override { return I18n::Message::PeriodicAffinitySubscript; }
};

class IonizationDataField : public DoubleDataFieldWithSubscriptSymbol {
public:
  I18n::Message fieldLegend() const override { return I18n::Message::PeriodicIonizationLegend; }
  I18n::Message fieldSymbol() const override { return I18n::Message::PeriodicIonizationSymbol; }
  double getDouble(AtomicNumber z) const override;
  const char * rawUnit() const override { return "_kJ×_mol^\x12-1\x13"; }
  I18n::Message fieldSubscript() const override { return I18n::Message::PeriodicIonizationSubscript; }
};



}

#endif
