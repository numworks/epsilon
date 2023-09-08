// #include <assert.h>
// #include <limits.h>
// #include <omg/round.h>
// #include <poincare/addition.h>
// #include <poincare/division.h>
// #include <poincare/float.h>
// #include <poincare/layout_helper.h>
// #include <poincare/multiplication.h>
// #include <poincare/power.h>
// #include <poincare/rational.h>
// #include <poincare/undefined.h>
// #include <poincare/unit.h>

// #include <algorithm>
// #include <array>
// #include <utility>
#include "unit.h"

#include <poincare_junior/src/n_ary.h>

#include "approximation.h"
#include "simplification.h"

namespace PoincareJ {

// UnitPrefix
const UnitPrefix* UnitPrefix::Prefixes() { return Unit::k_prefixes; }

const UnitPrefix* UnitPrefix::EmptyPrefix() {
  return Prefixes() + Unit::k_emptyPrefixIndex;
}

uint8_t UnitPrefix::ToId(const UnitPrefix* prefix) {
  uint8_t id = (prefix - Prefixes());
  assert(FromId(id) == prefix);
  return id;
}

const UnitPrefix* UnitPrefix::FromId(uint8_t id) {
  assert(id < k_numberOfPrefixes);
  return Prefixes() + id;
}

#if 0
int UnitPrefix::serialize(char* buffer, int bufferSize) const {
  assert(bufferSize >= 0);
  return std::min<int>(strlcpy(buffer, m_symbol, bufferSize), bufferSize - 1);
}
#endif

// Vector

DimensionVector DimensionVector::FromBaseUnits(const Tree* baseUnits) {
  /* Returns the vector of Base units with integer exponents. If rational, the
   * closest integer will be used. */
  DimensionVector vector = Empty();
  int numberOfFactors;
  int factorIndex = 0;
  const Tree* factor;
  if (baseUnits->type() == BlockType::Multiplication) {
    numberOfFactors = baseUnits->numberOfChildren();
    factor = baseUnits->childAtIndex(0);
  } else {
    numberOfFactors = 1;
    factor = baseUnits;
  }
  do {
    // Get the unit's exponent
    int8_t exponent = 1;
    if (factor->type() == BlockType::Power) {
      const Tree* exp = factor->childAtIndex(1);
      assert(exp->type().isRational());
      // Using the closest integer to the exponent.
      float exponentFloat = Approximation::To<float>(exp);
      if (exponentFloat != std::round(exponentFloat)) {
        /* If non-integer exponents are found, we round a null vector so that
         * Multiplication::shallowBeautify will not attempt to find derived
         * units. */
        return Empty();
      }
      /* We limit to INT_MAX / 3 because an exponent might get bigger with
       * simplification. As a worst case scenario, (_s²_m²_kg/_A²)^n should be
       * simplified to (_s^5_S)^n. If 2*n is under INT_MAX, 5*n might not. */
      if (std::fabs(exponentFloat) < INT8_MAX / 3) {
        // Exponent can be safely casted as int
        exponent = static_cast<int8_t>(std::round(exponentFloat));
        assert(std::fabs(exponentFloat - static_cast<float>(exponent)) <= 0.5f);
      } else {
        /* Base units vector will ignore this coefficient, to avoid exponent
         * overflow. In any way, shallowBeautify will conserve homogeneity. */
        exponent = 0;
      }
      factor = factor->childAtIndex(0);
    }
    // Fill the vector with the unit's exponent
    assert(factor->type() == BlockType::Unit);
    vector.addAllCoefficients(
        Unit::GetRepresentative(factor)->dimensionVector(),
        exponent);  // 7 in .time appears
    if (++factorIndex >= numberOfFactors) {
      break;
    }
    factor = baseUnits->childAtIndex(factorIndex);
  } while (true);
  return vector;
}

Tree* DimensionVector::toBaseUnits() const {
  Tree* result = SharedEditionPool->push<BlockType::Multiplication>(0);
  int numberOfChildren = 0;
  for (int i = 0; i < k_numberOfBaseUnits; i++) {
    // We require the base units to be the first seven in DefaultRepresentatives
    const UnitRepresentative* representative =
        UnitRepresentative::DefaultRepresentatives()[i];
    assert(representative);
    const UnitPrefix* prefix = representative->basePrefix();
    int8_t exponent = coefficientAtIndex(i);
    if (exponent == 0) {
      continue;
    }
    if (exponent == 1) {
      Unit::Push(representative, prefix);
    } else {
      SharedEditionPool->push<BlockType::Power>();
      Unit::Push(representative, prefix);
      IntegerHandler(exponent).pushOnEditionPool();
    }
    NAry::SetNumberOfChildren(result, ++numberOfChildren);
  }
  assert(numberOfChildren > 0);
  NAry::SquashIfUnary(result);
  return result;
}

void DimensionVector::addAllCoefficients(const DimensionVector other,
                                         int8_t factor) {
  for (uint8_t i = 0; i < k_numberOfBaseUnits; i++) {
    assert(static_cast<double>(coefficientAtIndex(i)) +
               static_cast<double>(other.coefficientAtIndex(i)) *
                   static_cast<double>(factor) <=
           static_cast<double>(INT8_MAX));
    assert(static_cast<double>(coefficientAtIndex(i)) +
               static_cast<double>(other.coefficientAtIndex(i)) *
                   static_cast<double>(factor) >=
           static_cast<double>(INT8_MIN));
    setCoefficientAtIndex(
        coefficientAtIndex(i) + other.coefficientAtIndex(i) * factor, i);
  }
}

void DimensionVector::setCoefficientAtIndex(int8_t coefficient, uint8_t i) {
  assert(i < k_numberOfBaseUnits);
  int8_t* coefficientsAddresses[] = {&time,
                                     &distance,
                                     &angle,
                                     &mass,
                                     &current,
                                     &temperature,
                                     &amountOfSubstance,
                                     &luminousIntensity};
  static_assert(std::size(coefficientsAddresses) == k_numberOfBaseUnits);
  *(coefficientsAddresses[i]) = coefficient;  // Something weird here ...
}

// UnitRepresentative
const UnitRepresentative* const* UnitRepresentative::DefaultRepresentatives() {
  constexpr static SpeedRepresentative defaultSpeedRepresentative =
      SpeedRepresentative::Default();
  constexpr static const UnitRepresentative*
      defaultRepresentatives[k_numberOfDimensions] = {
          Unit::k_timeRepresentatives,
          Unit::k_distanceRepresentatives,
          Unit::k_angleRepresentatives,
          Unit::k_massRepresentatives,
          Unit::k_currentRepresentatives,
          Unit::k_temperatureRepresentatives,
          Unit::k_amountOfSubstanceRepresentatives,
          Unit::k_luminousIntensityRepresentatives,
          Unit::k_frequencyRepresentatives,
          Unit::k_forceRepresentatives,
          Unit::k_pressureRepresentatives,
          Unit::k_energyRepresentatives,
          Unit::k_powerRepresentatives,
          Unit::k_electricChargeRepresentatives,
          Unit::k_electricPotentialRepresentatives,
          Unit::k_electricCapacitanceRepresentatives,
          Unit::k_electricResistanceRepresentatives,
          Unit::k_electricConductanceRepresentatives,
          Unit::k_magneticFluxRepresentatives,
          Unit::k_magneticFieldRepresentatives,
          Unit::k_inductanceRepresentatives,
          Unit::k_catalyticActivityRepresentatives,
          Unit::k_surfaceRepresentatives,
          Unit::k_volumeRepresentatives,
          &defaultSpeedRepresentative,
      };
  return defaultRepresentatives;
}

uint8_t UnitRepresentative::ToId(const UnitRepresentative* representative) {
  uint8_t id = 0;
  const UnitRepresentative* list =
      representative->representativesOfSameDimension();
  for (int i = 0; i < k_numberOfDimensions; i++) {
    if (list == DefaultRepresentatives()[i]) {
      size_t representativeOffset = (representative - list);
      assert(representativeOffset < list->numberOfRepresentatives());
      return id + representativeOffset;
    } else {
      id += DefaultRepresentatives()[i]->numberOfRepresentatives();
    }
  }
  assert(false);
  return 0;
}

const UnitRepresentative* UnitRepresentative::FromId(uint8_t id) {
  for (int i = 0; i < k_numberOfDimensions; i++) {
    const UnitRepresentative* list =
        UnitRepresentative::DefaultRepresentatives()[i];
    int listSize = list->numberOfRepresentatives();
    if (id < listSize) {
      return list + id;
    }
    id -= listSize;
  }
  assert(false);
  return UnitRepresentative::DefaultRepresentatives()[0];
}

const UnitRepresentative* UnitRepresentative::RepresentativeForDimension(
    DimensionVector vector) {
  for (int i = 0; i < k_numberOfDimensions; i++) {
    const UnitRepresentative* representative =
        UnitRepresentative::DefaultRepresentatives()[i];
    if (vector == representative->dimensionVector()) {
      return representative;
    }
  }
  return nullptr;
}

#if 0
static bool compareMagnitudeOrders(float order, float otherOrder) {
  /* Precision can be lost (with a year conversion for instance), so the order
   * value is rounded */
  order = OMG::LaxToZero(order);
  otherOrder = OMG::LaxToZero(otherOrder);
  if (std::fabs(std::fabs(order) - std::fabs(otherOrder)) <=
          3.0f + Float<float>::EpsilonLax() &&
      order * otherOrder < 0.0f) {
    /* If the two values are close, and their sign are opposed, the positive
     * order is preferred */
    return (order >= 0.0f);
  }
  // Otherwise, the closest order to 0 is preferred
  return (std::fabs(order) < std::fabs(otherOrder));
}

const UnitRepresentative* UnitRepresentative::defaultFindBestRepresentative(
    double value, double exponent, const UnitRepresentative* representatives,
    int length, const UnitPrefix** prefix) const {
  assert(length >= 1);
  /* Return this if every other representative gives an accuracy of 0 or Inf.
   * This can happen when searching for an Imperial representative for 1m^20000
   * for example. */
  const UnitRepresentative* result = this;
  double accuracy = 0.;
  const UnitPrefix* currentPrefix = UnitPrefix::EmptyPrefix();
  const UnitRepresentative* currentRepresentative = representatives;
  while (currentRepresentative < representatives + length) {
    double currentAccuracy =
        std::fabs(value / std::pow(currentRepresentative->ratio(), exponent));
    if (*prefix) {
      currentPrefix =
          currentRepresentative->findBestPrefix(currentAccuracy, exponent);
    }
    if (compareMagnitudeOrders(
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
    *prefix = result->basePrefix();
  }
  return result;
}

int UnitRepresentative::serialize(char* buffer, int bufferSize,
                              const UnitPrefix* prefix) const {
  int length = 0;
  length += prefix->serialize(buffer, bufferSize);
  assert(length == 0 || isInputPrefixable());
  assert(length < bufferSize);
  buffer += length;
  bufferSize -= length;
  assert(bufferSize >= 0);
  length += std::min<int>(
      strlcpy(buffer, m_rootSymbols.mainAlias(), bufferSize), bufferSize - 1);
  return length;
}

#endif

bool UnitRepresentative::canParseWithEquivalents(
    const char* symbol, size_t length,
    const UnitRepresentative** representative,
    const UnitPrefix** prefix) const {
  const UnitRepresentative* candidate = representativesOfSameDimension();
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

bool UnitRepresentative::canParse(const char* symbol, size_t length,
                                  const UnitPrefix** prefix) const {
  if (!isInputPrefixable()) {
    if (prefix) {
      *prefix = UnitPrefix::EmptyPrefix();
    }
    return length == 0;
  }
  for (size_t i = 0; i < UnitPrefix::k_numberOfPrefixes; i++) {
    const UnitPrefix* pre = UnitPrefix::Prefixes() + i;
    const char* prefixSymbol = pre->symbol();
    if (strlen(prefixSymbol) == length && canPrefix(pre, true) &&
        strncmp(symbol, prefixSymbol, length) == 0) {
      if (prefix) {
        *prefix = pre;
      }
      return true;
    }
  }
  return false;
}

bool UnitRepresentative::canPrefix(const UnitPrefix* prefix, bool input) const {
  Prefixable prefixable = (input) ? m_inputPrefixable : m_outputPrefixable;
  if (prefix->exponent() == 0) {
    return true;
  }
  if (prefixable == Prefixable::None) {
    return false;
  }
  if (prefixable == Prefixable::All) {
    return true;
  }
  if (prefixable == Prefixable::LongScale) {
    return prefix->exponent() % 3 == 0;
  }
  if (prefixable == Prefixable::NegativeAndKilo) {
    return prefix->exponent() < 0 || prefix->exponent() == 3;
  }
  if (prefixable == Prefixable::NegativeLongScale) {
    return prefix->exponent() < 0 && prefix->exponent() % 3 == 0;
  }
  if (prefixable == Prefixable::PositiveLongScale) {
    return prefix->exponent() > 0 && prefix->exponent() % 3 == 0;
  }
  if (prefixable == Prefixable::Negative) {
    return prefix->exponent() < 0;
  }
  if (prefixable == Prefixable::Positive) {
    return prefix->exponent() > 0;
  }
  assert(false);
  return false;
}

#if 0
const UnitPrefix* UnitRepresentative::findBestPrefix(double value,
                                             double exponent) const {
  if (!isOutputPrefixable()) {
    return UnitPrefix::EmptyPrefix();
  }
  if (value < Float<double>::EpsilonLax()) {
    return basePrefix();
  }
  const UnitPrefix* res = basePrefix();
  const float magnitude = std::log10(std::fabs(value));
  float bestOrder = magnitude;
  for (int i = 0; i < UnitPrefix::k_numberOfPrefixes; i++) {
    if (!canPrefix(UnitPrefix::Prefixes() + i, false)) {
      continue;
    }
    float order = magnitude - (UnitPrefix::Prefixes()[i].exponent() -
                               basePrefix()->exponent()) *
                                  exponent;
    if (compareMagnitudeOrders(order, bestOrder)) {
      bestOrder = order;
      res = UnitPrefix::Prefixes() + i;
    }
  }
  return res;
}
#endif

// ___Representative
int TimeRepresentative::numberOfRepresentatives() const {
  return std::size(Unit::k_timeRepresentatives);
}
const UnitRepresentative* TimeRepresentative::representativesOfSameDimension()
    const {
  return Unit::k_timeRepresentatives;
}

int DistanceRepresentative::numberOfRepresentatives() const {
  return std::size(Unit::k_distanceRepresentatives);
}
const UnitRepresentative*
DistanceRepresentative::representativesOfSameDimension() const {
  return Unit::k_distanceRepresentatives;
}

int AngleRepresentative::numberOfRepresentatives() const {
  return std::size(Unit::k_angleRepresentatives);
}
const UnitRepresentative* AngleRepresentative::representativesOfSameDimension()
    const {
  return Unit::k_angleRepresentatives;
}

int MassRepresentative::numberOfRepresentatives() const {
  return std::size(Unit::k_massRepresentatives);
}
const UnitRepresentative* MassRepresentative::representativesOfSameDimension()
    const {
  return Unit::k_massRepresentatives;
}

int CurrentRepresentative::numberOfRepresentatives() const {
  return std::size(Unit::k_currentRepresentatives);
}
const UnitRepresentative*
CurrentRepresentative::representativesOfSameDimension() const {
  return Unit::k_currentRepresentatives;
}

int TemperatureRepresentative::numberOfRepresentatives() const {
  return std::size(Unit::k_temperatureRepresentatives);
}
const UnitRepresentative*
TemperatureRepresentative::representativesOfSameDimension() const {
  return Unit::k_temperatureRepresentatives;
}

int AmountOfSubstanceRepresentative::numberOfRepresentatives() const {
  return std::size(Unit::k_amountOfSubstanceRepresentatives);
}
const UnitRepresentative*
AmountOfSubstanceRepresentative::representativesOfSameDimension() const {
  return Unit::k_amountOfSubstanceRepresentatives;
}

int LuminousIntensityRepresentative::numberOfRepresentatives() const {
  return std::size(Unit::k_luminousIntensityRepresentatives);
}
const UnitRepresentative*
LuminousIntensityRepresentative::representativesOfSameDimension() const {
  return Unit::k_luminousIntensityRepresentatives;
}

int FrequencyRepresentative::numberOfRepresentatives() const {
  return std::size(Unit::k_frequencyRepresentatives);
}
const UnitRepresentative*
FrequencyRepresentative::representativesOfSameDimension() const {
  return Unit::k_frequencyRepresentatives;
}

int ForceRepresentative::numberOfRepresentatives() const {
  return std::size(Unit::k_forceRepresentatives);
}
const UnitRepresentative* ForceRepresentative::representativesOfSameDimension()
    const {
  return Unit::k_forceRepresentatives;
}

int PressureRepresentative::numberOfRepresentatives() const {
  return std::size(Unit::k_pressureRepresentatives);
}
const UnitRepresentative*
PressureRepresentative::representativesOfSameDimension() const {
  return Unit::k_pressureRepresentatives;
}

int EnergyRepresentative::numberOfRepresentatives() const {
  return std::size(Unit::k_energyRepresentatives);
}
const UnitRepresentative* EnergyRepresentative::representativesOfSameDimension()
    const {
  return Unit::k_energyRepresentatives;
}

int PowerRepresentative::numberOfRepresentatives() const {
  return std::size(Unit::k_powerRepresentatives);
}
const UnitRepresentative* PowerRepresentative::representativesOfSameDimension()
    const {
  return Unit::k_powerRepresentatives;
}

int ElectricChargeRepresentative::numberOfRepresentatives() const {
  return std::size(Unit::k_electricChargeRepresentatives);
}
const UnitRepresentative*
ElectricChargeRepresentative::representativesOfSameDimension() const {
  return Unit::k_electricChargeRepresentatives;
}

int ElectricPotentialRepresentative::numberOfRepresentatives() const {
  return std::size(Unit::k_electricPotentialRepresentatives);
}
const UnitRepresentative*
ElectricPotentialRepresentative::representativesOfSameDimension() const {
  return Unit::k_electricPotentialRepresentatives;
}

int ElectricCapacitanceRepresentative::numberOfRepresentatives() const {
  return std::size(Unit::k_electricCapacitanceRepresentatives);
}
const UnitRepresentative*
ElectricCapacitanceRepresentative::representativesOfSameDimension() const {
  return Unit::k_electricCapacitanceRepresentatives;
}

int ElectricResistanceRepresentative::numberOfRepresentatives() const {
  return std::size(Unit::k_electricResistanceRepresentatives);
}
const UnitRepresentative*
ElectricResistanceRepresentative::representativesOfSameDimension() const {
  return Unit::k_electricResistanceRepresentatives;
}

int ElectricConductanceRepresentative::numberOfRepresentatives() const {
  return std::size(Unit::k_electricConductanceRepresentatives);
}
const UnitRepresentative*
ElectricConductanceRepresentative::representativesOfSameDimension() const {
  return Unit::k_electricConductanceRepresentatives;
}

int MagneticFluxRepresentative::numberOfRepresentatives() const {
  return std::size(Unit::k_magneticFluxRepresentatives);
}
const UnitRepresentative*
MagneticFluxRepresentative::representativesOfSameDimension() const {
  return Unit::k_magneticFluxRepresentatives;
}

int MagneticFieldRepresentative::numberOfRepresentatives() const {
  return std::size(Unit::k_magneticFieldRepresentatives);
}
const UnitRepresentative*
MagneticFieldRepresentative::representativesOfSameDimension() const {
  return Unit::k_magneticFieldRepresentatives;
}

int InductanceRepresentative::numberOfRepresentatives() const {
  return std::size(Unit::k_inductanceRepresentatives);
}
const UnitRepresentative*
InductanceRepresentative::representativesOfSameDimension() const {
  return Unit::k_inductanceRepresentatives;
}

int CatalyticActivityRepresentative::numberOfRepresentatives() const {
  return std::size(Unit::k_catalyticActivityRepresentatives);
}
const UnitRepresentative*
CatalyticActivityRepresentative::representativesOfSameDimension() const {
  return Unit::k_catalyticActivityRepresentatives;
}

int SurfaceRepresentative::numberOfRepresentatives() const {
  return std::size(Unit::k_surfaceRepresentatives);
}
const UnitRepresentative*
SurfaceRepresentative::representativesOfSameDimension() const {
  return Unit::k_surfaceRepresentatives;
}

int VolumeRepresentative::numberOfRepresentatives() const {
  return std::size(Unit::k_volumeRepresentatives);
}
const UnitRepresentative* VolumeRepresentative::representativesOfSameDimension()
    const {
  return Unit::k_volumeRepresentatives;
}

#if 0
int TimeRepresentative::setAdditionalExpressions(
    double value, Expression* dest, int availableLength,
    const ReductionContext& reductionContext) const {
  assert(availableLength >= 1);
  /* Use all representatives but week */
  const Unit splitUnits[] = {
      Unit::Builder(
          representativesOfSameDimension() + Unit::k_secondRepresentativeIndex,
          UnitPrefix::EmptyPrefix()),
      Unit::Builder(
          representativesOfSameDimension() + Unit::k_minuteRepresentativeIndex,
          UnitPrefix::EmptyPrefix()),
      Unit::Builder(
          representativesOfSameDimension() + Unit::k_hourRepresentativeIndex,
          UnitPrefix::EmptyPrefix()),
      Unit::Builder(
          representativesOfSameDimension() + Unit::k_dayRepresentativeIndex,
          UnitPrefix::EmptyPrefix()),
      Unit::Builder(
          representativesOfSameDimension() + Unit::k_monthRepresentativeIndex,
          UnitPrefix::EmptyPrefix()),
      Unit::Builder(
          representativesOfSameDimension() + Unit::k_yearRepresentativeIndex,
          UnitPrefix::EmptyPrefix()),
  };
  dest[0] = Unit::BuildSplit(value, splitUnits, numberOfRepresentatives() - 1,
                             reductionContext);
  return 1;
}

const UnitRepresentative* DistanceRepresentative::standardRepresentative(
    double value, double exponent, const ReductionContext& reductionContext,
    const UnitPrefix** prefix) const {
  return (reductionContext.unitFormat() == Preferences::UnitFormat::Metric)
             ?
             /* Exclude imperial units from the search. */
             defaultFindBestRepresentative(
                 value, exponent, representativesOfSameDimension(),
                 Unit::k_inchRepresentativeIndex, prefix)
             :
             /* Exclude m form the search. */
             defaultFindBestRepresentative(
                 value, exponent, representativesOfSameDimension() + 1,
                 numberOfRepresentatives() - 1, prefix);
}

int DistanceRepresentative::setAdditionalExpressions(
    double value, Expression* dest, int availableLength,
    const ReductionContext& reductionContext) const {
  assert(availableLength >= 1);
  if (reductionContext.unitFormat() == Preferences::UnitFormat::Metric) {
    return 0;
  }
  const Unit splitUnits[] = {
      Unit::Builder(
          representativesOfSameDimension() + Unit::k_inchRepresentativeIndex,
          UnitPrefix::EmptyPrefix()),
      Unit::Builder(
          representativesOfSameDimension() + Unit::k_footRepresentativeIndex,
          UnitPrefix::EmptyPrefix()),
      Unit::Builder(
          representativesOfSameDimension() + Unit::k_yardRepresentativeIndex,
          UnitPrefix::EmptyPrefix()),
      Unit::Builder(
          representativesOfSameDimension() + Unit::k_mileRepresentativeIndex,
          UnitPrefix::EmptyPrefix()),
  };
  dest[0] = Unit::BuildSplit(value, splitUnits, std::size(splitUnits),
                             reductionContext);
  return 1;
}

#endif

const UnitRepresentative*
AngleRepresentative::DefaultRepresentativeForAngleUnit(AngleUnit angleUnit) {
  switch (angleUnit) {
    case AngleUnit::Degree:
      return Unit::k_angleRepresentatives + Unit::k_degreeRepresentativeIndex;
    case AngleUnit::Radian:
      return Unit::k_angleRepresentatives + Unit::k_radianRepresentativeIndex;
    default:
      assert(angleUnit == AngleUnit::Gradian);
      return Unit::k_angleRepresentatives + Unit::k_gradianRepresentativeIndex;
  }
}

#if 0
const UnitRepresentative* AngleRepresentative::standardRepresentative(
    double value, double exponent, const ReductionContext& reductionContext,
    const UnitPrefix** prefix) const {
  if (reductionContext.angleUnit() == AngleUnit::Degree) {
    return defaultFindBestRepresentative(
        value, exponent,
        representativesOfSameDimension() + Unit::k_arcSecondRepresentativeIndex,
        3, prefix);
  }
  return DefaultRepresentativeForAngleUnit(reductionContext.angleUnit());
}

Expression AngleRepresentative::convertInto(
    Expression value, const UnitRepresentative* other,
    const ReductionContext& reductionContext) const {
  assert(dimensionVector() == other->dimensionVector());
  Expression unit = Unit::Builder(other, UnitPrefix::EmptyPrefix());
  Expression inRadians =
      Multiplication::Builder(value, ratioExpressionReduced(reductionContext))
          .shallowReduce(reductionContext);
  Expression inOther =
      Division::Builder(inRadians,
                        other->ratioExpressionReduced(reductionContext))
          .shallowReduce(reductionContext)
          .deepBeautify(reductionContext);
  return Multiplication::Builder(inOther, unit);
}

int AngleRepresentative::setAdditionalExpressionsWithExactValue(
    Expression exactValue, double value, Expression* dest, int availableLength,
    const ReductionContext& reductionContext) const {
  assert(availableLength >= 2);
  int numberOfResults = 0;
  // Conversion to degrees should be added to all units not degree related
  if (this == representativesOfSameDimension() +
                  Unit::k_radianRepresentativeIndex ||
      this == representativesOfSameDimension() +
                  Unit::k_gradianRepresentativeIndex) {
    const UnitRepresentative* degree =
        representativesOfSameDimension() + Unit::k_degreeRepresentativeIndex;
    dest[numberOfResults++] =
        convertInto(exactValue.clone(), degree, reductionContext)
            .approximateKeepingUnits<double>(reductionContext);
  }
  // Degrees related units should show their decomposition in DMS
  const Unit splitUnits[] = {
      Unit::Builder(representativesOfSameDimension() +
                        Unit::k_arcSecondRepresentativeIndex,
                    UnitPrefix::EmptyPrefix()),
      Unit::Builder(representativesOfSameDimension() +
                        Unit::k_arcMinuteRepresentativeIndex,
                    UnitPrefix::EmptyPrefix()),
      Unit::Builder(
          representativesOfSameDimension() + Unit::k_degreeRepresentativeIndex,
          UnitPrefix::EmptyPrefix()),
  };
  Expression split = Unit::BuildSplit(value, splitUnits, std::size(splitUnits),
                                      reductionContext);
  if (!split.isUndefined()) {
    dest[numberOfResults++] = split;
  }
  // Conversion to radians should be added to all other units.
  if (this !=
      representativesOfSameDimension() + Unit::k_radianRepresentativeIndex) {
    const UnitRepresentative* radian =
        representativesOfSameDimension() + Unit::k_radianRepresentativeIndex;
    dest[numberOfResults++] = convertInto(exactValue, radian, reductionContext);
  }
  return numberOfResults;
}
#endif

const UnitPrefix* MassRepresentative::basePrefix() const {
  return isBaseUnit() ? UnitPrefix::Prefixes() + Unit::k_kiloPrefixIndex
                      : UnitPrefix::EmptyPrefix();
}

#if 0
const UnitRepresentative* MassRepresentative::standardRepresentative(
    double value, double exponent, const ReductionContext& reductionContext,
    const UnitPrefix** prefix) const {
  if (reductionContext.unitFormat() == Preferences::UnitFormat::Imperial) {
    return defaultFindBestRepresentative(
        value, exponent,
        representativesOfSameDimension() + Unit::k_ounceRepresentativeIndex,
        Unit::k_shortTonRepresentativeIndex - Unit::k_ounceRepresentativeIndex +
            1,
        prefix);
  }
  assert(reductionContext.unitFormat() == Preferences::UnitFormat::Metric);
  bool useTon = exponent == 1. && value >= (representativesOfSameDimension() +
                                            Unit::k_tonRepresentativeIndex)
                                               ->ratio();
  int representativeIndex =
      useTon ? Unit::k_tonRepresentativeIndex : Unit::k_gramRepresentativeIndex;
  return defaultFindBestRepresentative(
      value, exponent, representativesOfSameDimension() + representativeIndex,
      1, prefix);
}

int MassRepresentative::setAdditionalExpressions(
    double value, Expression* dest, int availableLength,
    const ReductionContext& reductionContext) const {
  assert(availableLength >= 1);
  if (reductionContext.unitFormat() == Preferences::UnitFormat::Metric) {
    return 0;
  }
  const Unit splitUnits[] = {
      Unit::Builder(
          representativesOfSameDimension() + Unit::k_ounceRepresentativeIndex,
          UnitPrefix::EmptyPrefix()),
      Unit::Builder(
          representativesOfSameDimension() + Unit::k_poundRepresentativeIndex,
          UnitPrefix::EmptyPrefix()),
      Unit::Builder(representativesOfSameDimension() +
                        Unit::k_shortTonRepresentativeIndex,
                    UnitPrefix::EmptyPrefix()),
  };
  dest[0] = Unit::BuildSplit(value, splitUnits, std::size(splitUnits),
                             reductionContext);
  return 1;
}

double TemperatureRepresentative::ConvertTemperatures(
    double value, const UnitRepresentative* source,
    const UnitRepresentative* target) {
  assert(source->dimensionVector() ==
         TemperatureRepresentative::Default().dimensionVector());
  assert(target->dimensionVector() ==
         TemperatureRepresentative::Default().dimensionVector());
  if (source == target) {
    return value;
  }
  constexpr double origin[] = {0, k_celsiusOrigin, k_fahrenheitOrigin};
  assert(sizeof(origin) == source->numberOfRepresentatives() * sizeof(double));
  double sourceOrigin =
      origin[source - source->representativesOfSameDimension()];
  double targetOrigin =
      origin[target - target->representativesOfSameDimension()];
  /* (T + origin) * ration converts T to Kelvin.
   * T/ratio - origin converts T from Kelvin. */
  return (value + sourceOrigin) * source->ratio() / target->ratio() -
         targetOrigin;
}

int TemperatureRepresentative::setAdditionalExpressions(
    double value, Expression* dest, int availableLength,
    const ReductionContext& reductionContext) const {
  assert(availableLength >= 2);
  const UnitRepresentative* celsius =
      TemperatureRepresentative::Default().representativesOfSameDimension() +
      Unit::k_celsiusRepresentativeIndex;
  const UnitRepresentative* fahrenheit =
      TemperatureRepresentative::Default().representativesOfSameDimension() +
      Unit::k_fahrenheitRepresentativeIndex;
  const UnitRepresentative* kelvin =
      TemperatureRepresentative::Default().representativesOfSameDimension() +
      Unit::k_kelvinRepresentativeIndex;
  const UnitRepresentative* targets[] = {
      reductionContext.unitFormat() == Preferences::UnitFormat::Metric
          ? celsius
          : fahrenheit,
      reductionContext.unitFormat() == Preferences::UnitFormat::Metric
          ? fahrenheit
          : celsius,
      kelvin};
  int numberOfExpressionsSet = 0;
  constexpr int numberOfTargets = std::size(targets);
  for (int i = 0; i < numberOfTargets; i++) {
    if (targets[i] == this) {
      continue;
    }
    dest[numberOfExpressionsSet++] = Multiplication::Builder(
        Float<double>::Builder(TemperatureRepresentative::ConvertTemperatures(
            value, this, targets[i])),
        Unit::Builder(targets[i], UnitPrefix::EmptyPrefix()));
  }
  assert(numberOfExpressionsSet == 2);
  return numberOfExpressionsSet;
}

int EnergyRepresentative::setAdditionalExpressions(
    double value, Expression* dest, int availableLength,
    const ReductionContext& reductionContext) const {
  assert(availableLength >= 2);
  int index = 0;
  /* 1. Convert into Joules
   * As J is just a shorthand for _kg_m^2_s^-2, the value is used as is. */
  const UnitRepresentative* joule =
      representativesOfSameDimension() + Unit::k_jouleRepresentativeIndex;
  const UnitPrefix* joulePrefix = joule->findBestPrefix(value, 1.);
  dest[index++] = Multiplication::Builder(
      Float<double>::Builder(value * std::pow(10., -joulePrefix->exponent())),
      Unit::Builder(joule, joulePrefix));
  /* 2. Convert into Wh
   * As value is expressed in SI units (ie _kg_m^2_s^-2), the ratio is that of
   * hours to seconds. */
  const UnitRepresentative* hour =
      TimeRepresentative::Default().representativesOfSameDimension() +
      Unit::k_hourRepresentativeIndex;
  const UnitRepresentative* watt =
      PowerRepresentative::Default().representativesOfSameDimension() +
      Unit::k_wattRepresentativeIndex;
  double adjustedValue = value / hour->ratio() / watt->ratio();
  const UnitPrefix* wattPrefix = watt->findBestPrefix(adjustedValue, 1.);
  dest[index++] = Multiplication::Builder(
      Float<double>::Builder(adjustedValue *
                             std::pow(10., -wattPrefix->exponent())),
      Multiplication::Builder(Unit::Builder(watt, wattPrefix),
                              Unit::Builder(hour, UnitPrefix::EmptyPrefix())));
  /* 3. Convert into eV */
  const UnitRepresentative* eV = representativesOfSameDimension() +
                             Unit::k_electronVoltRepresentativeIndex;
  adjustedValue = value / eV->ratio();
  const UnitPrefix* eVPrefix = eV->findBestPrefix(adjustedValue, 1.);
  dest[index++] = Multiplication::Builder(
      Float<double>::Builder(adjustedValue *
                             std::pow(10., -eVPrefix->exponent())),
      Unit::Builder(eV, eVPrefix));
  return index;
}

const UnitRepresentative* SurfaceRepresentative::standardRepresentative(
    double value, double exponent, const ReductionContext& reductionContext,
    const UnitPrefix** prefix) const {
  *prefix = UnitPrefix::EmptyPrefix();
  return representativesOfSameDimension() +
         (reductionContext.unitFormat() == Preferences::UnitFormat::Metric
              ? Unit::k_hectareRepresentativeIndex
              : Unit::k_acreRepresentativeIndex);
}

int SurfaceRepresentative::setAdditionalExpressions(
    double value, Expression* dest, int availableLength,
    const ReductionContext& reductionContext) const {
  assert(availableLength >= 2);
  Expression* destMetric;
  Expression* destImperial = nullptr;
  if (reductionContext.unitFormat() == Preferences::UnitFormat::Metric) {
    destMetric = dest;
  } else {
    destImperial = dest;
    destMetric = dest + 1;
  }
  // 1. Convert to hectares
  const UnitRepresentative* hectare =
      representativesOfSameDimension() + Unit::k_hectareRepresentativeIndex;
  *destMetric =
      Multiplication::Builder(Float<double>::Builder(value / hectare->ratio()),
                              Unit::Builder(hectare, UnitPrefix::EmptyPrefix()));
  // 2. Convert to acres
  if (!destImperial) {
    return 1;
  }
  const UnitRepresentative* acre =
      representativesOfSameDimension() + Unit::k_acreRepresentativeIndex;
  *destImperial =
      Multiplication::Builder(Float<double>::Builder(value / acre->ratio()),
                              Unit::Builder(acre, UnitPrefix::EmptyPrefix()));
  return 2;
}

const UnitRepresentative* VolumeRepresentative::standardRepresentative(
    double value, double exponent, const ReductionContext& reductionContext,
    const UnitPrefix** prefix) const {
  if (reductionContext.unitFormat() == Preferences::UnitFormat::Metric) {
    *prefix = representativesOfSameDimension()->findBestPrefix(value, exponent);
    return representativesOfSameDimension();
  }
  return defaultFindBestRepresentative(value, exponent,
                                       representativesOfSameDimension() + 1,
                                       numberOfRepresentatives() - 1, prefix);
}

int VolumeRepresentative::setAdditionalExpressions(
    double value, Expression* dest, int availableLength,
    const ReductionContext& reductionContext) const {
  assert(availableLength >= 2);
  Expression* destMetric;
  Expression* destImperial = nullptr;
  if (reductionContext.unitFormat() == Preferences::UnitFormat::Metric) {
    destMetric = dest;
  } else {
    destImperial = dest;
    destMetric = dest + 1;
  }
  // 1. Convert to liters
  const UnitRepresentative* liter =
      representativesOfSameDimension() + Unit::k_literRepresentativeIndex;
  double adjustedValue = value / liter->ratio();
  const UnitPrefix* literPrefix = liter->findBestPrefix(adjustedValue, 1.);
  *destMetric = Multiplication::Builder(
      Float<double>::Builder(adjustedValue *
                             pow(10., -literPrefix->exponent())),
      Unit::Builder(liter, literPrefix));
  // 2. Convert to imperial volumes
  if (!destImperial) {
    return 1;
  }
  const Unit splitUnits[] = {
      Unit::Builder(
          representativesOfSameDimension() + Unit::k_cupRepresentativeIndex,
          UnitPrefix::EmptyPrefix()),
      Unit::Builder(
          representativesOfSameDimension() + Unit::k_pintRepresentativeIndex,
          UnitPrefix::EmptyPrefix()),
      Unit::Builder(
          representativesOfSameDimension() + Unit::k_quartRepresentativeIndex,
          UnitPrefix::EmptyPrefix()),
      Unit::Builder(
          representativesOfSameDimension() + Unit::k_gallonRepresentativeIndex,
          UnitPrefix::EmptyPrefix()),
  };
  *destImperial = Unit::BuildSplit(value, splitUnits, std::size(splitUnits),
                                   reductionContext);
  return 2;
}

int SpeedRepresentative::setAdditionalExpressions(
    double value, Expression* dest, int availableLength,
    const ReductionContext& reductionContext) const {
  assert(availableLength >= 2);
  Expression* destMetric;
  Expression* destImperial = nullptr;
  if (reductionContext.unitFormat() == Preferences::UnitFormat::Metric) {
    destMetric = dest;
  } else {
    destImperial = dest;
    destMetric = dest + 1;
  }
  // 1. Convert to km/h
  const UnitRepresentative* meter =
      DistanceRepresentative::Default().representativesOfSameDimension() +
      Unit::k_meterRepresentativeIndex;
  const UnitRepresentative* hour =
      TimeRepresentative::Default().representativesOfSameDimension() +
      Unit::k_hourRepresentativeIndex;
  *destMetric = Multiplication::Builder(
      Float<double>::Builder(value / 1000. * hour->ratio()),
      Multiplication::Builder(
          Unit::Builder(meter, UnitPrefix::Prefixes() + Unit::k_kiloPrefixIndex),
          Power::Builder(Unit::Builder(hour, UnitPrefix::EmptyPrefix()),
                         Rational::Builder(-1))));
  // 2. Convert to mph
  if (!destImperial) {
    return 1;
  }
  const UnitRepresentative* mile =
      DistanceRepresentative::Default().representativesOfSameDimension() +
      Unit::k_mileRepresentativeIndex;
  *destImperial = Multiplication::Builder(
      Float<double>::Builder(value / mile->ratio() * hour->ratio()),
      Multiplication::Builder(
          Unit::Builder(mile, UnitPrefix::EmptyPrefix()),
          Power::Builder(Unit::Builder(hour, UnitPrefix::EmptyPrefix()),
                         Rational::Builder(-1))));
  return 2;
}

// UnitNode
Expression removeUnit(Expression* unit) { return Unit(this).removeUnit(unit); }

Layout createLayout(Preferences::PrintFloatMode floatDisplayMode,
                    int numberOfSignificantDigits, Context* context) const {
  /* TODO: compute the bufferSize more precisely... So far the longest unit is
   * "month" of size 6 but later, we might add unicode to represent ohm or µ
   * which would change the required size?*/
  constexpr static size_t bufferSize = 10;
  char buffer[bufferSize];
  char* string = buffer;
  int stringLen = serialize(buffer, bufferSize, floatDisplayMode,
                            numberOfSignificantDigits);
  if (!context ||
      (context->canRemoveUnderscoreToUnits() &&
       context->expressionTypeForIdentifier(buffer + 1, strlen(buffer) - 1) ==
           Context::SymbolAbstractType::None)) {
    // If the unit is not a defined variable, do not display the '_'
    assert(string[0] == '_');
    string++;
    stringLen--;
  }
  return LayoutHelper::StringToStringLayout(string, stringLen);
}

int serialize(char* buffer, int bufferSize,
              Preferences::PrintFloatMode floatDisplayMode,
              int numberOfSignificantDigits) const {
  assert(bufferSize >= 0);
  int underscoreLength =
      std::min<int>(strlcpy(buffer, "_", bufferSize), bufferSize - 1);
  buffer += underscoreLength;
  bufferSize -= underscoreLength;
  return underscoreLength +
         m_representative->serialize(buffer, bufferSize, m_prefix);
}

int simplificationOrderSameType(const ExpressionNode* e, bool ascending,
                                bool ignoreParentheses) const {
  if (!ascending) {
    return e->simplificationOrderSameType(this, true, ignoreParentheses);
  }
  assert(type() == e->type());
  const UnitNode* eNode = static_cast<const UnitNode*>(e);
  DimensionVector v = representative()->dimensionVector();
  DimensionVector w = eNode->representative()->dimensionVector();
  for (int i = 0; i < k_numberOfBaseUnits; i++) {
    if (v.coefficientAtIndex(i) != w.coefficientAtIndex(i)) {
      return v.coefficientAtIndex(i) - w.coefficientAtIndex(i);
    }
  }
  const ptrdiff_t representativeDiff =
      m_representative - eNode->representative();
  if (representativeDiff != 0) {
    return representativeDiff;
  }
  const ptrdiff_t prediff = eNode->prefix()->exponent() - m_prefix->exponent();
  return prediff;
}

Expression shallowReduce(const ReductionContext& reductionContext) {
  return Unit(this).shallowReduce(reductionContext);
}

Expression shallowBeautify(const ReductionContext& reductionContext) {
  return Unit(this).shallowBeautify();
}

template <typename T>
Evaluation<T> templatedApproximate(
    const ApproximationContext& approximationContext) const {
  return Complex<T>::Undefined();
}
#endif

// Unit
bool Unit::CanParse(UnicodeDecoder* name,
                    const UnitRepresentative** representative,
                    const UnitPrefix** prefix) {
  if (name->nextCodePoint() != '_') {
    name->previousCodePoint();
  }
  // TODO: Better use of UnicodeDecoder. Here we assume units cannot be longer.
  constexpr static size_t bufferSize = 10;
  char symbol[bufferSize];
  size_t length = name->printInBuffer(symbol, bufferSize);
  assert(length < bufferSize);
  size_t offset = (symbol[0] == '_') ? 1 : 0;
  assert(length > offset && symbol[offset] != '_');
  for (int i = 0; i < UnitRepresentative::k_numberOfDimensions; i++) {
    if (UnitRepresentative::DefaultRepresentatives()[i]
            ->canParseWithEquivalents(symbol + offset, length - offset,
                                      representative, prefix)) {
      return true;
    }
  }
  return false;
}

#if 0
static void chooseBestRepresentativeAndPrefixForValueOnSingleUnit(
    Expression unit, double* value, const ReductionContext& reductionContext,
    bool optimizePrefix) {
  double exponent = 1.f;
  Expression factor = unit;
  if (factor.type() == ExpressionNode::Type::Power) {
    Expression childExponent = factor.childAtIndex(1);
    assert(factor.childAtIndex(0).type() == ExpressionNode::Type::Unit);
    assert(factor.childAtIndex(1).type() == ExpressionNode::Type::Rational);
    exponent =
        static_cast<Rational&>(childExponent)
            .approximateToScalar<double>(reductionContext.context(),
                                         reductionContext.complexFormat(),
                                         reductionContext.angleUnit());
    factor = factor.childAtIndex(0);
  }
  assert(factor.type() == ExpressionNode::Type::Unit);
  if (exponent == 0.f) {
    /* Finding the best representative for a unit with exponent 0 doesn't
     * really make sense, and should only happen with a weak ReductionTarget
     * (such as in Graph app), that only rely on approximations. We keep the
     * unit unchanged as it will approximate to undef anyway. */
    return;
  }
  static_cast<Unit&>(factor).chooseBestRepresentativeAndPrefix(
      value, exponent, reductionContext, optimizePrefix);
}

void Unit::ChooseBestRepresentativeAndPrefixForValue(
    Expression units, double* value, const ReductionContext& reductionContext) {
  int numberOfFactors;
  Expression factor;
  if (units.type() == ExpressionNode::Type::Multiplication) {
    numberOfFactors = units.numberOfChildren();
    factor = units.childAtIndex(0);
  } else {
    numberOfFactors = 1;
    factor = units;
  }
  chooseBestRepresentativeAndPrefixForValueOnSingleUnit(factor, value,
                                                        reductionContext, true);
  for (int i = 1; i < numberOfFactors; i++) {
    chooseBestRepresentativeAndPrefixForValueOnSingleUnit(
        units.childAtIndex(i), value, reductionContext, false);
  }
}

bool Unit::ShouldDisplayAdditionalOutputs(double value, Expression unit,
                                          Preferences::UnitFormat unitFormat) {
  if (unit.isUninitialized() || !std::isfinite(value)) {
    return false;
  }
  DimensionVector vector = DimensionVector::FromBaseUnits(unit);
  const UnitRepresentative* representative =
      UnitRepresentative::RepresentativeForDimension(vector);

  ExpressionTest isNonBase = [](const Expression e, Context* context) {
    return !e.isUninitialized() && e.type() == ExpressionNode::Type::Unit &&
           !e.convert<Unit>().isBaseUnit();
  };

  return (representative != nullptr &&
          representative->hasSpecialAdditionalExpressions(value, unitFormat)) ||
         unit.recursivelyMatches(isNonBase);
}

int Unit::SetAdditionalExpressions(Expression units, double value,
                                   Expression* dest, int availableLength,
                                   const ReductionContext& reductionContext,
                                   Expression exactOutput) {
  if (units.isUninitialized()) {
    return 0;
  }
  const UnitRepresentative* representative =
      units.type() == ExpressionNode::Type::Unit
          ? static_cast<Unit&>(units).node()->representative()
          : UnitRepresentative::RepresentativeForDimension(
                DimensionVector::FromBaseUnits(units));
  if (!representative) {
    return 0;
  }
  if (representative->dimensionVector() ==
      AngleRepresentative::Default().dimensionVector()) {
    /* Angles are the only unit where we want to display the exact value. */
    Expression exactValue = exactOutput.clone();
    Expression unit;
    ReductionContext childContext = reductionContext;
    childContext.setUnitConversion(UnitConversion::None);
    exactValue = exactValue.reduceAndRemoveUnit(childContext, &unit);
    assert(unit.type() == ExpressionNode::Type::Unit);
    return static_cast<const AngleRepresentative*>(
               static_cast<Unit&>(unit).representative())
        ->setAdditionalExpressionsWithExactValue(
            exactValue, value, dest, availableLength, reductionContext);
  }
  return representative->setAdditionalExpressions(value, dest, availableLength,
                                                  reductionContext);
}

Expression Unit::BuildSplit(double value, const Unit* units, int length,
                            const ReductionContext& reductionContext) {
  assert(!std::isnan(value));
  assert(units);
  assert(length > 0);

  double baseRatio = units->node()->representative()->ratio();
  double basedValue = value / baseRatio;
  // WARNING: Maybe this should be compared to 0.0 instead of EpsilonLax ? (see
  // below)
  if (std::isinf(basedValue) ||
      std::fabs(basedValue) < Float<double>::EpsilonLax()) {
    return Multiplication::Builder(Number::FloatNumber(basedValue), units[0]);
  }
  double err =
      std::pow(10.0, Poincare::PrintFloat::k_numberOfStoredSignificantDigits -
                         1 - std::ceil(log10(std::fabs(basedValue))));
  double remain = std::round(basedValue * err) / err;

  Addition res = Addition::Builder();
  for (int i = length - 1; i >= 0; i--) {
    assert(units[i].node()->prefix() == UnitPrefix::EmptyPrefix());
    double factor =
        std::round(units[i].node()->representative()->ratio() / baseRatio);
    double share = remain / factor;
    if (i > 0) {
      share = (share > 0.0) ? std::floor(share) : std::ceil(share);
    }
    remain -= share * factor;
    /* WARNING: Maybe this should be compared to 0.0 instead of EpsilonLax ?
     *  What happens if basedValue >= EpsilonLax but share < EpsilonLax for
     *  each unit of the split ?
     *  Right now it's not a problem because there is no unit with a ratio
     *  inferior to EpsilonLax but it might be if femto prefix is implemented.
     *  (EpsilonLax = 10^-15 = femto)*/
    if (std::abs(share) >= Float<double>::EpsilonLax()) {
      res.addChildAtIndexInPlace(
          Multiplication::Builder(Float<double>::Builder(share), units[i]),
          res.numberOfChildren(), res.numberOfChildren());
    }
  }
  assert(res.numberOfChildren() > 0);
  ReductionContext keepUnitsContext(
      reductionContext.context(), reductionContext.complexFormat(),
      reductionContext.angleUnit(), reductionContext.unitFormat(),
      ReductionTarget::User,
      SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition,
      UnitConversion::None);
  return res.squashUnaryHierarchyInPlace().shallowBeautify(keepUnitsContext);
}

Expression Unit::ConvertTemperatureUnits(
    Expression e, Unit unit, const ReductionContext& reductionContext) {
  const UnitRepresentative* targetRepr = unit.representative();
  const UnitPrefix* targetPrefix = unit.node()->prefix();
  assert(unit.representative()->dimensionVector() ==
         TemperatureRepresentative::Default().dimensionVector());

  Expression startUnit;
  e = e.removeUnit(&startUnit);
  if (startUnit.isUninitialized() ||
      startUnit.type() != ExpressionNode::Type::Unit) {
    return Undefined::Builder();
  }
  const UnitRepresentative* startRepr =
      static_cast<Unit&>(startUnit).representative();
  if (startRepr->dimensionVector() !=
      TemperatureRepresentative::Default().dimensionVector()) {
    return Undefined::Builder();
  }

  const UnitPrefix* startPrefix = static_cast<Unit&>(startUnit).node()->prefix();
  double value = e.approximateToScalar<double>(reductionContext.context(),
                                               reductionContext.complexFormat(),
                                               reductionContext.angleUnit());
  return Multiplication::Builder(
      Float<double>::Builder(TemperatureRepresentative::ConvertTemperatures(
                                 value * std::pow(10., startPrefix->exponent()),
                                 startRepr, targetRepr) *
                             std::pow(10., -targetPrefix->exponent())),
      unit.clone());
}

#endif

bool Unit::AllowImplicitAddition(
    const UnitRepresentative* smallestRepresentative,
    const UnitRepresentative* biggestRepresentative) {
  if (smallestRepresentative == biggestRepresentative) {
    return false;
  }
  for (int i = 0; i < k_representativesAllowingImplicitAdditionLength; i++) {
    bool foundFirstRepresentative = false;
    for (int j = 0; j < k_representativesAllowingImplicitAddition[i].length;
         j++) {
      if (smallestRepresentative ==
          k_representativesAllowingImplicitAddition[i].representativesList[j]) {
        foundFirstRepresentative = true;
      } else if (biggestRepresentative ==
                 k_representativesAllowingImplicitAddition[i]
                     .representativesList[j]) {
        if (foundFirstRepresentative) {
          // Both representatives were found, in order.
          return true;
        }
        return false;  // Not in right order
      }
    }
    if (foundFirstRepresentative) {
      return false;  // Only one representative was found.
    }
  }
  return false;
}

#if 0
bool Unit::ForceMarginLeftOfUnit(const Unit& unit) {
  const UnitRepresentative* representative = unit.representative();
  for (int i = 0; i < k_numberOfRepresentativesWithoutLeftMargin; i++) {
    if (k_representativesWithoutLeftMargin[i] == representative) {
      return false;
    }
  }
  return true;
}

Expression Unit::shallowReduce(ReductionContext reductionContext) {
  if (reductionContext.unitConversion() == UnitConversion::None ||
      isBaseUnit()) {
    /* We escape early if we are one of the seven base units.
     * Nb : For masses, k is considered the base prefix, so kg will be escaped
     * here but not g */
    return *this;
  }

  /* Handle temperatures : Celsius and Fahrenheit should not be used in
   * calculations, only in conversions and results.
   * These are the seven legal forms for writing non-kelvin temperatures :
   * (1)  _°C
   * (2)  _°C->_?
   * (3)  123_°C
   * (4)  -123_°C
   * (5)  123_°C->_K
   * (6)  -123_°C->_K
   * (7)  Right member of a unit convert - this is handled above, as
   *      UnitConversion is set to None in this case. */
  if (node()->representative()->dimensionVector() ==
          TemperatureRepresentative::Default().dimensionVector() &&
      node()->representative() !=
          k_temperatureRepresentatives + k_kelvinRepresentativeIndex) {
    Expression p = parent();
    if (p.isUninitialized() || p.type() == ExpressionNode::Type::UnitConvert ||
        p.type() == ExpressionNode::Type::Store ||
        (p.type() == ExpressionNode::Type::Multiplication &&
         p.numberOfChildren() == 2) ||
        p.type() == ExpressionNode::Type::Opposite) {
      /* If the parent is a UnitConvert, the temperature is always legal.
       * Otherwise, we need to wait until the reduction of the multiplication
       * to fully detect forbidden forms. */
      return *this;
    }
    return replaceWithUndefinedInPlace();
  }

  UnitNode* unitNode = node();
  const UnitRepresentative* representative = unitNode->representative();
  const UnitPrefix* prefix = unitNode->prefix();

  Expression result = representative->toBaseUnits(reductionContext)
                          .deepReduce(reductionContext);
  if (prefix != UnitPrefix::EmptyPrefix()) {
    Expression prefixFactor = Power::Builder(
        Rational::Builder(10), Rational::Builder(prefix->exponent()));
    prefixFactor = prefixFactor.shallowReduce(reductionContext);
    result = Multiplication::Builder(prefixFactor, result)
                 .shallowReduce(reductionContext);
  }
  replaceWithInPlace(result);
  return result;
}

Expression Unit::shallowBeautify() {
  // Force Float(1) in front of an orphan Unit
  if (parent().isUninitialized() ||
      parent().type() == ExpressionNode::Type::Opposite) {
    Multiplication m = Multiplication::Builder(Float<double>::Builder(1.));
    replaceWithInPlace(m);
    m.addChildAtIndexInPlace(*this, 1, 1);
    return std::move(m);
  }
  return *this;
}

Expression Unit::removeUnit(Expression* unit) {
  *unit = *this;
  Expression one = Rational::Builder(1);
  replaceWithInPlace(one);
  return one;
}

void Unit::chooseBestRepresentativeAndPrefix(
    double* value, double exponent, const ReductionContext& reductionContext,
    bool optimizePrefix) {
  assert(exponent != 0.f);

  if ((std::isinf(*value) ||
       (*value == 0.0 &&
        node()->representative()->dimensionVector() !=
            TemperatureRepresentative::Default().dimensionVector()))) {
    /* Use the base unit to represent an infinite or null value, as all units
     * are equivalent.
     * This is not true for temperatures (0 K != 0°C != 0°F). */
    node()->setRepresentative(
        node()->representative()->representativesOfSameDimension());
    node()->setPrefix(node()->representative()->basePrefix());
    return;
  }
  // Convert value to base units
  double baseValue =
      *value *
      std::pow(
          node()->representative()->ratio() *
              std::pow(10.,
                       node()->prefix()->exponent() -
                           node()->representative()->basePrefix()->exponent()),
          exponent);
  const UnitPrefix* bestPrefix = (optimizePrefix) ? UnitPrefix::EmptyPrefix() : nullptr;
  const UnitRepresentative* bestRepresentative =
      node()->representative()->standardRepresentative(
          baseValue, exponent, reductionContext, &bestPrefix);
  if (!optimizePrefix) {
    bestPrefix = bestRepresentative->basePrefix();
  }

  if (bestRepresentative != node()->representative()) {
    *value =
        *value *
        std::pow(
            node()->representative()->ratio() / bestRepresentative->ratio() *
                std::pow(
                    10.,
                    bestRepresentative->basePrefix()->exponent() -
                        node()->representative()->basePrefix()->exponent()),
            exponent);
    node()->setRepresentative(bestRepresentative);
  }
  if (bestPrefix != node()->prefix()) {
    *value = *value * std::pow(10., exponent * (node()->prefix()->exponent() -
                                                bestPrefix->exponent()));
    node()->setPrefix(bestPrefix);
  }
}
#endif

void Unit::RemoveUnit(Tree* unit) {
  Tree* result = SharedEditionPool->push<BlockType::Multiplication>(2);
  GetRepresentative(unit)->ratioExpressionReduced()->clone();
  SharedEditionPool->push<BlockType::Power>();
  IntegerHandler(10).pushOnEditionPool();
  IntegerHandler(GetPrefix(unit)->exponent()).pushOnEditionPool();
  unit->moveTreeOverTree(result);
}

Tree* Unit::Push(const UnitRepresentative* unitRepresentative,
                 const UnitPrefix* unitPrefix) {
  uint8_t repId = UnitRepresentative::ToId(unitRepresentative);
  uint8_t preId = UnitPrefix::ToId(unitPrefix);
  return SharedEditionPool->push<BlockType::Unit>(repId, preId);
}

const UnitRepresentative* Unit::GetRepresentative(const Tree* unit) {
  return UnitRepresentative::FromId(
      static_cast<uint8_t>(*unit->block()->next()));
}

const UnitPrefix* Unit::GetPrefix(const Tree* unit) {
  return UnitPrefix::FromId(
      static_cast<uint8_t>(*unit->block()->next()->next()));
}

}  // namespace PoincareJ
