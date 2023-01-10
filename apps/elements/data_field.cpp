#include "data_field.h"
#include "elements_data_base.h"
#include <apps/apps_container.h>
#include <poincare/code_point_layout.h>
#include <poincare/float.h>
#include <poincare/horizontal_layout.h>
#include <poincare/multiplication.h>
#include <poincare/vertical_offset_layout.h>

using namespace Poincare;

namespace Elements {

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
  Preferences::PrintFloatMode floatDisplayMode = Preferences::sharedPreferences()->displayMode();
  double v = getDouble(z);
  if (!std::isfinite(v)) {
    return DataField::UnknownValueLayout();
  }

  Expression value = Float<double>::Builder(v);
  /* Check the global context to know whether units need an underscore. */
  Context * globalContext = AppsContainer::sharedAppsContainer()->globalContext();
  Expression unit = Expression::Parse(rawUnit(), globalContext);

  if (unit.isUninitialized()) {
    return value.createLayout(floatDisplayMode, significantDigits, globalContext);
  }

  Expression result;
  if (unit.type() == ExpressionNode::Type::Multiplication) {
    static_cast<Multiplication&>(unit).addChildAtIndexInPlace(value, 0, unit.numberOfChildren());
    result = unit;
  } else {
    result = Multiplication::Builder(value, unit);
  }
  return result.createLayout(floatDisplayMode, significantDigits, globalContext);
}

DataField::ColorPair DoubleDataField::getColors(AtomicNumber z) const {
  if (!std::isfinite(getDouble(z))) {
    return ColorPair(Palette::ElementGrayDark, Palette::ElementGrayLight);
  }
  ColorPair min = minColors();
  ColorPair max = maxColors();
  return ColorPair(blendAlphaForContinuousParameter(z), min.fg(), max.fg(), min.bg(), max.bg());
}

uint8_t DoubleDataField::blendAlphaForContinuousParameter(AtomicNumber z) const {
  /* When drawing the whole table, the sorting order will be computed for each
   * element. This is still fast enough to not affect the display. */
  assert(ElementsDataBase::IsElement(z));
  float numberOfLowerValues = 0;
  double v = getDouble(z);
  for (AtomicNumber z2 = 1; z2 <= ElementsDataBase::k_numberOfElements; z2++) {
    if (getDouble(z2) < v) {
      numberOfLowerValues++;
    }
  }
  return static_cast<uint8_t>(numberOfLowerValues * ((1 << 8) - 1.f) / (ElementsDataBase::k_numberOfElements - 1.f));
}

// DoubleDataFieldWithSubscriptSymbol

Layout DoubleDataFieldWithSubscriptSymbol::fieldSymbolLayout() const {
  HorizontalLayout res = HorizontalLayout::Builder(DataField::fieldSymbolLayout());
  const char * subscript = I18n::translate(fieldSubscript());
  if (subscript[0] != '\0') {
    res.addOrMergeChildAtIndex(VerticalOffsetLayout::Builder(LayoutHelper::String(subscript), VerticalOffsetLayoutNode::VerticalPosition::Subscript), 1, false);
  }
  return std::move(res);
}

// ADataField

Layout ADataField::getLayout(AtomicNumber z, int) const {
  uint16_t a = ElementsDataBase::NumberOfMass(z);
  if (a == ElementData::k_AUnknown) {
    return DataField::UnknownValueLayout();
  }
  return Integer(ElementsDataBase::NumberOfMass(z)).createLayout();
}

// ConfigurationDataField

int maxPopulation(int l) {
  return 2 * (2 * l + 1);
}

