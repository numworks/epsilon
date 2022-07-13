#include "data_field.h"
#include "elements_data_base.h"
#include "palette.h"
#include <poincare/code_point_layout.h>
#include <poincare/float.h>
#include <poincare/horizontal_layout.h>
#include <poincare/multiplication.h>
#include <poincare/vertical_offset_layout.h>

using namespace Poincare;

namespace Periodic {

// DataField

I18n::Message DataField::getMessage(AtomicNumber z) const {
  if (ElementsDataBase::IsElement(z)) {
    return protectedGetMessage(z);
  }
  return I18n::Message::Default;
}

// DoubleDataField

bool DoubleDataField::hasDouble(AtomicNumber z) const {
  return ElementsDataBase::IsElement(z);
}

Layout DoubleDataField::getLayout(AtomicNumber z, int significantDigits) const {
  assert(hasDouble(z));
  Expression value = Float<double>::Builder(getDouble(z));
  Expression unit = Expression::Parse(fieldUnit(), nullptr);
  Preferences::PrintFloatMode floatDisplayMode = Preferences::sharedPreferences()->displayMode();
  if (unit.isUninitialized()) {
    return value.createLayout(floatDisplayMode, significantDigits, nullptr);
  }
  return Multiplication::Builder(value, unit).createLayout(floatDisplayMode, significantDigits, nullptr);
}

DataField::ColorPair DoubleDataField::getColors(AtomicNumber z) const {
  ColorPair min = minColors();
  ColorPair max = maxColors();
  return ColorPair(blendAlphaForContinuousParameter(z), min.fg(), max.fg(), min.bg(), max.bg());
}

uint8_t DoubleDataField::blendAlphaForContinuousParameter(AtomicNumber z) const {
  /* FIXME We will recompute alpha for each element when redrawing the whole
   * table. We could memoize the sorted indexes to speed up the process. */
  assert(ElementsDataBase::IsElement(z));
  float numberOfLowerValues = 0;
  double v = getDouble(z);
  for (AtomicNumber z2 = 1; z2 <= ElementsDataBase::k_numberOfElements; z2++) {
    if (getDouble(z2) < v) {
      numberOfLowerValues++;
    }
  }
  return static_cast<uint8_t>(numberOfLowerValues / (ElementsDataBase::k_numberOfElements - 1.f) * ((1 << 8) - 1.f));
}

// DoubleDataFieldWithSubscriptSymbol

Layout DoubleDataFieldWithSubscriptSymbol::fieldSymbolLayout() const {
  HorizontalLayout res = HorizontalLayout::Builder(DataField::fieldSymbolLayout());
  res.addOrMergeChildAtIndex(VerticalOffsetLayout::Builder(LayoutHelper::String(I18n::translate(fieldSubscript())), VerticalOffsetLayoutNode::Position::Subscript), 1, false);
  return std::move(res);
}

// ADataField

Layout ADataField::getLayout(AtomicNumber z, int) const {
  return Integer(ElementsDataBase::NumberOfMass(z)).createLayout();
}

// ConfigurationDataField

Layout ConfigurationDataField::getLayout(AtomicNumber z, int) const {
  constexpr int k_nMax = 7;
  constexpr int k_lMax = 4;
  constexpr char k_lSymbols[k_lMax] = { 's', 'p', 'd', 'f' };
  constexpr AtomicNumber k_nobles[] = { 10, 18, 36, 54, 86 };
  constexpr size_t k_numberOfNobles = sizeof(k_nobles) / sizeof(k_nobles[0]);

  int conf[k_nMax * k_lMax];
  for (int i = 0; i < k_nMax * k_lMax; i++) {
    conf[i] = 0;
  }

  int previousNoble = -1;
  for (int g = k_numberOfNobles - 1; g >= 0; g--) {
    if (k_nobles[g] < z) {
      previousNoble = g;
      break;
    }
  }

  HorizontalLayout res = HorizontalLayout::Builder();
  int electrons = z;
  int n = 0, l = 0;
  if (previousNoble >= 0) {
    electrons -= k_nobles[previousNoble];
    l = 0;
    n = previousNoble + 2;
    res.addOrMergeChildAtIndex(HorizontalLayout::Builder(CodePointLayout::Builder('['), LayoutHelper::String(ElementsDataBase::Symbol(k_nobles[previousNoble])) ,CodePointLayout::Builder(']')), 0, false);
  }
  while (electrons > 0) {
    int index = n * k_lMax + l;
    int maxPop = 2 * (2 * l + 1);
    int pop = maxPop > electrons ? electrons : maxPop;
    electrons -= pop;
    conf[index] = pop;
    /* Subshells are filled in order of increasing n+l then increasing n. */
    if (l > 0) {
      l--;
      n++;
    } else {
      l = (n + 1) / 2;
      n += 1 - l;
    }
  }

  /* Subshells are displayed in order of increasing n, then increasing l. */
  for (n = 0; n < k_nMax; n++) {
    for (l = 0; l < k_lMax; l++) {
      int index = n * k_lMax + l;
      if (l > n || conf[index] == 0) {
        continue;
      }
      Layout term = HorizontalLayout::Builder(Integer(n + 1).createLayout(), CodePointLayout::Builder(k_lSymbols[l]), VerticalOffsetLayout::Builder(Integer(conf[index]).createLayout(), VerticalOffsetLayoutNode::Position::Superscript));
      res.addOrMergeChildAtIndex(term, res.numberOfChildren(), false);
    }
  }

  return std::move(res);
}

// GroupDataField

DataField::ColorPair GroupDataField::getColors(AtomicNumber z) const {
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
    ColorPair(Palette::ElementGrayDark, Palette::ElementGrayLight),
  };
  ElementData::Group group = ElementsDataBase::Group(z);
  assert(static_cast<uint8_t>(group) < sizeof(k_colors) / sizeof(k_colors[0]));
  return k_colors[static_cast<uint8_t>(group)];
}

