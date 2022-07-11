#include "display_type.h"
#include "elements_view_data_source.h"
#include <apps/shared/poincare_helpers.h>
#include <assert.h>

namespace Periodic {

// DisplayType

void DisplayType::setLegendForElement(AtomicNumber z, char * buffer, size_t bufferSize) const {
  int numberOfChars = strlcpy(buffer, I18n::translate(titleForElement(z)), bufferSize);
  numberOfChars += legendContentForElement(z, buffer + numberOfChars, bufferSize - numberOfChars);
  assert(numberOfChars < bufferSize - 1);
}

// ContinuousDisplayType

DisplayType::ColorPair ContinuousDisplayType::colorPairForElement(AtomicNumber z) const {
  assert(ElementsDataBase::IsElement(z));
  ColorPair min = minimalColors();
  ColorPair max = maximalColors();
  return ColorPair(blendAlphaForContinuousParameter(z), min.fg(), max.fg(), min.bg(), max.bg());
}

size_t ContinuousDisplayType::printValueWithUnitForElement(AtomicNumber z, char * buffer, size_t bufferSize) const {
  assert(ElementsDataBase::IsElement(z));

  size_t numberOfChars = Shared::PoincareHelpers::ConvertFloatToText(parameter(z), buffer, bufferSize, Poincare::Preferences::sharedPreferences()->numberOfSignificantDigits());

  if (unit() && unit()[0] != '\0') {
    buffer[numberOfChars++] = ' ';
    numberOfChars += strlcpy(buffer + numberOfChars, unit(), bufferSize - numberOfChars);
  }

  assert(numberOfChars < bufferSize - 1);
  return numberOfChars;
}

size_t ContinuousDisplayType::legendContentForElement(AtomicNumber z, char * buffer, size_t bufferSize) const {
  if (!ElementsDataBase::IsElement(z)) {
    return 0;
  }
  buffer[0] = ' ';
  buffer[1] = ':';
  buffer[2] = ' ';
  int numberOfChars = 3;

  return printValueWithUnitForElement(z, buffer + numberOfChars, bufferSize - numberOfChars);
}

uint8_t ContinuousDisplayType::blendAlphaForContinuousParameter(AtomicNumber z) const {
  /* FIXME We will recompute alpha for each element when redrawing the whole
   * table. We could memoize the sorted indexes to speed up the process. */
  assert(ElementsDataBase::IsElement(z));
  float numberOfLowerValues = 0;
  double v = parameter(z);
  for (AtomicNumber z2 = 1; z2 <= ElementsDataBase::k_numberOfElements; z2++) {
    if (parameter(z2) < v) {
      numberOfLowerValues++;
    }
  }
  return static_cast<uint8_t>(numberOfLowerValues / (ElementsDataBase::k_numberOfElements - 1.f) * ((1 << 8) - 1.f));
}

// GroupsDisplayType

DisplayType::ColorPair GroupsDisplayType::colorPairForElement(AtomicNumber z) const {
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

I18n::Message GroupsDisplayType::titleForElement(AtomicNumber z) const {
  if (!ElementsDataBase::IsElement(z)) {
    return I18n::Message::GroupOfElementsTitle;
  }
  constexpr I18n::Message k_titles[] = {
    I18n::Message::GroupAlkali,
    I18n::Message::GroupAlkalineEarth,
    I18n::Message::GroupTransitionMetal,
    I18n::Message::GroupPostTransitionMetal,
    I18n::Message::GroupLanthanide,
    I18n::Message::GroupActinide,
    I18n::Message::GroupMetalloid,
    I18n::Message::GroupNonmetal,
    I18n::Message::GroupHalogen,
    I18n::Message::GroupNobleGas,
    I18n::Message::GroupUnidentified,
  };
  ElementData::Group group = ElementsDataBase::Group(z);
  assert(static_cast<uint8_t>(group) < sizeof(k_titles) / sizeof(k_titles[0]));
  return k_titles[static_cast<uint8_t>(group)];
}

// StatesDisplayType

I18n::Message StatesDisplayType::titleForElement(AtomicNumber z) const {
  constexpr I18n::Message k_titles[] = {
    I18n::Message::StateSolid,
    I18n::Message::StateLiquid,
    I18n::Message::StateGas
  };
  ElementData::PhysicalState state = ElementsDataBase::PhysicalState(z);
  assert(static_cast<uint8_t>(state) < sizeof(k_titles) / sizeof(k_titles[0]));
  return k_titles[static_cast<uint8_t>(state)];
}

}
