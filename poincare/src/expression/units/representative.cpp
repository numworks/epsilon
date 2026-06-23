#include "representative.h"

#include <string.h>

#include "../simplification.h"
#include "helpers.h"
#include "prefix.h"
#include "representatives.h"

namespace Poincare::Internal {
namespace Units {

const Representative* const* Representative::DefaultRepresentatives() {
  static const Representative* defaultRepresentatives[k_numberOfDimensions] = {
#if POINCARE_UNIT
    Angle::representatives,
#if !POINCARE_UNIT_ONLY_ANGLES
    Time::representatives,
    Distance::representatives,
    Mass::representatives,
    Current::representatives,
    Temperature::representatives,
    AmountOfSubstance::representatives,
    LuminousIntensity::representatives,
    Frequency::representatives,
    Force::representatives,
    Pressure::representatives,
    Energy::representatives,
    Power::representatives,
    ElectricCharge::representatives,
    ElectricPotential::representatives,
    ElectricCapacitance::representatives,
    ElectricResistance::representatives,
    ElectricConductance::representatives,
    MagneticFlux::representatives,
    MagneticField::representatives,
    Inductance::representatives,
    CatalyticActivity::representatives,
    Surface::representatives,
    Volume::representatives,
    Speed::representatives,
#endif
#endif
    // Update AllRepresentatives in units/k_units.h if you change this
  };
  return defaultRepresentatives;
}

uint8_t Representative::ToId(const Representative* representative) {
  uint8_t id = 0;
  const Representative* list = representative->representativesOfSameDimension();
  for (int i = 0; i < k_numberOfDimensions; i++) {
    if (list == DefaultRepresentatives()[i]) {
      int representativeOffset = (representative - list);
      assert(representativeOffset < list->numberOfRepresentatives());
      return id + representativeOffset;
    } else {
      id += DefaultRepresentatives()[i]->numberOfRepresentatives();
    }
  }
  assert(false);
  return 0;
}

const Representative* Representative::FromId(uint8_t id) {
  for (int i = 0; i < k_numberOfDimensions; i++) {
    const Representative* list = Representative::DefaultRepresentatives()[i];
    int listSize = list->numberOfRepresentatives();
    if (id < listSize) {
      return list + id;
    }
    id -= listSize;
  }
  assert(false);
  return Representative::DefaultRepresentatives()[0];
}

const Representative* Representative::RepresentativeForDimension(
    SIVector vector) {
  for (int i = 0; i < k_numberOfDimensions; i++) {
    const Representative* representative =
        Representative::DefaultRepresentatives()[i];
    if (vector == representative->siVector()) {
      return representative;
    }
  }
  return nullptr;
}

const Representative* Representative::defaultFindBestRepresentativeAndPrefix(
    double value, double exponent, const Representative* begin,
    const Representative* end, const Prefix** prefix) const {
  assert(begin < end);
  /* Return this if every other representative gives an accuracy of 0 or Inf.
   * This can happen when searching for an Imperial representative for 1m^20000
   * for example. */
  const Representative* result = this;
  double accuracy = 0.;
  const Prefix* currentPrefix = Prefix::EmptyPrefix();
  const Representative* currentRepresentative = begin;
  while (currentRepresentative < end) {
    double currentAccuracy =
        std::fabs(value / std::pow(currentRepresentative->ratio(), exponent));
    if (*prefix) {
      currentPrefix = ChooseBestPrefix(currentRepresentative->outputPrefixSet(),
                                       currentAccuracy, exponent);
    }
    if (CompareMagnitudeOrders(
            std::log10(currentAccuracy) - currentPrefix->exponent() * exponent,
            std::log10(accuracy) -
                ((!*prefix) ? 0 : (*prefix)->exponent() * exponent))) {
      accuracy = currentAccuracy;
      result = currentRepresentative;
      *prefix = currentPrefix;
    }
    currentRepresentative++;
  }
  if (!*prefix) {
    *prefix = Prefix::EmptyPrefix();
  }
  return result;
}

bool Representative::canParseWithEquivalents(
    const char* symbol, size_t length, const Representative** representative,
    const Prefix** prefix) const {
  const Representative* candidate = representativesOfSameDimension();
  if (!candidate) {
    return false;
  }
  for (int i = 0; i < numberOfRepresentatives(); i++) {
    Aliases rootSymbolAliasesList = (candidate + i)->rootSymbols();
    for (const char* rootSymbolAlias : rootSymbolAliasesList) {
      size_t rootSymbolLength = strlen(rootSymbolAlias);
      int potentialPrefixLength = length - rootSymbolLength;
      if (potentialPrefixLength >= 0 &&
          strncmp(rootSymbolAlias, symbol + potentialPrefixLength,
                  rootSymbolLength) == 0 &&
          candidate[i].canParse(symbol, potentialPrefixLength, prefix)) {
        if (representative) {
          *representative = (candidate + i);
        }
        return true;
      }
    }
  }
  return false;
}

bool Representative::canParse(const char* symbol, size_t length,
                              const Prefix** prefix) const {
  if (inputPrefixSet() == PrefixSet::None) {
    if (prefix) {
      *prefix = Prefix::EmptyPrefix();
    }
    return length == 0;
  }
  for (size_t i = 0; i < Prefix::k_numberOfPrefixes; i++) {
    const Prefix* pre = Prefix::Prefixes() + i;
    const char* prefixSymbol = pre->symbol();
    if (strlen(prefixSymbol) == length &&
        PrefixSetContainsPrefix(m_inputPrefixSet, pre) &&
        strncmp(symbol, prefixSymbol, length) == 0) {
      if (prefix) {
        *prefix = pre;
      }
      return true;
    }
  }
  return false;
}

Tree* Representative::pushReducedRatioExpression() const {
  Tree* result = ratioExpression()->cloneTree();
  /* Representative's ratio expressions are quite simple and not dependent on
   * any context. */
  Simplification::ProjectAndReduce(
      result, SimplificationParameters{.reductionPolicy = {.advanced = false}});
  return result;
}

}  // namespace Units
}  // namespace Poincare::Internal
