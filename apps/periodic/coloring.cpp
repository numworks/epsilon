#include "coloring.h"
#include "assert.h"
#include <apps/shared/poincare_helpers.h>

namespace Periodic {

// Coloring

void Coloring::setLegendForElement(AtomicNumber z, char * buffer, size_t bufferSize) const {
  int numberOfChars = strlcpy(buffer, I18n::translate(titleForElement(z)), bufferSize);
  numberOfChars += legendContentForElement(z, buffer + numberOfChars, bufferSize - numberOfChars);
  assert(numberOfChars < bufferSize - 1);
}

// ContinuousColoring

Coloring::ColorPair ContinuousColoring::colorPairForElement(AtomicNumber z) const {
  ColorPair min = minimalColors();
  ColorPair max = maximalColors();
  return ColorPair(blendAlphaForContinuousParameter(z), min.fg(), max.fg(), min.bg(), max.bg());
}

size_t ContinuousColoring::legendContentForElement(AtomicNumber z, char * buffer, size_t bufferSize) const {
  buffer[0] = ' ';
  buffer[1] = ':';
  buffer[2] = ' ';
  int numberOfChars = 3;

  numberOfChars += Shared::PoincareHelpers::ConvertFloatToText(parameter(z), buffer + numberOfChars, bufferSize - numberOfChars, Poincare::Preferences::sharedPreferences()->numberOfSignificantDigits());

  buffer[numberOfChars++] = ' ';

  numberOfChars += strlcpy(buffer + numberOfChars, unit(), bufferSize - numberOfChars);
  assert(numberOfChars < bufferSize - 1);
  return numberOfChars;
}

uint8_t ContinuousColoring::blendAlphaForContinuousParameter(AtomicNumber z) const {
  /* FIXME We will recompute alpha for each element when redrawing the whole
   * table. We could memoize the sorted indexes to speed up the process. */
  float numberOfLowerValues = 0;
  double v = parameter(z);
  for (AtomicNumber z2 = 1; z2 <= ElementsDataBase::k_numberOfElements; z2++) {
    if (parameter(z2) < v) {
      numberOfLowerValues++;
    }
  }
  return static_cast<uint8_t>(numberOfLowerValues / (ElementsDataBase::k_numberOfElements - 1.f) * ((1 << 8) - 1.f));
}

// GroupsColoring

Coloring::ColorPair GroupsColoring::colorPairForElement(AtomicNumber z) const {
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
  ElementData::Group group = ElementsDataBase::Group(z);
  assert(static_cast<uint8_t>(group) < sizeof(k_colors) / sizeof(k_colors[0]));
  return k_colors[static_cast<uint8_t>(group)];
}

I18n::Message GroupsColoring::titleForElement(AtomicNumber z) const {
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

}