void nextSubshell(int * n, int * l) {
  assert(n && l && *n >= 1 && *l >= 0 && *l < *n);
  /* Subshells are filled in order of increasing n+l then increasing n. */
  if (*l > 0) {
    /* 4p (n=4, l=1) -> 5s (n=5, l=0) */
    *l = *l - 1;
    *n = *n + 1;
  } else {
    /* 5s (n=5, l=0) -> 4d (n=4, l=2) */
    *l = *n / 2;
    *n = *n + 1 - *l;
  }
}

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
  int n = 1, l = 0;
  if (previousNoble >= 0) {
    electrons -= k_nobles[previousNoble];
    n = previousNoble + 3;
    res.addOrMergeChildAtIndex(HorizontalLayout::Builder(CodePointLayout::Builder('['), LayoutHelper::String(ElementsDataBase::Symbol(k_nobles[previousNoble])), CodePointLayout::Builder(']')), 0, false);
  }
  while (electrons > 0) {
    int index = (n - 1) * k_lMax + l;
    int maxPop = maxPopulation(l);
    int pop = maxPop > electrons ? electrons : maxPop;
    electrons -= pop;
    conf[index] = pop;
    nextSubshell(&n, &l);
  }

  /* Some elements have irregular configurations */
  int nD = -1, lD = -1;
  int numberOfDisplacedElectrons = DisplacedElectrons(z, &nD, &lD);
  if (numberOfDisplacedElectrons > 0) {
    size_t fromIndex = (nD - 1) * k_lMax + lD;
    size_t toIndex;
    do {
      nextSubshell(&nD, &lD);
      toIndex = (nD - 1) * k_lMax + lD;
    } while (conf[toIndex] == maxPopulation(lD));
    assert(conf[fromIndex] >= numberOfDisplacedElectrons);
    conf[fromIndex] -= numberOfDisplacedElectrons;
    conf[toIndex] += numberOfDisplacedElectrons;
  }

  /* Subshells are displayed in order of increasing n, then increasing l. */
  for (n = 1; n <= k_nMax; n++) {
    for (l = 0; l < k_lMax && l < n; l++) {
      int index = (n - 1) * k_lMax + l;
      if (conf[index] == 0) {
        continue;
      }
      Layout term = HorizontalLayout::Builder(Integer(n).createLayout(), CodePointLayout::Builder(k_lSymbols[l]), VerticalOffsetLayout::Builder(Integer(conf[index]).createLayout(), VerticalOffsetLayoutNode::VerticalPosition::Superscript));
      res.addOrMergeChildAtIndex(term, res.numberOfChildren(), false);
    }
  }

  return std::move(res);
}

int ConfigurationDataField::DisplacedElectrons(AtomicNumber z, int * n, int * l) {
  assert(n && l);
  switch (z) {
  case 24:
  case 28:
  case 29:
    // 4s
    *n = 4;
    *l = 0;
    break;
  case 41:
  case 42:
  case 44:
  case 45:
  case 46:
  case 47:
    // 5s
    *n = 5;
    *l = 0;
    break;
  case 57:
  case 58:
  case 64:
    // 4f
    *n = 4;
    *l = 3;
    break;
  case 78:
  case 79:
    // 6s
    *n = 6;
    *l = 0;
    break;
  case 89:
  case 90:
  case 91:
  case 92:
  case 93:
  case 96:
    // 5f
    *n = 5;
    *l = 3;
    break;
  case 103:
    // 6d
    *n = 6;
    *l = 2;
    break;
  default:
    return 0;
  }
  return z == 46 || z == 90 ? 2 : 1;
}

// GroupDataField

I18n::Message GroupDataField::protectedGetMessage(AtomicNumber z) const {
  constexpr I18n::Message k_titles[] = {
    I18n::Message::ElementsGroupAlkali,
    I18n::Message::ElementsGroupAlkalineEarth,
    I18n::Message::ElementsGroupTransitionMetal,
    I18n::Message::ElementsGroupPostTransitionMetal,
    I18n::Message::ElementsGroupLanthanide,
    I18n::Message::ElementsGroupActinide,
    I18n::Message::ElementsGroupMetalloid,
    I18n::Message::ElementsGroupNonmetal,
    I18n::Message::ElementsGroupHalogen,
    I18n::Message::ElementsGroupNobleGas,
    I18n::Message::ElementsGroupUnidentified,
  };
  ElementData::Group group = ElementsDataBase::Group(z);
  assert(static_cast<uint8_t>(group) < sizeof(k_titles) / sizeof(k_titles[0]));
  return k_titles[static_cast<uint8_t>(group)];
}

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

