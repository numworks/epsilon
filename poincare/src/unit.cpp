#include <assert.h>
#include <limits.h>
#include <omg/round.h>
#include <poincare/addition.h>
#include <poincare/division.h>
#include <poincare/float.h>
#include <poincare/layout.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>
#include <poincare/rational.h>
#include <poincare/undefined.h>
#include <poincare/unit.h>

#include <algorithm>
#include <array>
#include <utility>

namespace Poincare {

// UnitNode::Prefix
const UnitNode::Prefix* UnitNode::Prefix::Prefixes() {
  return OUnit::k_prefixes;
}

const UnitNode::Prefix* UnitNode::Prefix::EmptyPrefix() {
  return Prefixes() + OUnit::k_emptyPrefixIndex;
}

// UnitNode::Vector
size_t UnitNode::DimensionVector::supportSize() const {
  size_t supportSize = 0;
  for (int i = 0; i < k_numberOfBaseUnits; i++) {
    if (coefficientAtIndex(i) == 0) {
      continue;
    }
    supportSize++;
  }
  return supportSize;
}

void UnitNode::DimensionVector::addAllCoefficients(const DimensionVector other,
                                                   int factor) {
  for (int i = 0; i < UnitNode::k_numberOfBaseUnits; i++) {
    setCoefficientAtIndex(
        i, coefficientAtIndex(i) + other.coefficientAtIndex(i) * factor);
  }
}

UnitNode::DimensionVector UnitNode::DimensionVector::FromBaseUnits(
    const OExpression baseUnits, bool canIgnoreCoefficients) {
  /* Returns the vector of Base units with integer exponents. If rational, the
   * closest integer will be used. */
  DimensionVector nullVector = {
      .time = 0,
      .distance = 0,
      .angle = 0,
      .mass = 0,
      .current = 0,
      .temperature = 0,
      .amountOfSubstance = 0,
      .luminuousIntensity = 0,
  };
  DimensionVector vector = nullVector;
  int numberOfFactors;
  int factorIndex = 0;
  OExpression factor;
  if (baseUnits.otype() == ExpressionNode::Type::Multiplication) {
    numberOfFactors = baseUnits.numberOfChildren();
    factor = baseUnits.childAtIndex(0);
  } else {
    numberOfFactors = 1;
    factor = baseUnits;
  }
  do {
    // Get the unit's exponent
    int exponent = 1;
    if (factor.otype() == ExpressionNode::Type::Power) {
      OExpression exp = factor.childAtIndex(1);
      assert(exp.otype() == ExpressionNode::Type::Rational);
      if (!static_cast<Rational&>(exp).isInteger()) {
        /* If non-integer exponents are found, we return a null vector so that
         * Multiplication::shallowBeautify will not attempt to find derived
         * units. */
        return nullVector;
      }
      float exponentFloat = static_cast<const Rational&>(exp)
                                .node()
                                ->templatedApproximate<float>();
      /* We limit to INT_MAX / 3 because an exponent might get bigger with
       * simplification. As a worst case scenario, (_s²_m²_kg/_A²)^n should be
       * simplified to (_s^5_S)^n. If 2*n is under INT_MAX, 5*n might not. */
      if (std::fabs(exponentFloat) < static_cast<float>(INT_MAX) / 3.f) {
        // Exponent can be safely casted as int
        exponent = static_cast<int>(std::round(exponentFloat));
        assert(std::fabs(exponentFloat - static_cast<float>(exponent)) <= 0.5f);
      } else {
        /* Base units vector will ignore this coefficient, to avoid exponent
         * overflow. In any way, shallowBeautify will conserve homogeneity. */
        if (!canIgnoreCoefficients) {
          return nullVector;
        }
        exponent = 0;
      }
      factor = factor.childAtIndex(0);
    }
    // Fill the vector with the unit's exponent
    assert(factor.otype() == ExpressionNode::Type::OUnit);
    vector.addAllCoefficients(
        static_cast<OUnit&>(factor).node()->representative()->dimensionVector(),
        exponent);
    if (++factorIndex >= numberOfFactors) {
      break;
    }
    factor = baseUnits.childAtIndex(factorIndex);
  } while (true);
  return vector;
}

OExpression UnitNode::DimensionVector::toBaseUnits() const {
  OExpression result = Multiplication::Builder();
  int numberOfChildren = 0;
  for (int i = 0; i < k_numberOfBaseUnits; i++) {
    // We require the base units to be the first seven in DefaultRepresentatives
    const Representative* representative =
        Representative::DefaultRepresentatives()[i];
    assert(representative);
    const Prefix* prefix = representative->basePrefix();
    int exponent = coefficientAtIndex(i);
    OExpression e;
    if (exponent == 0) {
      continue;
    }
    if (exponent == 1) {
      e = OUnit::Builder(representative, prefix);
    } else {
      e = Power::Builder(OUnit::Builder(representative, prefix),
                         Rational::Builder(exponent));
    }
    static_cast<Multiplication&>(result).addChildAtIndexInPlace(
        e, numberOfChildren, numberOfChildren);
    numberOfChildren++;
  }
  assert(numberOfChildren > 0);
  result = static_cast<Multiplication&>(result).squashUnaryHierarchyInPlace();
  return result;
}

// UnitNode::Representative
const UnitNode::Representative* const*
UnitNode::Representative::DefaultRepresentatives() {
  constexpr static SpeedRepresentative defaultSpeedRepresentative =
      SpeedRepresentative::Default();
  constexpr static const Representative*
      defaultRepresentatives[k_numberOfDimensions] = {
          OUnit::k_timeRepresentatives,
          OUnit::k_distanceRepresentatives,
          OUnit::k_angleRepresentatives,
          OUnit::k_massRepresentatives,
          OUnit::k_currentRepresentatives,
          OUnit::k_temperatureRepresentatives,
          OUnit::k_amountOfSubstanceRepresentatives,
          OUnit::k_luminousIntensityRepresentatives,
          OUnit::k_frequencyRepresentatives,
          OUnit::k_forceRepresentatives,
          OUnit::k_pressureRepresentatives,
          OUnit::k_energyRepresentatives,
          OUnit::k_powerRepresentatives,
          OUnit::k_electricChargeRepresentatives,
          OUnit::k_electricPotentialRepresentatives,
          OUnit::k_electricCapacitanceRepresentatives,
          OUnit::k_electricResistanceRepresentatives,
          OUnit::k_electricConductanceRepresentatives,
          OUnit::k_magneticFluxRepresentatives,
          OUnit::k_magneticFieldRepresentatives,
          OUnit::k_inductanceRepresentatives,
          OUnit::k_catalyticActivityRepresentatives,
          OUnit::k_surfaceRepresentatives,
          OUnit::k_volumeRepresentatives,
          &defaultSpeedRepresentative,
      };
  return defaultRepresentatives;
}

const UnitNode::Representative*
UnitNode::Representative::RepresentativeForDimension(
    UnitNode::DimensionVector vector) {
  for (int i = 0; i < k_numberOfDimensions; i++) {
    const Representative* representative =
        Representative::DefaultRepresentatives()[i];
    if (vector == representative->dimensionVector()) {
      return representative;
    }
  }
  return nullptr;
}

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

size_t UnitNode::Prefix::serialize(char* buffer, size_t bufferSize) const {
  assert(bufferSize >= 0);
  return std::min<size_t>(strlcpy(buffer, m_symbol, bufferSize),
                          bufferSize - 1);
}

const UnitNode::Representative*
UnitNode::Representative::defaultFindBestRepresentative(
    double value, double exponent,
    const UnitNode::Representative* representatives, int length,
    const Prefix** prefix) const {
  assert(length >= 1);
  /* Return this if every other representative gives an accuracy of 0 or Inf.
   * This can happen when searching for an Imperial representative for 1m^20000
   * for example. */
  const Representative* result = this;
  double accuracy = 0.;
  const Prefix* currentPrefix = Prefix::EmptyPrefix();
  const Representative* currentRepresentative = representatives;
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

size_t UnitNode::Representative::serialize(char* buffer, size_t bufferSize,
                                           const Prefix* prefix) const {
  size_t length = 0;
  length += prefix->serialize(buffer, bufferSize);
  assert(length == 0 || isInputPrefixable());
  assert(length < bufferSize);
  buffer += length;
  bufferSize -= length;
  assert(bufferSize >= 0);
  length += std::min<size_t>(
      strlcpy(buffer, m_rootSymbols.mainAlias(), bufferSize), bufferSize - 1);
  return length;
}

bool UnitNode::Representative::canParseWithEquivalents(
    const char* symbol, size_t length, const Representative** representative,
    const Prefix** prefix) const {
  const Representative* candidate = representativesOfSameDimension();
  if (!candidate) {
    return false;
  }
  for (int i = 0; i < numberOfRepresentatives(); i++) {
    AliasesList rootSymbolAliasesList = (candidate + i)->rootSymbols();
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

bool UnitNode::Representative::canParse(const char* symbol, size_t length,
                                        const Prefix** prefix) const {
  if (!isInputPrefixable()) {
    if (prefix) {
      *prefix = Prefix::EmptyPrefix();
    }
    return length == 0;
  }
  for (size_t i = 0; i < Prefix::k_numberOfPrefixes; i++) {
    const Prefix* pre = Prefix::Prefixes() + i;
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

OExpression UnitNode::Representative::toBaseUnits(
    const ReductionContext& reductionContext) const {
  OExpression result;
  if (isBaseUnit()) {
    result = OUnit::Builder(this, basePrefix());
  } else {
    result = dimensionVector().toBaseUnits();
  }
  Rational basePrefixFactor =
      Rational::IntegerPower(Rational::Builder(10), -basePrefix()->exponent());
  OExpression factor =
      Multiplication::Builder(basePrefixFactor,
                              ratioExpressionReduced(reductionContext))
          .shallowReduce(reductionContext);
  return Multiplication::Builder(factor, result);
}

bool UnitNode::Representative::canPrefix(const UnitNode::Prefix* prefix,
                                         bool input) const {
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

const UnitNode::Prefix* UnitNode::Representative::findBestPrefix(
    double value, double exponent) const {
  if (!isOutputPrefixable()) {
    return Prefix::EmptyPrefix();
  }
  if (value < Float<double>::EpsilonLax()) {
    return basePrefix();
  }
  const Prefix* res = basePrefix();
  const float magnitude = std::log10(std::fabs(value));
  float bestOrder = magnitude;
  for (int i = 0; i < Prefix::k_numberOfPrefixes; i++) {
    if (!canPrefix(Prefix::Prefixes() + i, false)) {
      continue;
    }
    float order = magnitude - (Prefix::Prefixes()[i].exponent() -
                               basePrefix()->exponent()) *
                                  exponent;
    if (compareMagnitudeOrders(order, bestOrder)) {
      bestOrder = order;
      res = Prefix::Prefixes() + i;
    }
  }
  return res;
}

// UnitNode::___Representative
int UnitNode::TimeRepresentative::numberOfRepresentatives() const {
  return std::size(OUnit::k_timeRepresentatives);
}
const UnitNode::Representative*
UnitNode::TimeRepresentative::representativesOfSameDimension() const {
  return OUnit::k_timeRepresentatives;
}

int UnitNode::DistanceRepresentative::numberOfRepresentatives() const {
  return std::size(OUnit::k_distanceRepresentatives);
}
const UnitNode::Representative*
UnitNode::DistanceRepresentative::representativesOfSameDimension() const {
  return OUnit::k_distanceRepresentatives;
}

int UnitNode::AngleRepresentative::numberOfRepresentatives() const {
  return std::size(OUnit::k_angleRepresentatives);
}
const UnitNode::Representative*
UnitNode::AngleRepresentative::representativesOfSameDimension() const {
  return OUnit::k_angleRepresentatives;
}

int UnitNode::MassRepresentative::numberOfRepresentatives() const {
  return std::size(OUnit::k_massRepresentatives);
}
const UnitNode::Representative*
UnitNode::MassRepresentative::representativesOfSameDimension() const {
  return OUnit::k_massRepresentatives;
}

int UnitNode::CurrentRepresentative::numberOfRepresentatives() const {
  return std::size(OUnit::k_currentRepresentatives);
}
const UnitNode::Representative*
UnitNode::CurrentRepresentative::representativesOfSameDimension() const {
  return OUnit::k_currentRepresentatives;
}

int UnitNode::TemperatureRepresentative::numberOfRepresentatives() const {
  return std::size(OUnit::k_temperatureRepresentatives);
}
const UnitNode::Representative*
UnitNode::TemperatureRepresentative::representativesOfSameDimension() const {
  return OUnit::k_temperatureRepresentatives;
}

int UnitNode::AmountOfSubstanceRepresentative::numberOfRepresentatives() const {
  return std::size(OUnit::k_amountOfSubstanceRepresentatives);
}
const UnitNode::Representative*
UnitNode::AmountOfSubstanceRepresentative::representativesOfSameDimension()
    const {
  return OUnit::k_amountOfSubstanceRepresentatives;
}

int UnitNode::LuminousIntensityRepresentative::numberOfRepresentatives() const {
  return std::size(OUnit::k_luminousIntensityRepresentatives);
}
const UnitNode::Representative*
UnitNode::LuminousIntensityRepresentative::representativesOfSameDimension()
    const {
  return OUnit::k_luminousIntensityRepresentatives;
}

int UnitNode::FrequencyRepresentative::numberOfRepresentatives() const {
  return std::size(OUnit::k_frequencyRepresentatives);
}
const UnitNode::Representative*
UnitNode::FrequencyRepresentative::representativesOfSameDimension() const {
  return OUnit::k_frequencyRepresentatives;
}

int UnitNode::ForceRepresentative::numberOfRepresentatives() const {
  return std::size(OUnit::k_forceRepresentatives);
}
const UnitNode::Representative*
UnitNode::ForceRepresentative::representativesOfSameDimension() const {
  return OUnit::k_forceRepresentatives;
}

int UnitNode::PressureRepresentative::numberOfRepresentatives() const {
  return std::size(OUnit::k_pressureRepresentatives);
}
const UnitNode::Representative*
UnitNode::PressureRepresentative::representativesOfSameDimension() const {
  return OUnit::k_pressureRepresentatives;
}

int UnitNode::EnergyRepresentative::numberOfRepresentatives() const {
  return std::size(OUnit::k_energyRepresentatives);
}
const UnitNode::Representative*
UnitNode::EnergyRepresentative::representativesOfSameDimension() const {
  return OUnit::k_energyRepresentatives;
}

int UnitNode::PowerRepresentative::numberOfRepresentatives() const {
  return std::size(OUnit::k_powerRepresentatives);
}
const UnitNode::Representative*
UnitNode::PowerRepresentative::representativesOfSameDimension() const {
  return OUnit::k_powerRepresentatives;
}

int UnitNode::ElectricChargeRepresentative::numberOfRepresentatives() const {
  return std::size(OUnit::k_electricChargeRepresentatives);
}
const UnitNode::Representative*
UnitNode::ElectricChargeRepresentative::representativesOfSameDimension() const {
  return OUnit::k_electricChargeRepresentatives;
}

int UnitNode::ElectricPotentialRepresentative::numberOfRepresentatives() const {
  return std::size(OUnit::k_electricPotentialRepresentatives);
}
const UnitNode::Representative*
UnitNode::ElectricPotentialRepresentative::representativesOfSameDimension()
    const {
  return OUnit::k_electricPotentialRepresentatives;
}

int UnitNode::ElectricCapacitanceRepresentative::numberOfRepresentatives()
    const {
  return std::size(OUnit::k_electricCapacitanceRepresentatives);
}
const UnitNode::Representative*
UnitNode::ElectricCapacitanceRepresentative::representativesOfSameDimension()
    const {
  return OUnit::k_electricCapacitanceRepresentatives;
}

int UnitNode::ElectricResistanceRepresentative::numberOfRepresentatives()
    const {
  return std::size(OUnit::k_electricResistanceRepresentatives);
}
const UnitNode::Representative*
UnitNode::ElectricResistanceRepresentative::representativesOfSameDimension()
    const {
  return OUnit::k_electricResistanceRepresentatives;
}

int UnitNode::ElectricConductanceRepresentative::numberOfRepresentatives()
    const {
  return std::size(OUnit::k_electricConductanceRepresentatives);
}
const UnitNode::Representative*
UnitNode::ElectricConductanceRepresentative::representativesOfSameDimension()
    const {
  return OUnit::k_electricConductanceRepresentatives;
}

int UnitNode::MagneticFluxRepresentative::numberOfRepresentatives() const {
  return std::size(OUnit::k_magneticFluxRepresentatives);
}
const UnitNode::Representative*
UnitNode::MagneticFluxRepresentative::representativesOfSameDimension() const {
  return OUnit::k_magneticFluxRepresentatives;
}

int UnitNode::MagneticFieldRepresentative::numberOfRepresentatives() const {
  return std::size(OUnit::k_magneticFieldRepresentatives);
}
const UnitNode::Representative*
UnitNode::MagneticFieldRepresentative::representativesOfSameDimension() const {
  return OUnit::k_magneticFieldRepresentatives;
}

int UnitNode::InductanceRepresentative::numberOfRepresentatives() const {
  return std::size(OUnit::k_inductanceRepresentatives);
}
const UnitNode::Representative*
UnitNode::InductanceRepresentative::representativesOfSameDimension() const {
  return OUnit::k_inductanceRepresentatives;
}

int UnitNode::CatalyticActivityRepresentative::numberOfRepresentatives() const {
  return std::size(OUnit::k_catalyticActivityRepresentatives);
}
const UnitNode::Representative*
UnitNode::CatalyticActivityRepresentative::representativesOfSameDimension()
    const {
  return OUnit::k_catalyticActivityRepresentatives;
}

int UnitNode::SurfaceRepresentative::numberOfRepresentatives() const {
  return std::size(OUnit::k_surfaceRepresentatives);
}
const UnitNode::Representative*
UnitNode::SurfaceRepresentative::representativesOfSameDimension() const {
  return OUnit::k_surfaceRepresentatives;
}

int UnitNode::VolumeRepresentative::numberOfRepresentatives() const {
  return std::size(OUnit::k_volumeRepresentatives);
}
const UnitNode::Representative*
UnitNode::VolumeRepresentative::representativesOfSameDimension() const {
  return OUnit::k_volumeRepresentatives;
}

int UnitNode::TimeRepresentative::setAdditionalExpressions(
    double value, OExpression* dest, int availableLength,
    const ReductionContext& reductionContext) const {
  assert(availableLength >= 1);
  /* Use all representatives but week */
  const OUnit splitUnits[] = {
      OUnit::Builder(
          representativesOfSameDimension() + OUnit::k_secondRepresentativeIndex,
          Prefix::EmptyPrefix()),
      OUnit::Builder(
          representativesOfSameDimension() + OUnit::k_minuteRepresentativeIndex,
          Prefix::EmptyPrefix()),
      OUnit::Builder(
          representativesOfSameDimension() + OUnit::k_hourRepresentativeIndex,
          Prefix::EmptyPrefix()),
      OUnit::Builder(
          representativesOfSameDimension() + OUnit::k_dayRepresentativeIndex,
          Prefix::EmptyPrefix()),
      OUnit::Builder(
          representativesOfSameDimension() + OUnit::k_monthRepresentativeIndex,
          Prefix::EmptyPrefix()),
      OUnit::Builder(
          representativesOfSameDimension() + OUnit::k_yearRepresentativeIndex,
          Prefix::EmptyPrefix()),
  };
  dest[0] = OUnit::BuildSplit(value, splitUnits, numberOfRepresentatives() - 1,
                              reductionContext);
  return 1;
}

const UnitNode::Representative*
UnitNode::DistanceRepresentative::standardRepresentative(
    double value, double exponent, const ReductionContext& reductionContext,
    const Prefix** prefix) const {
  return (reductionContext.unitFormat() == Preferences::UnitFormat::Metric)
             ?
             /* Exclude imperial units from the search. */
             defaultFindBestRepresentative(
                 value, exponent, representativesOfSameDimension(),
                 OUnit::k_inchRepresentativeIndex, prefix)
             :
             /* Exclude m form the search. */
             defaultFindBestRepresentative(
                 value, exponent, representativesOfSameDimension() + 1,
                 numberOfRepresentatives() - 1, prefix);
}

int UnitNode::DistanceRepresentative::setAdditionalExpressions(
    double value, OExpression* dest, int availableLength,
    const ReductionContext& reductionContext) const {
  assert(availableLength >= 1);
  if (reductionContext.unitFormat() == Preferences::UnitFormat::Metric) {
    return 0;
  }
  const OUnit splitUnits[] = {
      OUnit::Builder(
          representativesOfSameDimension() + OUnit::k_inchRepresentativeIndex,
          Prefix::EmptyPrefix()),
      OUnit::Builder(
          representativesOfSameDimension() + OUnit::k_footRepresentativeIndex,
          Prefix::EmptyPrefix()),
      OUnit::Builder(
          representativesOfSameDimension() + OUnit::k_yardRepresentativeIndex,
          Prefix::EmptyPrefix()),
      OUnit::Builder(
          representativesOfSameDimension() + OUnit::k_mileRepresentativeIndex,
          Prefix::EmptyPrefix()),
  };
  dest[0] = OUnit::BuildSplit(value, splitUnits, std::size(splitUnits),
                              reductionContext);
  return 1;
}

const UnitNode::Representative*
UnitNode::AngleRepresentative::DefaultRepresentativeForAngleUnit(
    Preferences::AngleUnit angleUnit) {
  switch (angleUnit) {
    case Preferences::AngleUnit::Degree:
      return OUnit::k_angleRepresentatives + OUnit::k_degreeRepresentativeIndex;
    case Preferences::AngleUnit::Radian:
      return OUnit::k_angleRepresentatives + OUnit::k_radianRepresentativeIndex;
    default:
      assert(angleUnit == Preferences::AngleUnit::Gradian);
      return OUnit::k_angleRepresentatives +
             OUnit::k_gradianRepresentativeIndex;
  }
}

const UnitNode::Representative*
UnitNode::AngleRepresentative::standardRepresentative(
    double value, double exponent, const ReductionContext& reductionContext,
    const Prefix** prefix) const {
  if (reductionContext.angleUnit() ==
      Poincare::Preferences::AngleUnit::Degree) {
    return defaultFindBestRepresentative(
        value, exponent,
        representativesOfSameDimension() +
            OUnit::k_arcSecondRepresentativeIndex,
        3, prefix);
  }
  return DefaultRepresentativeForAngleUnit(reductionContext.angleUnit());
}

OExpression UnitNode::AngleRepresentative::convertInto(
    OExpression value, const UnitNode::Representative* other,
    const ReductionContext& reductionContext) const {
  assert(dimensionVector() == other->dimensionVector());
  OExpression unit = OUnit::Builder(other, Prefix::EmptyPrefix());
  OExpression inRadians =
      Multiplication::Builder(value, ratioExpressionReduced(reductionContext))
          .shallowReduce(reductionContext);
  OExpression inOther =
      Division::Builder(inRadians,
                        other->ratioExpressionReduced(reductionContext))
          .shallowReduce(reductionContext)
          .deepBeautify(reductionContext);
  return Multiplication::Builder(inOther, unit);
}

int UnitNode::AngleRepresentative::setAdditionalExpressionsWithExactValue(
    OExpression exactValue, double value, OExpression* dest,
    int availableLength, const ReductionContext& reductionContext) const {
  assert(availableLength >= 2);
  int numberOfResults = 0;
  // Conversion to degrees should be added to all units not degree related
  if (this == representativesOfSameDimension() +
                  OUnit::k_radianRepresentativeIndex ||
      this == representativesOfSameDimension() +
                  OUnit::k_gradianRepresentativeIndex) {
    const Representative* degree =
        representativesOfSameDimension() + OUnit::k_degreeRepresentativeIndex;
    dest[numberOfResults++] =
        convertInto(exactValue.clone(), degree, reductionContext)
            .approximateKeepingUnits<double>(reductionContext);
  }
  // Degrees related units should show their decomposition in DMS
  const OUnit splitUnits[] = {
      OUnit::Builder(representativesOfSameDimension() +
                         OUnit::k_arcSecondRepresentativeIndex,
                     Prefix::EmptyPrefix()),
      OUnit::Builder(representativesOfSameDimension() +
                         OUnit::k_arcMinuteRepresentativeIndex,
                     Prefix::EmptyPrefix()),
      OUnit::Builder(
          representativesOfSameDimension() + OUnit::k_degreeRepresentativeIndex,
          Prefix::EmptyPrefix()),
  };
  OExpression split = OUnit::BuildSplit(
      value, splitUnits, std::size(splitUnits), reductionContext);
  if (!split.isUndefined()) {
    dest[numberOfResults++] = split;
  }
  // Conversion to radians should be added to all other units.
  if (this !=
      representativesOfSameDimension() + OUnit::k_radianRepresentativeIndex) {
    const Representative* radian =
        representativesOfSameDimension() + OUnit::k_radianRepresentativeIndex;
    dest[numberOfResults++] = convertInto(exactValue, radian, reductionContext);
  }
  return numberOfResults;
}

const UnitNode::Prefix* UnitNode::MassRepresentative::basePrefix() const {
  return isBaseUnit() ? Prefix::Prefixes() + OUnit::k_kiloPrefixIndex
                      : Prefix::EmptyPrefix();
}

const UnitNode::Representative*
UnitNode::MassRepresentative::standardRepresentative(
    double value, double exponent, const ReductionContext& reductionContext,
    const Prefix** prefix) const {
  if (reductionContext.unitFormat() == Preferences::UnitFormat::Imperial) {
    return defaultFindBestRepresentative(
        value, exponent,
        representativesOfSameDimension() + OUnit::k_ounceRepresentativeIndex,
        OUnit::k_shortTonRepresentativeIndex -
            OUnit::k_ounceRepresentativeIndex + 1,
        prefix);
  }
  assert(reductionContext.unitFormat() == Preferences::UnitFormat::Metric);
  bool useTon = exponent == 1. && value >= (representativesOfSameDimension() +
                                            OUnit::k_tonRepresentativeIndex)
                                               ->ratio();
  int representativeIndex = useTon ? OUnit::k_tonRepresentativeIndex
                                   : OUnit::k_gramRepresentativeIndex;
  return defaultFindBestRepresentative(
      value, exponent, representativesOfSameDimension() + representativeIndex,
      1, prefix);
}

int UnitNode::MassRepresentative::setAdditionalExpressions(
    double value, OExpression* dest, int availableLength,
    const ReductionContext& reductionContext) const {
  assert(availableLength >= 1);
  if (reductionContext.unitFormat() == Preferences::UnitFormat::Metric) {
    return 0;
  }
  const OUnit splitUnits[] = {
      OUnit::Builder(
          representativesOfSameDimension() + OUnit::k_ounceRepresentativeIndex,
          Prefix::EmptyPrefix()),
      OUnit::Builder(
          representativesOfSameDimension() + OUnit::k_poundRepresentativeIndex,
          Prefix::EmptyPrefix()),
      OUnit::Builder(representativesOfSameDimension() +
                         OUnit::k_shortTonRepresentativeIndex,
                     Prefix::EmptyPrefix()),
  };
  dest[0] = OUnit::BuildSplit(value, splitUnits, std::size(splitUnits),
                              reductionContext);
  return 1;
}

double UnitNode::TemperatureRepresentative::ConvertTemperatures(
    double value, const Representative* source, const Representative* target) {
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

int UnitNode::TemperatureRepresentative::setAdditionalExpressions(
    double value, OExpression* dest, int availableLength,
    const ReductionContext& reductionContext) const {
  assert(availableLength >= 2);
  const Representative* celsius =
      TemperatureRepresentative::Default().representativesOfSameDimension() +
      OUnit::k_celsiusRepresentativeIndex;
  const Representative* fahrenheit =
      TemperatureRepresentative::Default().representativesOfSameDimension() +
      OUnit::k_fahrenheitRepresentativeIndex;
  const Representative* kelvin =
      TemperatureRepresentative::Default().representativesOfSameDimension() +
      OUnit::k_kelvinRepresentativeIndex;
  const Representative* targets[] = {
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
        OUnit::Builder(targets[i], Prefix::EmptyPrefix()));
  }
  assert(numberOfExpressionsSet == 2);
  return numberOfExpressionsSet;
}

int UnitNode::EnergyRepresentative::setAdditionalExpressions(
    double value, OExpression* dest, int availableLength,
    const ReductionContext& reductionContext) const {
  assert(availableLength >= 2);
  int index = 0;
  /* 1. Convert into Joules
   * As J is just a shorthand for _kg_m^2_s^-2, the value is used as is. */
  const Representative* joule =
      representativesOfSameDimension() + OUnit::k_jouleRepresentativeIndex;
  const Prefix* joulePrefix = joule->findBestPrefix(value, 1.);
  dest[index++] = Multiplication::Builder(
      Float<double>::Builder(value * std::pow(10., -joulePrefix->exponent())),
      OUnit::Builder(joule, joulePrefix));
  /* 2. Convert into Wh
   * As value is expressed in SI units (ie _kg_m^2_s^-2), the ratio is that of
   * hours to seconds. */
  const Representative* hour =
      TimeRepresentative::Default().representativesOfSameDimension() +
      OUnit::k_hourRepresentativeIndex;
  const Representative* watt =
      PowerRepresentative::Default().representativesOfSameDimension() +
      OUnit::k_wattRepresentativeIndex;
  double adjustedValue = value / hour->ratio() / watt->ratio();
  const Prefix* wattPrefix = watt->findBestPrefix(adjustedValue, 1.);
  dest[index++] = Multiplication::Builder(
      Float<double>::Builder(adjustedValue *
                             std::pow(10., -wattPrefix->exponent())),
      Multiplication::Builder(OUnit::Builder(watt, wattPrefix),
                              OUnit::Builder(hour, Prefix::EmptyPrefix())));
  /* 3. Convert into eV */
  const Representative* eV = representativesOfSameDimension() +
                             OUnit::k_electronVoltRepresentativeIndex;
  adjustedValue = value / eV->ratio();
  const Prefix* eVPrefix = eV->findBestPrefix(adjustedValue, 1.);
  dest[index++] = Multiplication::Builder(
      Float<double>::Builder(adjustedValue *
                             std::pow(10., -eVPrefix->exponent())),
      OUnit::Builder(eV, eVPrefix));
  return index;
}

const UnitNode::Representative*
UnitNode::SurfaceRepresentative::standardRepresentative(
    double value, double exponent, const ReductionContext& reductionContext,
    const Prefix** prefix) const {
  *prefix = Prefix::EmptyPrefix();
  return representativesOfSameDimension() +
         (reductionContext.unitFormat() == Preferences::UnitFormat::Metric
              ? OUnit::k_hectareRepresentativeIndex
              : OUnit::k_acreRepresentativeIndex);
}

int UnitNode::SurfaceRepresentative::setAdditionalExpressions(
    double value, OExpression* dest, int availableLength,
    const ReductionContext& reductionContext) const {
  assert(availableLength >= 2);
  OExpression* destMetric;
  OExpression* destImperial = nullptr;
  if (reductionContext.unitFormat() == Preferences::UnitFormat::Metric) {
    destMetric = dest;
  } else {
    destImperial = dest;
    destMetric = dest + 1;
  }
  // 1. Convert to hectares
  const Representative* hectare =
      representativesOfSameDimension() + OUnit::k_hectareRepresentativeIndex;
  *destMetric =
      Multiplication::Builder(Float<double>::Builder(value / hectare->ratio()),
                              OUnit::Builder(hectare, Prefix::EmptyPrefix()));
  // 2. Convert to acres
  if (!destImperial) {
    return 1;
  }
  const Representative* acre =
      representativesOfSameDimension() + OUnit::k_acreRepresentativeIndex;
  *destImperial =
      Multiplication::Builder(Float<double>::Builder(value / acre->ratio()),
                              OUnit::Builder(acre, Prefix::EmptyPrefix()));
  return 2;
}

const UnitNode::Representative*
UnitNode::VolumeRepresentative::standardRepresentative(
    double value, double exponent, const ReductionContext& reductionContext,
    const Prefix** prefix) const {
  if (reductionContext.unitFormat() == Preferences::UnitFormat::Metric) {
    *prefix = representativesOfSameDimension()->findBestPrefix(value, exponent);
    return representativesOfSameDimension();
  }
  return defaultFindBestRepresentative(value, exponent,
                                       representativesOfSameDimension() + 1,
                                       numberOfRepresentatives() - 1, prefix);
}

int UnitNode::VolumeRepresentative::setAdditionalExpressions(
    double value, OExpression* dest, int availableLength,
    const ReductionContext& reductionContext) const {
  assert(availableLength >= 2);
  OExpression* destMetric;
  OExpression* destImperial = nullptr;
  if (reductionContext.unitFormat() == Preferences::UnitFormat::Metric) {
    destMetric = dest;
  } else {
    destImperial = dest;
    destMetric = dest + 1;
  }
  // 1. Convert to liters
  const Representative* liter =
      representativesOfSameDimension() + OUnit::k_literRepresentativeIndex;
  double adjustedValue = value / liter->ratio();
  const Prefix* literPrefix = liter->findBestPrefix(adjustedValue, 1.);
  *destMetric = Multiplication::Builder(
      Float<double>::Builder(adjustedValue *
                             pow(10., -literPrefix->exponent())),
      OUnit::Builder(liter, literPrefix));
  // 2. Convert to imperial volumes
  if (!destImperial) {
    return 1;
  }
  const OUnit splitUnits[] = {
      OUnit::Builder(
          representativesOfSameDimension() + OUnit::k_cupRepresentativeIndex,
          Prefix::EmptyPrefix()),
      OUnit::Builder(
          representativesOfSameDimension() + OUnit::k_pintRepresentativeIndex,
          Prefix::EmptyPrefix()),
      OUnit::Builder(
          representativesOfSameDimension() + OUnit::k_quartRepresentativeIndex,
          Prefix::EmptyPrefix()),
      OUnit::Builder(
          representativesOfSameDimension() + OUnit::k_gallonRepresentativeIndex,
          Prefix::EmptyPrefix()),
  };
  *destImperial = OUnit::BuildSplit(value, splitUnits, std::size(splitUnits),
                                    reductionContext);
  return 2;
}

int UnitNode::SpeedRepresentative::setAdditionalExpressions(
    double value, OExpression* dest, int availableLength,
    const ReductionContext& reductionContext) const {
  assert(availableLength >= 2);
  OExpression* destMetric;
  OExpression* destImperial = nullptr;
  if (reductionContext.unitFormat() == Preferences::UnitFormat::Metric) {
    destMetric = dest;
  } else {
    destImperial = dest;
    destMetric = dest + 1;
  }
  // 1. Convert to km/h
  const Representative* meter =
      DistanceRepresentative::Default().representativesOfSameDimension() +
      OUnit::k_meterRepresentativeIndex;
  const Representative* hour =
      TimeRepresentative::Default().representativesOfSameDimension() +
      OUnit::k_hourRepresentativeIndex;
  *destMetric = Multiplication::Builder(
      Float<double>::Builder(value / 1000. * hour->ratio()),
      Multiplication::Builder(
          OUnit::Builder(meter, Prefix::Prefixes() + OUnit::k_kiloPrefixIndex),
          Power::Builder(OUnit::Builder(hour, Prefix::EmptyPrefix()),
                         Rational::Builder(-1))));
  // 2. Convert to mph
  if (!destImperial) {
    return 1;
  }
  const Representative* mile =
      DistanceRepresentative::Default().representativesOfSameDimension() +
      OUnit::k_mileRepresentativeIndex;
  *destImperial = Multiplication::Builder(
      Float<double>::Builder(value / mile->ratio() * hour->ratio()),
      Multiplication::Builder(
          OUnit::Builder(mile, Prefix::EmptyPrefix()),
          Power::Builder(OUnit::Builder(hour, Prefix::EmptyPrefix()),
                         Rational::Builder(-1))));
  return 2;
}

// UnitNode
OExpression UnitNode::removeUnit(OExpression* unit) {
  return OUnit(this).removeUnit(unit);
}

size_t UnitNode::serialize(char* buffer, size_t bufferSize,
                           Preferences::PrintFloatMode floatDisplayMode,
                           int numberOfSignificantDigits) const {
  assert(bufferSize >= 0);
  size_t underscoreLength =
      std::min<size_t>(strlcpy(buffer, "_", bufferSize), bufferSize - 1);
  buffer += underscoreLength;
  bufferSize -= underscoreLength;
  return underscoreLength +
         m_representative->serialize(buffer, bufferSize, m_prefix);
}

int UnitNode::simplificationOrderSameType(const ExpressionNode* e,
                                          bool ascending,
                                          bool ignoreParentheses) const {
  if (!ascending) {
    return e->simplificationOrderSameType(this, true, ignoreParentheses);
  }
  assert(otype() == e->otype());
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

OExpression UnitNode::shallowReduce(const ReductionContext& reductionContext) {
  return OUnit(this).shallowReduce(reductionContext);
}

OExpression UnitNode::shallowBeautify(
    const ReductionContext& reductionContext) {
  return OUnit(this).shallowBeautify();
}

template <typename T>
Evaluation<T> UnitNode::templatedApproximate(
    const ApproximationContext& approximationContext) const {
  return Complex<T>::Undefined();
}

// OUnit
OUnit OUnit::Builder(const OUnit::Representative* representative,
                     const Prefix* prefix) {
  void* bufferNode = Pool::sharedPool->alloc(sizeof(UnitNode));
  UnitNode* node = new (bufferNode) UnitNode(representative, prefix);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  return static_cast<OUnit&>(h);
}

bool OUnit::CanParse(const char* symbol, size_t length,
                     const OUnit::Representative** representative,
                     const OUnit::Prefix** prefix) {
  if (symbol[0] == '_') {
    symbol++;
    length--;
  }
  for (int i = 0; i < Representative::k_numberOfDimensions; i++) {
    if (Representative::DefaultRepresentatives()[i]->canParseWithEquivalents(
            symbol, length, representative, prefix)) {
      return true;
    }
  }
  return false;
}

static void chooseBestRepresentativeAndPrefixForValueOnSingleUnit(
    OExpression unit, double* value, const ReductionContext& reductionContext,
    bool optimizePrefix) {
  double exponent = 1.f;
  OExpression factor = unit;
  if (factor.otype() == ExpressionNode::Type::Power) {
    OExpression childExponent = factor.childAtIndex(1);
    assert(factor.childAtIndex(0).otype() == ExpressionNode::Type::OUnit);
    assert(factor.childAtIndex(1).otype() == ExpressionNode::Type::Rational);
    ApproximationContext approximationContext(reductionContext);
    exponent = static_cast<Rational&>(childExponent)
                   .approximateToScalar<double>(approximationContext);
    factor = factor.childAtIndex(0);
  }
  assert(factor.otype() == ExpressionNode::Type::OUnit);
  if (exponent == 0.f) {
    /* Finding the best representative for a unit with exponent 0 doesn't
     * really make sense, and should only happen with a weak ReductionTarget
     * (such as in Graph app), that only rely on approximations. We keep the
     * unit unchanged as it will approximate to undef anyway. */
    return;
  }
  static_cast<OUnit&>(factor).chooseBestRepresentativeAndPrefix(
      value, exponent, reductionContext, optimizePrefix);
}

void OUnit::ChooseBestRepresentativeAndPrefixForValue(
    OExpression units, double* value,
    const ReductionContext& reductionContext) {
  int numberOfFactors;
  OExpression factor;
  if (units.otype() == ExpressionNode::Type::Multiplication) {
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

bool OUnit::ShouldDisplayAdditionalOutputs(double value, OExpression unit,
                                           Preferences::UnitFormat unitFormat) {
  if (unit.isUninitialized() || !std::isfinite(value)) {
    return false;
  }
  UnitNode::DimensionVector vector =
      UnitNode::DimensionVector::FromBaseUnits(unit, false);
  const Representative* representative =
      Representative::RepresentativeForDimension(vector);

  ExpressionTest isNonBase = [](const OExpression e, Context* context) {
    return !e.isUninitialized() && e.otype() == ExpressionNode::Type::OUnit &&
           !e.convert<OUnit>().isBaseUnit();
  };

  return (representative != nullptr &&
          representative->hasSpecialAdditionalExpressions(value, unitFormat)) ||
         unit.recursivelyMatches(isNonBase);
}

int OUnit::SetAdditionalExpressions(OExpression units, double value,
                                    OExpression* dest, int availableLength,
                                    const ReductionContext& reductionContext,
                                    const OExpression exactOutput) {
  if (units.isUninitialized()) {
    return 0;
  }
  const Representative* representative =
      units.otype() == ExpressionNode::Type::OUnit
          ? static_cast<OUnit&>(units).node()->representative()
          : UnitNode::Representative::RepresentativeForDimension(
                UnitNode::DimensionVector::FromBaseUnits(units));
  if (!representative) {
    return 0;
  }
  if (representative->dimensionVector() ==
      AngleRepresentative::Default().dimensionVector()) {
    /* Angles are the only unit where we want to display the exact value. */
    OExpression unit;
    ReductionContext childContext = reductionContext;
    childContext.setUnitConversion(UnitConversion::None);
    OExpression exactValue =
        exactOutput.cloneAndReduceAndRemoveUnit(childContext, &unit);
    assert(unit.otype() == ExpressionNode::Type::OUnit);
    return static_cast<const AngleRepresentative*>(
               static_cast<OUnit&>(unit).representative())
        ->setAdditionalExpressionsWithExactValue(
            exactValue, value, dest, availableLength, reductionContext);
  }
  return representative->setAdditionalExpressions(value, dest, availableLength,
                                                  reductionContext);
}

OExpression OUnit::BuildSplit(double value, const OUnit* units, int length,
                              const ReductionContext& reductionContext) {
  assert(!std::isnan(value));
  assert(units);
  assert(length > 0);

  double baseRatio = units->node()->representative()->ratio();
  double basedValue = value / baseRatio;
  /* WARNING: Maybe this should be compared to 0.0 instead of EpsilonLax ? (see
   * below) */
  if (std::isinf(basedValue) ||
      std::fabs(basedValue) < Float<double>::EpsilonLax()) {
    return Multiplication::Builder(Number::FloatNumber(basedValue), units[0]);
  }
  double err =
      std::pow(10.0, Poincare::PrintFloat::k_maxNumberOfSignificantDigits - 1 -
                         std::ceil(log10(std::fabs(basedValue))));
  double remain = std::round(basedValue * err) / err;

  Addition res = Addition::Builder();
  for (int i = length - 1; i >= 0; i--) {
    assert(units[i].node()->prefix() == Prefix::EmptyPrefix());
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

OExpression OUnit::ConvertTemperatureUnits(
    OExpression e, OUnit unit, const ReductionContext& reductionContext) {
  const Representative* targetRepr = unit.representative();
  const Prefix* targetPrefix = unit.node()->prefix();
  assert(unit.representative()->dimensionVector() ==
         TemperatureRepresentative::Default().dimensionVector());

  OExpression startUnit;
  e = e.removeUnit(&startUnit);
  if (startUnit.isUninitialized() ||
      startUnit.otype() != ExpressionNode::Type::OUnit) {
    return Undefined::Builder();
  }
  const Representative* startRepr =
      static_cast<OUnit&>(startUnit).representative();
  if (startRepr->dimensionVector() !=
      TemperatureRepresentative::Default().dimensionVector()) {
    return Undefined::Builder();
  }

  const Prefix* startPrefix = static_cast<OUnit&>(startUnit).node()->prefix();
  ApproximationContext approximationContext(reductionContext);
  double value = e.approximateToScalar<double>(approximationContext);
  return Multiplication::Builder(
      Float<double>::Builder(TemperatureRepresentative::ConvertTemperatures(
                                 value * std::pow(10., startPrefix->exponent()),
                                 startRepr, targetRepr) *
                             std::pow(10., -targetPrefix->exponent())),
      unit.clone());
}

bool OUnit::IsForbiddenTemperatureProduct(OExpression e) {
  assert(e.otype() == ExpressionNode::Type::Multiplication);
  if (e.numberOfChildren() != 2) {
    /* A multiplication cannot contain a °C or °F if it does not have 2
     * children, as otherwise the temperature would have reduced itself to
     * undef. */
    return false;
  }
  int temperatureChildIndex = -1;
  for (int i = 0; i < 2; i++) {
    OExpression child = e.childAtIndex(i);
    if (child.otype() == ExpressionNode::Type::OUnit &&
        (static_cast<OUnit&>(child).node()->representative() ==
             k_temperatureRepresentatives + k_celsiusRepresentativeIndex ||
         static_cast<OUnit&>(child).node()->representative() ==
             k_temperatureRepresentatives + k_fahrenheitRepresentativeIndex)) {
      temperatureChildIndex = i;
      break;
    }
  }
  if (temperatureChildIndex < 0) {
    return false;
  }
  if (e.childAtIndex(1 - temperatureChildIndex).hasUnit()) {
    return true;
  }
  OExpression p = e.parent();
  if (p.isUninitialized() || p.isOfType({ExpressionNode::Type::UnitConvert,
                                         ExpressionNode::Type::Store})) {
    return false;
  }
  OExpression pp = p.parent();
  return !(
      p.otype() == ExpressionNode::Type::Opposite &&
      (pp.isUninitialized() || pp.isOfType({ExpressionNode::Type::UnitConvert,
                                            ExpressionNode::Type::Store})));
}

bool OUnit::AllowImplicitAddition(
    const UnitNode::Representative* smallestRepresentative,
    const UnitNode::Representative* biggestRepresentative) {
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

bool OUnit::ForceMarginLeftOfUnit(const OUnit& unit) {
  const UnitNode::Representative* representative = unit.representative();
  for (int i = 0; i < k_numberOfRepresentativesWithoutLeftMargin; i++) {
    if (k_representativesWithoutLeftMargin[i] == representative) {
      return false;
    }
  }
  return true;
}

OExpression OUnit::shallowReduce(ReductionContext reductionContext) {
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
    OExpression p = parent();
    if (p.isUninitialized() ||
        p.isOfType({ExpressionNode::Type::UnitConvert,
                    ExpressionNode::Type::Store,
                    ExpressionNode::Type::Opposite}) ||
        (p.otype() == ExpressionNode::Type::Multiplication &&
         p.numberOfChildren() == 2)) {
      /* If the parent is a UnitConvert, the temperature is always legal.
       * Otherwise, we need to wait until the reduction of the multiplication
       * to fully detect forbidden forms. */
      return *this;
    }
    return replaceWithUndefinedInPlace();
  }

  UnitNode* unitNode = node();
  const Representative* representative = unitNode->representative();
  const Prefix* prefix = unitNode->prefix();

  OExpression result = representative->toBaseUnits(reductionContext)
                           .deepReduce(reductionContext);
  if (prefix != Prefix::EmptyPrefix()) {
    OExpression prefixFactor = Power::Builder(
        Rational::Builder(10), Rational::Builder(prefix->exponent()));
    prefixFactor = prefixFactor.shallowReduce(reductionContext);
    result = Multiplication::Builder(prefixFactor, result)
                 .shallowReduce(reductionContext);
  }
  replaceWithInPlace(result);
  return result;
}

OExpression OUnit::shallowBeautify() {
  // Force Float(1) in front of an orphan OUnit
  if (parent().isUninitialized() ||
      parent().otype() == ExpressionNode::Type::Opposite) {
    Multiplication m = Multiplication::Builder(Float<double>::Builder(1.));
    replaceWithInPlace(m);
    m.addChildAtIndexInPlace(*this, 1, 1);
    return std::move(m);
  }
  return *this;
}

OExpression OUnit::removeUnit(OExpression* unit) {
  *unit = *this;
  OExpression one = Rational::Builder(1);
  replaceWithInPlace(one);
  return one;
}

void OUnit::chooseBestRepresentativeAndPrefix(
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
  const Prefix* bestPrefix = (optimizePrefix) ? Prefix::EmptyPrefix() : nullptr;
  const Representative* bestRepresentative =
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

template Evaluation<float> UnitNode::templatedApproximate<float>(
    const ApproximationContext& approximationContext) const;
template Evaluation<double> UnitNode::templatedApproximate<double>(
    const ApproximationContext& approximationContext) const;

}  // namespace Poincare