I18n::Message GroupDataField::protectedGetMessage(AtomicNumber z) const {
  constexpr I18n::Message k_titles[] = {
    I18n::Message::PeriodicGroupAlkali,
    I18n::Message::PeriodicGroupAlkalineEarth,
    I18n::Message::PeriodicGroupTransitionMetal,
    I18n::Message::PeriodicGroupPostTransitionMetal,
    I18n::Message::PeriodicGroupLanthanide,
    I18n::Message::PeriodicGroupActinide,
    I18n::Message::PeriodicGroupMetalloid,
    I18n::Message::PeriodicGroupNonmetal,
    I18n::Message::PeriodicGroupHalogen,
    I18n::Message::PeriodicGroupNobleGas,
    I18n::Message::PeriodicGroupUnidentified,
  };
  ElementData::Group group = ElementsDataBase::Group(z);
  assert(static_cast<uint8_t>(group) < sizeof(k_titles) / sizeof(k_titles[0]));
  return k_titles[static_cast<uint8_t>(group)];
}

// BlockDataField

I18n::Message BlockDataField::protectedGetMessage(AtomicNumber z) const {
  constexpr I18n::Message k_titles[] = {
    I18n::Message::PeriodicBlockS,
    I18n::Message::PeriodicBlockP,
    I18n::Message::PeriodicBlockD,
    I18n::Message::PeriodicBlockF,
  };
  ElementData::Block block = ElementsDataBase::Block(z);
  assert(static_cast<uint8_t>(block) < sizeof(k_titles) / sizeof(k_titles[0]));
  return k_titles[static_cast<uint8_t>(block)];
}

// MetalDataField

I18n::Message MetalDataField::protectedGetMessage(AtomicNumber z) const {
  if (ElementsDataBase::IsMetal(z)) {
    return ElementsDataBase::GroupField.protectedGetMessage(z);
  }
  return I18n::Message::PeriodicGroupNonmetal;
}

// StateDataField

I18n::Message StateDataField::protectedGetMessage(AtomicNumber z) const {
  constexpr I18n::Message k_titles[] = {
    I18n::Message::PeriodicStateSolid,
    I18n::Message::PeriodicStateLiquid,
    I18n::Message::PeriodicStateGas,
  };
  ElementData::PhysicalState state = ElementsDataBase::PhysicalState(z);
  assert(static_cast<uint8_t>(state) < sizeof(k_titles) / sizeof(k_titles[0]));
  return k_titles[static_cast<uint8_t>(state)];
}

// MassDataField

double MassDataField::getDouble(AtomicNumber z) const {
  return ElementsDataBase::MolarMass(z);
}

// ElectronegativityDataField

double ElectronegativityDataField::getDouble(AtomicNumber z) const {
  return ElementsDataBase::Electronegativity(z);
}

// RadiusDataField

double RadiusDataField::getDouble(AtomicNumber z) const {
  return ElementsDataBase::Radius(z);
}

// MeltingPointDataField

double MeltingPointDataField::getDouble(AtomicNumber z) const {
  return ElementsDataBase::MeltingPoint(z);
}

// BoilingPointDataField

double BoilingPointDataField::getDouble(AtomicNumber z) const {
  return ElementsDataBase::BoilingPoint(z);
}

// DensityDataField

double DensityDataField::getDouble(AtomicNumber z) const {
  return ElementsDataBase::Density(z);
}

// AffinityDataField

double AffinityDataField::getDouble(AtomicNumber z) const {
  return ElementsDataBase::Affinity(z);
}

// IonisationDataField

double IonisationDataField::getDouble(AtomicNumber z) const {
  return ElementsDataBase::EnergyOfIonisation(z);
}

}