// BlockDataField

I18n::Message BlockDataField::protectedGetMessage(AtomicNumber z) const {
  constexpr I18n::Message k_titles[] = {
    I18n::Message::ElementsBlockS,
    I18n::Message::ElementsBlockP,
    I18n::Message::ElementsBlockD,
    I18n::Message::ElementsBlockF,
  };
  ElementData::Block block = ElementsDataBase::Block(z);
  assert(static_cast<uint8_t>(block) < sizeof(k_titles) / sizeof(k_titles[0]));
  return k_titles[static_cast<uint8_t>(block)];
}

DataField::ColorPair BlockDataField::getColors(AtomicNumber z) const {
  constexpr ColorPair k_colors[] = {
    /* s */ ColorPair(Palette::ElementRedDark, Palette::ElementRedLight),
    /* p */ ColorPair(Palette::ElementBlueDark, Palette::ElementBlueLight),
    /* d */ ColorPair(Palette::ElementOrangeDark, Palette::ElementOrangeLight),
    /* f */ ColorPair(Palette::ElementGreenDark, Palette::ElementGreenLight),
  };
  ElementData::Block block = ElementsDataBase::Block(z);
  assert(static_cast<uint8_t>(block) < sizeof(k_colors) / sizeof(k_colors[0]));
  return k_colors[static_cast<uint8_t>(block)];
}

// MetalDataField

I18n::Message MetalDataField::protectedGetMessage(AtomicNumber z) const {
  ElementData::Group group = ElementsDataBase::Group(z);
  if (ElementsDataBase::IsMetal(z) || group == ElementData::Group::Metalloid || group == ElementData::Group::Unidentified) {
    return ElementsDataBase::GroupField.protectedGetMessage(z);
  }
  return I18n::Message::ElementsGroupNonmetal;
}

DataField::ColorPair MetalDataField::getColors(AtomicNumber z) const {
  ElementData::Group group = ElementsDataBase::Group(z);
  if (group == ElementData::Group::Unidentified) {
    return ElementsDataBase::GroupField.getColors(z);
  } else if (group == ElementData::Group::Metalloid) {
    return ColorPair(Palette::MetallicRedDark, Palette::MetallicRedLight);
  } else if (!ElementsDataBase::IsMetal(z)) {
    return ColorPair(Palette::ElementOrangeDark, Palette::ElementOrangeLight);
  }
  constexpr ColorPair k_colors[] = {
    ColorPair(Palette::MetallicBlueDark, Palette::MetallicBlueLight),
    ColorPair(Palette::MetallicTealDark, Palette::MetallicTealLight),
    ColorPair(Palette::MetallicVerdigrisDark, Palette::MetallicVerdigrisLight),
    ColorPair(Palette::MetallicTurquoiseDark, Palette::MetallicTurquoiseLight),
    ColorPair(Palette::MetallicAquamarineDark, Palette::MetallicAquamarineLight),
    ColorPair(Palette::MetallicGreenDark, Palette::MetallicGreenLight),
  };
  assert(static_cast<uint8_t>(group) < sizeof(k_colors) / sizeof(k_colors[0]));
  return k_colors[static_cast<uint8_t>(group)];
}

// StateDataField

I18n::Message StateDataField::protectedGetMessage(AtomicNumber z) const {
  constexpr I18n::Message k_titles[] = {
    I18n::Message::ElementsStateSolid,
    I18n::Message::ElementsStateLiquid,
    I18n::Message::ElementsStateGas,
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

Layout AffinityDataField::getLayout(AtomicNumber z, int significantDigits) const {
  assert(std::isinf(ElementData::k_affinityUnstable));
  if (std::isinf(getDouble(z))) {
    return LayoutHelper::String("Unstable");
  }
  return DoubleDataFieldWithSubscriptSymbol::getLayout(z, significantDigits);
}

// IonizationDataField

double IonizationDataField::getDouble(AtomicNumber z) const {
  return ElementsDataBase::EnergyOfIonization(z);
}

}
