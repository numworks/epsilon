#include <poincare/unit.h>
#include <poincare/addition.h>
#include <poincare/division.h>
#include <poincare/float.h>
#include <poincare/layout_helper.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>
#include <poincare/rational.h>
#include <poincare/undefined.h>
#include <algorithm>
#include <assert.h>
#include <limits.h>
#include <utility>

namespace Poincare {

constexpr const UnitNode::Prefix Unit::k_prefixes[];
constexpr const UnitNode::TimeRepresentative Unit::k_timeRepresentatives[];
constexpr const UnitNode::DistanceRepresentative Unit::k_distanceRepresentatives[];
constexpr const UnitNode::AngleRepresentative Unit::k_angleRepresentatives[];
constexpr const UnitNode::MassRepresentative Unit::k_massRepresentatives[];
constexpr const UnitNode::CurrentRepresentative Unit::k_currentRepresentatives[];
constexpr const UnitNode::TemperatureRepresentative Unit::k_temperatureRepresentatives[];
constexpr const UnitNode::AmountOfSubstanceRepresentative Unit::k_amountOfSubstanceRepresentatives[];
constexpr const UnitNode::LuminousIntensityRepresentative Unit::k_luminousIntensityRepresentatives[];
constexpr const UnitNode::FrequencyRepresentative Unit::k_frequencyRepresentatives[];
constexpr const UnitNode::ForceRepresentative Unit::k_forceRepresentatives[];
constexpr const UnitNode::PressureRepresentative Unit::k_pressureRepresentatives[];
constexpr const UnitNode::EnergyRepresentative Unit::k_energyRepresentatives[];
constexpr const UnitNode::PowerRepresentative Unit::k_powerRepresentatives[];
constexpr const UnitNode::ElectricChargeRepresentative Unit::k_electricChargeRepresentatives[];
constexpr const UnitNode::ElectricPotentialRepresentative Unit::k_electricPotentialRepresentatives[];
constexpr const UnitNode::ElectricCapacitanceRepresentative Unit::k_electricCapacitanceRepresentatives[];
constexpr const UnitNode::ElectricResistanceRepresentative Unit::k_electricResistanceRepresentatives[];
constexpr const UnitNode::ElectricConductanceRepresentative Unit::k_electricConductanceRepresentatives[];
constexpr const UnitNode::MagneticFluxRepresentative Unit::k_magneticFluxRepresentatives[];
constexpr const UnitNode::MagneticFieldRepresentative Unit::k_magneticFieldRepresentatives[];
constexpr const UnitNode::InductanceRepresentative Unit::k_inductanceRepresentatives[];
constexpr const UnitNode::CatalyticActivityRepresentative Unit::k_catalyticActivityRepresentatives[];
constexpr const UnitNode::SurfaceRepresentative Unit::k_surfaceRepresentatives[];
constexpr const UnitNode::VolumeRepresentative Unit::k_volumeRepresentatives[];

constexpr const int
  Unit::k_emptyPrefixIndex,
  Unit::k_kiloPrefixIndex,
  Unit::k_secondRepresentativeIndex,
  Unit::k_minuteRepresentativeIndex,
  Unit::k_hourRepresentativeIndex,
  Unit::k_dayRepresentativeIndex,
  Unit::k_monthRepresentativeIndex,
  Unit::k_yearRepresentativeIndex,
  Unit::k_meterRepresentativeIndex,
  Unit::k_inchRepresentativeIndex,
  Unit::k_footRepresentativeIndex,
  Unit::k_yardRepresentativeIndex,
  Unit::k_mileRepresentativeIndex,
  Unit::k_radianRepresentativeIndex,
  Unit::k_degreeRepresentativeIndex,
  Unit::k_arcMinuteRepresentativeIndex,
  Unit::k_arcSecondRepresentativeIndex,
  Unit::k_gradianRepresentativeIndex,
  Unit::k_ounceRepresentativeIndex,
  Unit::k_poundRepresentativeIndex,
  Unit::k_shortTonRepresentativeIndex,
  Unit::k_kelvinRepresentativeIndex,
  Unit::k_celsiusRepresentativeIndex,
  Unit::k_fahrenheitRepresentativeIndex,
  Unit::k_jouleRepresentativeIndex,
  Unit::k_electronVoltRepresentativeIndex,
  Unit::k_wattRepresentativeIndex,
  Unit::k_hectareRepresentativeIndex,
  Unit::k_acreRepresentativeIndex,
  Unit::k_literRepresentativeIndex,
  Unit::k_cupRepresentativeIndex,
  Unit::k_pintRepresentativeIndex,
  Unit::k_quartRepresentativeIndex,
  Unit::k_gallonRepresentativeIndex;

// UnitNode::Prefix
const UnitNode::Prefix * UnitNode::Prefix::Prefixes() {
  return Unit::k_prefixes;
}

const UnitNode::Prefix * UnitNode::Prefix::EmptyPrefix() {
  return Prefixes() + Unit::k_emptyPrefixIndex;
}

// UnitNode::Vector
template<>
size_t UnitNode::Vector<int>::supportSize() const {
  size_t supportSize = 0;
  for (int i = 0; i < k_numberOfBaseUnits; i++) {
    if (coefficientAtIndex(i) == 0) {
      continue;
    }
    supportSize++;
  }
  return supportSize;
}

template<>
void UnitNode::Vector<int>::addAllCoefficients(const Vector<int> other, int factor) {
  for (int i = 0; i < UnitNode::k_numberOfBaseUnits; i++) {
    setCoefficientAtIndex(i, coefficientAtIndex(i) + other.coefficientAtIndex(i) * factor);
  }
}

template<>
UnitNode::Vector<int> UnitNode::Vector<int>::FromBaseUnits(const Expression baseUnits) {
  /* Returns the vector of Base units with integer exponents. If rational, the
   * closest integer will be used. */
  Vector<int> nullVector = {
    .time               = 0,
    .distance           = 0,
    .angle              = 0,
    .mass               = 0,
    .current            = 0,
    .temperature        = 0,
    .amountOfSubstance  = 0,
    .luminuousIntensity = 0,
  };
  Vector<int> vector = nullVector;
  int numberOfFactors;
  int factorIndex = 0;
  Expression factor;
  if (baseUnits.type() == ExpressionNode::Type::Multiplication) {
    numberOfFactors = baseUnits.numberOfChildren();
    factor = baseUnits.childAtIndex(0);
  } else {
    numberOfFactors = 1;
    factor = baseUnits;
  }
  do {
    // Get the unit's exponent
    int exponent = 1;
    if (factor.type() == ExpressionNode::Type::Power) {
      Expression exp = factor.childAtIndex(1);
      assert(exp.type() == ExpressionNode::Type::Rational);
      // Using the closest integer to the exponent.
      float exponentFloat = static_cast<const Rational &>(exp).node()->templatedApproximate<float>();
      if (exponentFloat != std::round(exponentFloat)) {
        /* If non-integer exponents are found, we round a null vector so that
         * Multiplication::shallowBeautify will not attempt to find derived
         * units. */
        return nullVector;
      }
      /* We limit to INT_MAX / 3 because an exponent might get bigger with
       * simplification. As a worst case scenario, (_s²_m²_kg/_A²)^n should be
       * simplified to (_s^5_S)^n. If 2*n is under INT_MAX, 5*n might not. */
      if (std::fabs(exponentFloat) < INT_MAX / 3) {
        // Exponent can be safely casted as int
        exponent = static_cast<int>(std::round(exponentFloat));
        assert(std::fabs(exponentFloat - static_cast<float>(exponent)) <= 0.5f);
      } else {
        /* Base units vector will ignore this coefficient, to avoid exponent
         * overflow. In any way, shallowBeautify will conserve homogeneity. */
        exponent = 0;
      }
      factor = factor.childAtIndex(0);
    }
    // Fill the vector with the unit's exponent
    assert(factor.type() == ExpressionNode::Type::Unit);
    vector.addAllCoefficients(static_cast<Unit &>(factor).node()->representative()->dimensionVector(), exponent);
    if (++factorIndex >= numberOfFactors) {
      break;
    }
    factor = baseUnits.childAtIndex(factorIndex);
  } while (true);
  return vector;
}

template<>
Expression UnitNode::Vector<int>::toBaseUnits() const {
  Expression result = Multiplication::Builder();
  int numberOfChildren = 0;
  for (int i = 0; i < k_numberOfBaseUnits; i++) {
    // We require the base units to be the first seven in DefaultRepresentatives
    const Representative * representative = Representative::DefaultRepresentatives()[i];
    assert(representative);
    const Prefix * prefix = representative->basePrefix();
    int exponent = coefficientAtIndex(i);
    Expression e;
    if (exponent == 0) {
      continue;
    }
    if (exponent == 1) {
      e = Unit::Builder(representative, prefix);
    } else {
      e = Power::Builder(Unit::Builder(representative, prefix), Rational::Builder(exponent));
    }
    static_cast<Multiplication &>(result).addChildAtIndexInPlace(e, numberOfChildren, numberOfChildren);
    numberOfChildren++;
  }
  assert(numberOfChildren > 0);
  result = static_cast<Multiplication &>(result).squashUnaryHierarchyInPlace();
  return result;
}

// UnitNode::Representative
const UnitNode::Representative * const * UnitNode::Representative::DefaultRepresentatives() {
  constexpr static SpeedRepresentative defaultSpeedRepresentative = SpeedRepresentative::Default();
  constexpr static const Representative * defaultRepresentatives[k_numberOfDimensions] = {
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

const UnitNode::Representative * UnitNode::Representative::RepresentativeForDimension(UnitNode::Vector<int> vector) {
  for (int i = 0; i < k_numberOfDimensions; i++) {
    const Representative * representative = Representative::DefaultRepresentatives()[i];
    if (vector == representative->dimensionVector()) {
      return representative;
    }
  }
  return nullptr;
}

static bool compareMagnitudeOrders(float order, float otherOrder) {
  /* Precision can be lost (with a year conversion for instance), so the order
   * value is rounded */
  if (std::fabs(order) < Float<float>::EpsilonLax()) {
    order = 0.0f;
  }
  if (std::fabs(otherOrder) < Float<float>::EpsilonLax()) {
    otherOrder = 0.0f;
  }
  if (std::fabs(std::fabs(order) - std::fabs(otherOrder)) <= 3.0f + Float<float>::EpsilonLax() && order * otherOrder < 0.0f) {
    /* If the two values are close, and their sign are opposed, the positive
     * order is preferred */
    return (order >= 0.0f);
  }
  // Otherwise, the closest order to 0 is preferred
  return (std::fabs(order) < std::fabs(otherOrder));
}

int UnitNode::Prefix::serialize(char * buffer, int bufferSize) const {
  assert(bufferSize >= 0);
  return std::min<int>(strlcpy(buffer, m_symbol, bufferSize), bufferSize - 1);
}

const UnitNode::Representative * UnitNode::Representative::DefaultFindBestRepresentative(double value, double exponent, const UnitNode::Representative * representatives, int length, const Prefix * * prefix) {
  assert(length >= 1);
  const Representative * res = representatives;
  double acc = value / std::pow(res->ratio(), exponent);
  if (*prefix) {
    *prefix = res->findBestPrefix(acc, exponent);
  }
  if (length == 1) {
    return res;
  }
  const Prefix * pre = Prefix::EmptyPrefix();
  const Representative * iter = res + 1;
  while (iter < representatives + length) {
    double temp = value / std::pow(iter->ratio(), exponent);
    if (*prefix) {
      pre = iter->findBestPrefix(temp, exponent);
    }
    if (compareMagnitudeOrders(std::log10(temp) - pre->exponent() * exponent, std::log10(acc) - ((!*prefix) ? 0 : (*prefix)->exponent() * exponent))) {
      acc = temp;
      res = iter;
      *prefix = pre;
    }
    iter++;
  }
  if (!*prefix) {
    *prefix = res->basePrefix();
  }
  return res;
}

int UnitNode::Representative::serialize(char * buffer, int bufferSize, const Prefix * prefix) const {
  int length = 0;
  length += prefix->serialize(buffer, bufferSize);
  assert(length == 0 || isInputPrefixable());
  assert(length < bufferSize);
  buffer += length;
  bufferSize -= length;
  assert(bufferSize >= 0);
  length += std::min<int>(strlcpy(buffer, m_rootSymbols.mainAlias(), bufferSize), bufferSize - 1);
  return length;
}

bool UnitNode::Representative::canParseWithEquivalents(const char * symbol, size_t length, const Representative * * representative, const Prefix * * prefix) const {
  const Representative * candidate = representativesOfSameDimension();
  if (!candidate) {
    return false;
  }
  for (int i = 0; i < numberOfRepresentatives(); i++) {
    AliasesList rootSymbolAliasesList = (candidate + i)->rootSymbols();
    for (const char * rootSymbolAlias : rootSymbolAliasesList) {
      size_t rootSymbolLength = strlen(rootSymbolAlias);
      int potentialPrefixLength = length - rootSymbolLength;
      if (potentialPrefixLength >= 0
      && strncmp(rootSymbolAlias, symbol + potentialPrefixLength, rootSymbolLength) == 0
      && candidate[i].canParse(symbol, potentialPrefixLength, prefix)) {
        if (representative) {
          *representative = (candidate + i);
        }
        return true;
      }
    }
  }
  return false;
}

bool UnitNode::Representative::canParse(const char * symbol, size_t length, const Prefix * * prefix) const {
  if (!isInputPrefixable()) {
    if (prefix) {
      *prefix = Prefix::EmptyPrefix();
    }
    return length == 0;
  }
  for (size_t i = 0; i < Prefix::k_numberOfPrefixes; i++) {
    const Prefix * pre = Prefix::Prefixes() + i;
    const char * prefixSymbol = pre->symbol();
    if (strlen(prefixSymbol) == length
     && canPrefix(pre, true)
     && strncmp(symbol, prefixSymbol, length) == 0)
    {
      if (prefix) {
        *prefix = pre;
      }
      return true;
    }
  }
  return false;
}

Expression UnitNode::Representative::toBaseUnits(const ExpressionNode::ReductionContext& reductionContext) const {
  Expression result;
  if (isBaseUnit()) {
    result =  Unit::Builder(this, basePrefix());
  } else {
    result = dimensionVector().toBaseUnits();
  }
  Rational basePrefixFactor = Rational::IntegerPower(Rational::Builder(10), -basePrefix()->exponent());
  Expression factor = Multiplication::Builder(basePrefixFactor, ratioExpressionReduced(reductionContext)).shallowReduce(reductionContext);
  return Multiplication::Builder(factor, result);

}

bool UnitNode::Representative::canPrefix(const UnitNode::Prefix * prefix, bool input) const {
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

const UnitNode::Prefix * UnitNode::Representative::findBestPrefix(double value, double exponent) const {
  if (!isOutputPrefixable()) {
    return Prefix::EmptyPrefix();
  }
  if (value < Float<double>::EpsilonLax()) {
    return basePrefix();
  }
  const Prefix * res = basePrefix();
  const float magnitude = std::log10(std::fabs(value));
  float bestOrder = magnitude;
  for (int i = 0; i < Prefix::k_numberOfPrefixes; i++) {
    if (!canPrefix(Prefix::Prefixes() + i, false)) {
      continue;
    }
    float order = magnitude - (Prefix::Prefixes()[i].exponent() - basePrefix()->exponent()) * exponent;
    if (compareMagnitudeOrders(order, bestOrder)) {
      bestOrder = order;
      res = Prefix::Prefixes() + i;
    }
  }
  return res;
}

// UnitNode::___Representative
const UnitNode::Representative * UnitNode::TimeRepresentative::representativesOfSameDimension() const { return Unit::k_timeRepresentatives; }
const UnitNode::Representative * UnitNode::DistanceRepresentative::representativesOfSameDimension() const { return Unit::k_distanceRepresentatives; }
const UnitNode::Representative * UnitNode::AngleRepresentative::representativesOfSameDimension() const { return Unit::k_angleRepresentatives; }
const UnitNode::Representative * UnitNode::MassRepresentative::representativesOfSameDimension() const { return Unit::k_massRepresentatives; }
const UnitNode::Representative * UnitNode::CurrentRepresentative::representativesOfSameDimension() const { return Unit::k_currentRepresentatives; }
const UnitNode::Representative * UnitNode::TemperatureRepresentative::representativesOfSameDimension() const { return Unit::k_temperatureRepresentatives; }
const UnitNode::Representative * UnitNode::AmountOfSubstanceRepresentative::representativesOfSameDimension() const { return Unit::k_amountOfSubstanceRepresentatives; }
const UnitNode::Representative * UnitNode::LuminousIntensityRepresentative::representativesOfSameDimension() const { return Unit::k_luminousIntensityRepresentatives; }
const UnitNode::Representative * UnitNode::FrequencyRepresentative::representativesOfSameDimension() const { return Unit::k_frequencyRepresentatives; }
const UnitNode::Representative * UnitNode::ForceRepresentative::representativesOfSameDimension() const { return Unit::k_forceRepresentatives; }
const UnitNode::Representative * UnitNode::PressureRepresentative::representativesOfSameDimension() const { return Unit::k_pressureRepresentatives; }
const UnitNode::Representative * UnitNode::EnergyRepresentative::representativesOfSameDimension() const { return Unit::k_energyRepresentatives; }
const UnitNode::Representative * UnitNode::PowerRepresentative::representativesOfSameDimension() const { return Unit::k_powerRepresentatives; }
const UnitNode::Representative * UnitNode::ElectricChargeRepresentative::representativesOfSameDimension() const { return Unit::k_electricChargeRepresentatives; }
const UnitNode::Representative * UnitNode::ElectricPotentialRepresentative::representativesOfSameDimension() const { return Unit::k_electricPotentialRepresentatives; }
const UnitNode::Representative * UnitNode::ElectricCapacitanceRepresentative::representativesOfSameDimension() const { return Unit::k_electricCapacitanceRepresentatives; }
const UnitNode::Representative * UnitNode::ElectricResistanceRepresentative::representativesOfSameDimension() const { return Unit::k_electricResistanceRepresentatives; }
const UnitNode::Representative * UnitNode::ElectricConductanceRepresentative::representativesOfSameDimension() const { return Unit::k_electricConductanceRepresentatives; }
const UnitNode::Representative * UnitNode::MagneticFluxRepresentative::representativesOfSameDimension() const { return Unit::k_magneticFluxRepresentatives; }
const UnitNode::Representative * UnitNode::MagneticFieldRepresentative::representativesOfSameDimension() const { return Unit::k_magneticFieldRepresentatives; }
const UnitNode::Representative * UnitNode::InductanceRepresentative::representativesOfSameDimension() const { return Unit::k_inductanceRepresentatives; }
const UnitNode::Representative * UnitNode::CatalyticActivityRepresentative::representativesOfSameDimension() const { return Unit::k_catalyticActivityRepresentatives; }
const UnitNode::Representative * UnitNode::SurfaceRepresentative::representativesOfSameDimension() const { return Unit::k_surfaceRepresentatives; }
const UnitNode::Representative * UnitNode::VolumeRepresentative::representativesOfSameDimension() const { return Unit::k_volumeRepresentatives; }

int UnitNode::TimeRepresentative::setAdditionalExpressions(double value, Expression * dest, int availableLength, const ExpressionNode::ReductionContext& reductionContext) const {
  assert(availableLength >= 1);
  /* Use all representatives but week */
  const Unit splitUnits[] = {
    Unit::Builder(representativesOfSameDimension() + Unit::k_secondRepresentativeIndex, Prefix::EmptyPrefix()),
    Unit::Builder(representativesOfSameDimension() + Unit::k_minuteRepresentativeIndex, Prefix::EmptyPrefix()),
    Unit::Builder(representativesOfSameDimension() + Unit::k_hourRepresentativeIndex, Prefix::EmptyPrefix()),
    Unit::Builder(representativesOfSameDimension() + Unit::k_dayRepresentativeIndex, Prefix::EmptyPrefix()),
    Unit::Builder(representativesOfSameDimension() + Unit::k_monthRepresentativeIndex, Prefix::EmptyPrefix()),
    Unit::Builder(representativesOfSameDimension() + Unit::k_yearRepresentativeIndex, Prefix::EmptyPrefix()),
  };
  dest[0] = Unit::BuildSplit(value, splitUnits, numberOfRepresentatives() - 1, reductionContext);
  return 1;
}

const UnitNode::Representative * UnitNode::DistanceRepresentative::standardRepresentative(double value, double exponent, const ExpressionNode::ReductionContext& reductionContext, const Prefix * * prefix) const {
  return (reductionContext.unitFormat() == Preferences::UnitFormat::Metric) ?
    /* Exclude imperial units from the search. */
    DefaultFindBestRepresentative(value, exponent, representativesOfSameDimension(), Unit::k_inchRepresentativeIndex, prefix) :
    /* Exclude m form the search. */
    DefaultFindBestRepresentative(value, exponent, representativesOfSameDimension() + 1, numberOfRepresentatives() - 1, prefix);
}

int UnitNode::DistanceRepresentative::setAdditionalExpressions(double value, Expression * dest, int availableLength, const ExpressionNode::ReductionContext& reductionContext) const {
  assert(availableLength >= 1);
  if (reductionContext.unitFormat() == Preferences::UnitFormat::Metric) {
    return 0;
  }
  const Unit splitUnits[] = {
    Unit::Builder(representativesOfSameDimension() + Unit::k_inchRepresentativeIndex, Prefix::EmptyPrefix()),
    Unit::Builder(representativesOfSameDimension() + Unit::k_footRepresentativeIndex, Prefix::EmptyPrefix()),
    Unit::Builder(representativesOfSameDimension() + Unit::k_yardRepresentativeIndex, Prefix::EmptyPrefix()),
    Unit::Builder(representativesOfSameDimension() + Unit::k_mileRepresentativeIndex, Prefix::EmptyPrefix()),
  };
  dest[0] = Unit::BuildSplit(value, splitUnits, sizeof(splitUnits)/sizeof(Unit), reductionContext);
  return 1;
}

const UnitNode::Representative * UnitNode::AngleRepresentative::DefaultRepresentative(const ExpressionNode::ReductionContext& reductionContext) {
  if (reductionContext.angleUnit() == Poincare::Preferences::AngleUnit::Radian) {
    return &Unit::k_angleRepresentatives[Unit::k_radianRepresentativeIndex];
  }
  if (reductionContext.angleUnit() == Poincare::Preferences::AngleUnit::Gradian) {
    return &Unit::k_angleRepresentatives[Unit::k_gradianRepresentativeIndex];
  }
  assert(reductionContext.angleUnit() == Poincare::Preferences::AngleUnit::Degree);
  return &Unit::k_angleRepresentatives[Unit::k_degreeRepresentativeIndex];
}

const UnitNode::Representative * UnitNode::AngleRepresentative::standardRepresentative(double value, double exponent, const ExpressionNode::ReductionContext& reductionContext, const Prefix * * prefix) const {
  if (reductionContext.angleUnit() == Poincare::Preferences::AngleUnit::Degree) {
    return DefaultFindBestRepresentative(value, exponent, representativesOfSameDimension() + Unit::k_arcSecondRepresentativeIndex, 3, prefix);
  }
  return DefaultRepresentative(reductionContext);
}

Expression UnitNode::AngleRepresentative::convertInto(Expression value, const UnitNode::Representative * other , const ExpressionNode::ReductionContext& reductionContext) const {
  assert(dimensionVector() == other->dimensionVector());
  Expression unit = Unit::Builder(other, Prefix::EmptyPrefix());
  Expression inRadians = Multiplication::Builder(value, ratioExpressionReduced(reductionContext)).shallowReduce(reductionContext);
  Expression inOther = Division::Builder(inRadians, other->ratioExpressionReduced(reductionContext)).shallowReduce(reductionContext).deepBeautify(reductionContext);
  return Multiplication::Builder(inOther, unit);
}

int UnitNode::AngleRepresentative::setAdditionalExpressionsWithExactValue(Expression exactValue, double value, Expression * dest, int availableLength, const ExpressionNode::ReductionContext& reductionContext) const {
  assert(availableLength >= 2);
  int numberOfResults = 0;
  // Conversion to degrees should be added to all units not degree related
  if (this == representativesOfSameDimension() + Unit::k_radianRepresentativeIndex || this == representativesOfSameDimension() + Unit::k_gradianRepresentativeIndex) {
    const Representative * degree = representativesOfSameDimension() + Unit::k_degreeRepresentativeIndex;
    dest[numberOfResults++] = convertInto(exactValue.clone(), degree, reductionContext).approximateKeepingUnits<double>(reductionContext);
  }
  // Degrees related units should show their decomposition in DMS
  const Unit splitUnits[] = {
    Unit::Builder(representativesOfSameDimension() + Unit::k_arcSecondRepresentativeIndex, Prefix::EmptyPrefix()),
    Unit::Builder(representativesOfSameDimension() + Unit::k_arcMinuteRepresentativeIndex, Prefix::EmptyPrefix()),
    Unit::Builder(representativesOfSameDimension() + Unit::k_degreeRepresentativeIndex, Prefix::EmptyPrefix()),
  };
  Expression split = Unit::BuildSplit(value, splitUnits, sizeof(splitUnits)/sizeof(Unit), reductionContext);
  if (!split.isUndefined()) {
    dest[numberOfResults++] = split;
  }
  // Conversion to radians should be added to all other units.
  if (this != representativesOfSameDimension() + Unit::k_radianRepresentativeIndex) {
    const Representative * radian = representativesOfSameDimension() + Unit::k_radianRepresentativeIndex;
    dest[numberOfResults++] = convertInto(exactValue, radian, reductionContext);
  }
  return numberOfResults;
}

const UnitNode::Prefix * UnitNode::MassRepresentative::basePrefix() const {
  return isBaseUnit() ? Prefix::Prefixes() + Unit::k_kiloPrefixIndex : Prefix::EmptyPrefix();
}

const UnitNode::Representative * UnitNode::MassRepresentative::standardRepresentative(double value, double exponent, const ExpressionNode::ReductionContext& reductionContext, const Prefix * * prefix) const {
  if (reductionContext.unitFormat() == Preferences::UnitFormat::Imperial) {
    return DefaultFindBestRepresentative(value, exponent, representativesOfSameDimension() + Unit::k_ounceRepresentativeIndex, Unit::k_shortTonRepresentativeIndex - Unit::k_ounceRepresentativeIndex + 1, prefix);
  }
  assert(reductionContext.unitFormat() == Preferences::UnitFormat::Metric);
  bool useTon = exponent == 1. && value >= (representativesOfSameDimension() + Unit::k_tonRepresentativeIndex)->ratio();
  int representativeIndex = useTon ? Unit::k_tonRepresentativeIndex : Unit::k_gramRepresentativeIndex;
  return DefaultFindBestRepresentative(value, exponent, representativesOfSameDimension() + representativeIndex, 1, prefix);
}

int UnitNode::MassRepresentative::setAdditionalExpressions(double value, Expression * dest, int availableLength, const ExpressionNode::ReductionContext& reductionContext) const {
  assert(availableLength >= 1);
  if (reductionContext.unitFormat() == Preferences::UnitFormat::Metric) {
    return 0;
  }
  const Unit splitUnits[] = {
    Unit::Builder(representativesOfSameDimension() + Unit::k_ounceRepresentativeIndex, Prefix::EmptyPrefix()),
    Unit::Builder(representativesOfSameDimension() + Unit::k_poundRepresentativeIndex, Prefix::EmptyPrefix()),
    Unit::Builder(representativesOfSameDimension() + Unit::k_shortTonRepresentativeIndex, Prefix::EmptyPrefix()),
  };
  dest[0] = Unit::BuildSplit(value, splitUnits, sizeof(splitUnits)/sizeof(Unit), reductionContext);
  return 1;
}

double UnitNode::TemperatureRepresentative::ConvertTemperatures(double value, const  Representative * source, const Representative * target) {
  assert(source->dimensionVector() == TemperatureRepresentative::Default().dimensionVector());
  assert(target->dimensionVector() == TemperatureRepresentative::Default().dimensionVector());
  if (source == target) {
    return value;
  }
  constexpr double origin[] = {0, k_celsiusOrigin, k_fahrenheitOrigin};
  assert(sizeof(origin) == source->numberOfRepresentatives() * sizeof(double));
  double sourceOrigin = origin[source - source->representativesOfSameDimension()];
  double targetOrigin = origin[target - target->representativesOfSameDimension()];
  /* (T + origin) * ration converts T to Kelvin.
   * T/ratio - origin converts T from Kelvin. */
  return (value + sourceOrigin) * source->ratio() / target->ratio() - targetOrigin;
}

int UnitNode::TemperatureRepresentative::setAdditionalExpressions(double value, Expression * dest, int availableLength, const ExpressionNode::ReductionContext& reductionContext) const {
  assert(availableLength >= 2);
  const Representative * celsius = TemperatureRepresentative::Default().representativesOfSameDimension() + Unit::k_celsiusRepresentativeIndex;
  const Representative * fahrenheit = TemperatureRepresentative::Default().representativesOfSameDimension() + Unit::k_fahrenheitRepresentativeIndex;
  const Representative * kelvin = TemperatureRepresentative::Default().representativesOfSameDimension() + Unit::k_kelvinRepresentativeIndex;
  const Representative * targets[] =  {
    reductionContext.unitFormat() == Preferences::UnitFormat::Metric ? celsius : fahrenheit,
    reductionContext.unitFormat() == Preferences::UnitFormat::Metric ? fahrenheit : celsius,
    kelvin};
  int numberOfExpressionsSet = 0;
  int numberOfTargets = sizeof(targets) / sizeof(Representative *);
  for (int i = 0; i < numberOfTargets; i++) {
    if (targets[i] == this) {
      continue;
    }
    dest[numberOfExpressionsSet++] = Multiplication::Builder(
        Float<double>::Builder(TemperatureRepresentative::ConvertTemperatures(value, this, targets[i])),
        Unit::Builder(targets[i], Prefix::EmptyPrefix()));
  }
  assert(numberOfExpressionsSet == 2);
  return numberOfExpressionsSet;
}

int UnitNode::EnergyRepresentative::setAdditionalExpressions(double value, Expression * dest, int availableLength, const ExpressionNode::ReductionContext& reductionContext) const {
  assert(availableLength >= 2);
  int index = 0;
  /* 1. Convert into Joules
   * As J is just a shorthand for _kg_m^2_s^-2, the value is used as is. */
  const Representative * joule = representativesOfSameDimension() + Unit::k_jouleRepresentativeIndex;
  const Prefix * joulePrefix = joule->findBestPrefix(value, 1.);
  dest[index++] = Multiplication::Builder(Float<double>::Builder(value * std::pow(10., -joulePrefix->exponent())), Unit::Builder(joule, joulePrefix));
  /* 2. Convert into Wh
   * As value is expressed in SI units (ie _kg_m^2_s^-2), the ratio is that of
   * hours to seconds. */
  const Representative * hour = TimeRepresentative::Default().representativesOfSameDimension() + Unit::k_hourRepresentativeIndex;
  const Representative * watt = PowerRepresentative::Default().representativesOfSameDimension() + Unit::k_wattRepresentativeIndex;
  double adjustedValue = value / hour->ratio() / watt->ratio();
  const Prefix * wattPrefix = watt->findBestPrefix(adjustedValue, 1.);
  dest[index++] = Multiplication::Builder(
      Float<double>::Builder(adjustedValue * std::pow(10., -wattPrefix->exponent())),
      Multiplication::Builder(
        Unit::Builder(watt, wattPrefix),
        Unit::Builder(hour, Prefix::EmptyPrefix())));
  /* 3. Convert into eV */
  const Representative * eV = representativesOfSameDimension() + Unit::k_electronVoltRepresentativeIndex;
  adjustedValue = value / eV->ratio();
  const Prefix * eVPrefix = eV->findBestPrefix(adjustedValue, 1.);
  dest[index++] = Multiplication::Builder(
      Float<double>::Builder(adjustedValue * std::pow(10., -eVPrefix->exponent())),
      Unit::Builder(eV, eVPrefix));
  return index;
}

const UnitNode::Representative * UnitNode::SurfaceRepresentative::standardRepresentative(double value, double exponent, const ExpressionNode::ReductionContext& reductionContext, const Prefix * * prefix) const {
  *prefix = Prefix::EmptyPrefix();
  return representativesOfSameDimension() + (reductionContext.unitFormat() == Preferences::UnitFormat::Metric ? Unit::k_hectareRepresentativeIndex : Unit::k_acreRepresentativeIndex);
}

int UnitNode::SurfaceRepresentative::setAdditionalExpressions(double value, Expression * dest, int availableLength, const ExpressionNode::ReductionContext& reductionContext) const {
  assert(availableLength >= 2);
  Expression * destMetric;
  Expression * destImperial = nullptr;
  if (reductionContext.unitFormat() == Preferences::UnitFormat::Metric) {
    destMetric = dest;
  } else {
    destImperial = dest;
    destMetric = dest + 1;
  }
  // 1. Convert to hectares
  const Representative * hectare = representativesOfSameDimension() + Unit::k_hectareRepresentativeIndex;
  *destMetric = Multiplication::Builder(Float<double>::Builder(value / hectare->ratio()), Unit::Builder(hectare, Prefix::EmptyPrefix()));
  // 2. Convert to acres
  if (!destImperial) {
    return 1;
  }
  const Representative * acre = representativesOfSameDimension() + Unit::k_acreRepresentativeIndex;
  *destImperial = Multiplication::Builder(Float<double>::Builder(value / acre->ratio()), Unit::Builder(acre, Prefix::EmptyPrefix()));
  return 2;
}

const UnitNode::Representative * UnitNode::VolumeRepresentative::standardRepresentative(double value, double exponent, const ExpressionNode::ReductionContext& reductionContext, const Prefix * * prefix) const {
  if (reductionContext.unitFormat() == Preferences::UnitFormat::Metric) {
    *prefix = representativesOfSameDimension()->findBestPrefix(value, exponent);
    return representativesOfSameDimension();
  }
  return DefaultFindBestRepresentative(value, exponent, representativesOfSameDimension() + 1, numberOfRepresentatives() - 1, prefix);
}

int UnitNode::VolumeRepresentative::setAdditionalExpressions(double value, Expression * dest, int availableLength, const ExpressionNode::ReductionContext& reductionContext) const {
  assert(availableLength >= 2);
  Expression * destMetric;
  Expression * destImperial = nullptr;
  if (reductionContext.unitFormat() == Preferences::UnitFormat::Metric) {
    destMetric = dest;
  } else {
    destImperial = dest;
    destMetric = dest + 1;
  }
  // 1. Convert to liters
  const Representative * liter = representativesOfSameDimension() + Unit::k_literRepresentativeIndex;
  double adjustedValue = value / liter->ratio();
  const Prefix * literPrefix = liter->findBestPrefix(adjustedValue, 1.);
  *destMetric = Multiplication::Builder(
      Float<double>::Builder(adjustedValue * pow(10., -literPrefix->exponent())),
      Unit::Builder(liter, literPrefix));
  // 2. Convert to imperial volumes
  if (!destImperial) {
    return 1;
  }
  const Unit splitUnits[] = {
    Unit::Builder(representativesOfSameDimension() + Unit::k_cupRepresentativeIndex, Prefix::EmptyPrefix()),
    Unit::Builder(representativesOfSameDimension() + Unit::k_pintRepresentativeIndex, Prefix::EmptyPrefix()),
    Unit::Builder(representativesOfSameDimension() + Unit::k_quartRepresentativeIndex, Prefix::EmptyPrefix()),
    Unit::Builder(representativesOfSameDimension() + Unit::k_gallonRepresentativeIndex, Prefix::EmptyPrefix()),
  };
  *destImperial = Unit::BuildSplit(value, splitUnits, sizeof(splitUnits)/sizeof(Unit), reductionContext);
  return 2;
}

int UnitNode::SpeedRepresentative::setAdditionalExpressions(double value, Expression * dest, int availableLength, const ExpressionNode::ReductionContext& reductionContext) const {
  assert(availableLength >= 2);
  Expression * destMetric;
  Expression * destImperial = nullptr;
  if (reductionContext.unitFormat() == Preferences::UnitFormat::Metric) {
    destMetric = dest;
  } else {
    destImperial = dest;
    destMetric = dest + 1;
  }
  // 1. Convert to km/h
  const Representative * meter = DistanceRepresentative::Default().representativesOfSameDimension() + Unit::k_meterRepresentativeIndex;
  const Representative * hour = TimeRepresentative::Default().representativesOfSameDimension() + Unit::k_hourRepresentativeIndex;
  *destMetric = Multiplication::Builder(
      Float<double>::Builder(value / 1000. * hour->ratio()),
      Multiplication::Builder(
        Unit::Builder(meter, Prefix::Prefixes() + Unit::k_kiloPrefixIndex),
        Power::Builder(Unit::Builder(hour, Prefix::EmptyPrefix()), Rational::Builder(-1))));
  // 2. Convert to mph
  if (!destImperial) {
    return 1;
  }
  const Representative * mile = DistanceRepresentative::Default().representativesOfSameDimension() + Unit::k_mileRepresentativeIndex;
  *destImperial = Multiplication::Builder(
      Float<double>::Builder(value / mile->ratio() * hour->ratio()),
      Multiplication::Builder(
        Unit::Builder(mile, Prefix::EmptyPrefix()),
        Power::Builder(Unit::Builder(hour, Prefix::EmptyPrefix()), Rational::Builder(-1))));
  return 2;
}

// UnitNode
Expression UnitNode::removeUnit(Expression * unit) {
  return Unit(this).removeUnit(unit);
}

Layout UnitNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  /* TODO: compute the bufferSize more precisely... So far the longest unit is
   * "month" of size 6 but later, we might add unicode to represent ohm or µ
   * which would change the required size?*/
  constexpr static size_t bufferSize = 10;
  char buffer[bufferSize];
  char * string = buffer;
  int stringLen = serialize(buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits);
  if (!context
     || (context->canRemoveUnderscoreToUnits()
         && context->expressionTypeForIdentifier(buffer+1, strlen(buffer)-1) == Context::SymbolAbstractType::None)) {
    // If the unit is not a defined variable, do not display the '_'
    assert(string[0] == '_');
    string++;
    stringLen--;
  }
  return LayoutHelper::StringToStringLayout(string, stringLen);
}

int UnitNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  assert(bufferSize >= 0);
  int underscoreLength = std::min<int>(strlcpy(buffer, "_", bufferSize), bufferSize - 1);
  buffer += underscoreLength;
  bufferSize -= underscoreLength;
  return underscoreLength + m_representative->serialize(buffer, bufferSize, m_prefix);
}

int UnitNode::simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool ignoreParentheses) const {
  if (!ascending) {
    return e->simplificationOrderSameType(this, true, ignoreParentheses);
  }
  assert(type() == e->type());
  const UnitNode * eNode = static_cast<const UnitNode *>(e);
  Vector<int> v = representative()->dimensionVector();
  Vector<int> w = eNode->representative()->dimensionVector();
  for (int i = 0; i < k_numberOfBaseUnits; i++) {
    if (v.coefficientAtIndex(i) != w.coefficientAtIndex(i)) {
      return v.coefficientAtIndex(i) - w.coefficientAtIndex(i);
    }
  }
  const ptrdiff_t representativeDiff = m_representative - eNode->representative();
  if (representativeDiff != 0) {
    return representativeDiff;
  }
  const ptrdiff_t prediff = eNode->prefix()->exponent() - m_prefix->exponent();
  return prediff;
}

Expression UnitNode::shallowReduce(const ReductionContext& reductionContext) {
  return Unit(this).shallowReduce(reductionContext);
}

Expression UnitNode::shallowBeautify(const ReductionContext& reductionContext) {
  return Unit(this).shallowBeautify();
}

template<typename T>
Evaluation<T> UnitNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  return Complex<T>::Undefined();
}

// Unit
Unit Unit::Builder(const Unit::Representative * representative, const Prefix * prefix) {
  void * bufferNode = TreePool::sharedPool()->alloc(sizeof(UnitNode));
  UnitNode * node = new (bufferNode) UnitNode(representative, prefix);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  return static_cast<Unit &>(h);
}

bool Unit::CanParse(const char * symbol, size_t length, const Unit::Representative * * representative, const Unit::Prefix * * prefix) {
  if (symbol[0] == '_') {
    symbol++;
    length--;
  }
  for (int i = 0; i < Representative::k_numberOfDimensions; i++) {
    if (Representative::DefaultRepresentatives()[i]->canParseWithEquivalents(symbol, length, representative, prefix)) {
      return true;
    }
  }
  return false;
}

static void chooseBestRepresentativeAndPrefixForValueOnSingleUnit(Expression unit, double * value, const ExpressionNode::ReductionContext& reductionContext, bool optimizePrefix) {
  double exponent = 1.f;
  Expression factor = unit;
  if (factor.type() == ExpressionNode::Type::Power) {
    Expression childExponent = factor.childAtIndex(1);
    assert(factor.childAtIndex(0).type() == ExpressionNode::Type::Unit);
    assert(factor.childAtIndex(1).type() == ExpressionNode::Type::Rational);
    exponent = static_cast<Rational &>(childExponent).approximateToScalar<double>(reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit());
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
  static_cast<Unit &>(factor).chooseBestRepresentativeAndPrefix(value, exponent, reductionContext, optimizePrefix);
}

void Unit::ChooseBestRepresentativeAndPrefixForValue(Expression units, double * value, const ExpressionNode::ReductionContext& reductionContext) {
  int numberOfFactors;
  Expression factor;
  if (units.type() == ExpressionNode::Type::Multiplication) {
    numberOfFactors = units.numberOfChildren();
    factor = units.childAtIndex(0);
  } else {
    numberOfFactors = 1;
    factor = units;
  }
  chooseBestRepresentativeAndPrefixForValueOnSingleUnit(factor, value, reductionContext, true);
  for (int i = 1; i < numberOfFactors; i++) {
    chooseBestRepresentativeAndPrefixForValueOnSingleUnit(units.childAtIndex(i), value, reductionContext, false);
  }
}

bool Unit::ShouldDisplayAdditionalOutputs(double value, Expression unit, Preferences::UnitFormat unitFormat) {
  if (unit.isUninitialized() || !std::isfinite(value)) {
    return false;
  }
  UnitNode::Vector<int> vector = UnitNode::Vector<int>::FromBaseUnits(unit);
  const Representative * representative = Representative::RepresentativeForDimension(vector);

  ExpressionTest isNonBase = [](const Expression e, Context * context) {
    return !e.isUninitialized() && e.type() == ExpressionNode::Type::Unit && !e.convert<Unit>().isBaseUnit();
  };

  return (representative != nullptr && representative->hasSpecialAdditionalExpressions(value, unitFormat))
         || unit.recursivelyMatches(isNonBase);
}

int Unit::SetAdditionalExpressions(Expression units, double value, Expression * dest, int availableLength, const ExpressionNode::ReductionContext& reductionContext, Expression exactOutput) {
  if (units.isUninitialized()) {
    return 0;
  }
  const Representative * representative = units.type() == ExpressionNode::Type::Unit ? static_cast<Unit &>(units).node()->representative() : UnitNode::Representative::RepresentativeForDimension(UnitNode::Vector<int>::FromBaseUnits(units));
  if (!representative) {
    return 0;
  }
  if (representative->dimensionVector() == AngleRepresentative::Default().dimensionVector()) {
    /* Angles are the only unit where we want to display the exact value. */
    Expression exactValue = exactOutput.clone();
    Expression unit;
    ExpressionNode::ReductionContext childContext = reductionContext;
    childContext.setUnitConversion(ExpressionNode::UnitConversion::None);
    exactValue = exactValue.reduceAndRemoveUnit(childContext, &unit);
    assert(unit.type() == ExpressionNode::Type::Unit);
    return static_cast<const AngleRepresentative*>(static_cast<Unit &>(unit).representative())->setAdditionalExpressionsWithExactValue(exactValue, value, dest, availableLength, reductionContext);
  }
  return representative->setAdditionalExpressions(value, dest, availableLength, reductionContext);
}

Expression Unit::BuildSplit(double value, const Unit * units, int length, const ExpressionNode::ReductionContext& reductionContext) {
  assert(!std::isnan(value));
  assert(units);
  assert(length > 0);

  double baseRatio = units->node()->representative()->ratio();
  double basedValue = value / baseRatio;
  // WARNING: Maybe this should be compared to 0.0 instead of EpsilonLax ? (see below)
  if (std::isinf(basedValue) || std::fabs(basedValue) < Float<double>::EpsilonLax()) {
    return Multiplication::Builder(Number::FloatNumber(basedValue), units[0]);
  }
  double err = std::pow(10.0, Poincare::PrintFloat::k_numberOfStoredSignificantDigits - 1 - std::ceil(log10(std::fabs(basedValue))));
  double remain = std::round(basedValue*err)/err;

  Addition res = Addition::Builder();
  for (int i = length - 1; i >= 0; i--) {
    assert(units[i].node()->prefix() == Prefix::EmptyPrefix());
    double factor = std::round(units[i].node()->representative()->ratio() / baseRatio);
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
      res.addChildAtIndexInPlace(Multiplication::Builder(Float<double>::Builder(share), units[i]), res.numberOfChildren(), res.numberOfChildren());
    }
  }
  assert(res.numberOfChildren() > 0);
  ExpressionNode::ReductionContext keepUnitsContext(
      reductionContext.context(),
      reductionContext.complexFormat(),
      reductionContext.angleUnit(),
      reductionContext.unitFormat(),
      ExpressionNode::ReductionTarget::User,
      ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition,
      ExpressionNode::UnitConversion::None);
  return res.squashUnaryHierarchyInPlace().shallowBeautify(keepUnitsContext);
}

Expression Unit::ConvertTemperatureUnits(Expression e, Unit unit, const ExpressionNode::ReductionContext& reductionContext) {
  const Representative * targetRepr = unit.representative();
  const Prefix * targetPrefix = unit.node()->prefix();
  assert(unit.representative()->dimensionVector() == TemperatureRepresentative::Default().dimensionVector());

  Expression startUnit;
  e = e.removeUnit(&startUnit);
  if (startUnit.isUninitialized() || startUnit.type() != ExpressionNode::Type::Unit) {
    return Undefined::Builder();
  }
  const Representative * startRepr = static_cast<Unit &>(startUnit).representative();
  if (startRepr->dimensionVector() != TemperatureRepresentative::Default().dimensionVector()) {
    return Undefined::Builder();
  }

  const Prefix * startPrefix = static_cast<Unit &>(startUnit).node()->prefix();
  double value = e.approximateToScalar<double>(reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit());
  return Multiplication::Builder(
      Float<double>::Builder(TemperatureRepresentative::ConvertTemperatures(value * std::pow(10., startPrefix->exponent()), startRepr, targetRepr) * std::pow(10., - targetPrefix->exponent())),
      unit.clone());
}

bool Unit::IsForbiddenTemperatureProduct(Expression e) {
  assert(e.type() == ExpressionNode::Type::Multiplication);
  if (e.numberOfChildren() != 2) {
    /* A multiplication cannot contain a °C or °F if it does not have 2
     * children, as otherwise the temperature would have reduced itself to
     * undef. */
    return false;
  }
  int temperatureChildIndex = -1;
  for (int i = 0; i < 2; i++) {
    Expression child = e.childAtIndex(i);
    if (child.type() == ExpressionNode::Type::Unit
     && (static_cast<Unit &>(child).node()->representative() == k_temperatureRepresentatives + k_celsiusRepresentativeIndex
      || static_cast<Unit &>(child).node()->representative() == k_temperatureRepresentatives + k_fahrenheitRepresentativeIndex))
    {
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
  Expression p = e.parent();
  if (p.isUninitialized() || p.type() == ExpressionNode::Type::UnitConvert || p.type() == ExpressionNode::Type::Store) {
    return false;
  }
  Expression pp = p.parent();
  return !(p.type() == ExpressionNode::Type::Opposite && (pp.isUninitialized() || pp.type() == ExpressionNode::Type::UnitConvert || pp.type() == ExpressionNode::Type::Store));
}

bool Unit::AllowImplicitAddition(const UnitNode::Representative * smallestRepresentative, const UnitNode::Representative * biggestRepresentative) {
  if (smallestRepresentative == biggestRepresentative) {
    return false;
  }
  for (int i = 0; i < k_representativesAllowingImplicitAdditionLength; i++) {
    bool foundFirstRepresentative = false;
    for (int j = 0; j < k_representativesAllowingImplicitAddition[i].length; j++) {
      if (smallestRepresentative == k_representativesAllowingImplicitAddition[i].representativesList[j]) {
        foundFirstRepresentative = true;
      } else if (biggestRepresentative == k_representativesAllowingImplicitAddition[i].representativesList[j]) {
        if (foundFirstRepresentative) {
          // Both representatives were found, in order.
          return true;
        }
        return false; // Not in right order
      }
    }
    if (foundFirstRepresentative) {
      return false; // Only one representative was found.
    }
  }
  return false;
}

bool Unit::ForceMarginLeftOfUnit(const Unit& unit) {
  const UnitNode::Representative * representative = unit.representative();
  for (int i = 0; i < k_numberOfRepresentativesWithoutLeftMargin; i++) {
    if (k_representativesWithoutLeftMargin[i] == representative) {
      return false;
    }
  }
  return true;
}

Expression Unit::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  if (reductionContext.unitConversion() == ExpressionNode::UnitConversion::None
      || isBaseUnit()) {
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
  if (node()->representative()->dimensionVector() == TemperatureRepresentative::Default().dimensionVector() && node()->representative() != k_temperatureRepresentatives + k_kelvinRepresentativeIndex) {
    Expression p = parent();
    if (p.isUninitialized() ||
        p.type() == ExpressionNode::Type::UnitConvert ||
        p.type() == ExpressionNode::Type::Store ||
        (p.type() == ExpressionNode::Type::Multiplication && p.numberOfChildren() == 2) ||
        p.type() == ExpressionNode::Type::Opposite)
    {
      /* If the parent is a UnitConvert, the temperature is always legal.
       * Otherwise, we need to wait until the reduction of the multiplication
       * to fully detect forbidden forms. */
      return *this;
    }
    return replaceWithUndefinedInPlace();
  }

  UnitNode * unitNode = node();
  const Representative * representative = unitNode->representative();
  const Prefix * prefix = unitNode->prefix();

  Expression result = representative->toBaseUnits(reductionContext).deepReduce(reductionContext);
  if (prefix != Prefix::EmptyPrefix()) {
    Expression prefixFactor = Power::Builder(Rational::Builder(10), Rational::Builder(prefix->exponent()));
    prefixFactor = prefixFactor.shallowReduce(reductionContext);
    result = Multiplication::Builder(prefixFactor, result).shallowReduce(reductionContext);
  }
  replaceWithInPlace(result);
  return result;
}

Expression Unit::shallowBeautify() {
  // Force Float(1) in front of an orphan Unit
  if (parent().isUninitialized() || parent().type() == ExpressionNode::Type::Opposite) {
    Multiplication m = Multiplication::Builder(Float<double>::Builder(1.));
    replaceWithInPlace(m);
    m.addChildAtIndexInPlace(*this, 1, 1);
    return std::move(m);
  }
  return *this;
}

Expression Unit::removeUnit(Expression * unit) {
  *unit = *this;
  Expression one = Rational::Builder(1);
  replaceWithInPlace(one);
  return one;
}

void Unit::chooseBestRepresentativeAndPrefix(double * value, double exponent, const ExpressionNode::ReductionContext& reductionContext, bool optimizePrefix) {
  assert(exponent != 0.f);

  if ((std::isinf(*value) || (*value == 0.0 && node()->representative()->dimensionVector() != TemperatureRepresentative::Default().dimensionVector()))) {
    /* Use the base unit to represent an infinite or null value, as all units
     * are equivalent.
     * This is not true for temperatures (0 K != 0°C != 0°F). */
    node()->setRepresentative(node()->representative()->representativesOfSameDimension());
    node()->setPrefix(node()->representative()->basePrefix());
    return;
  }
  // Convert value to base units
  double baseValue = *value * std::pow(node()->representative()->ratio() * std::pow(10., node()->prefix()->exponent() - node()->representative()->basePrefix()->exponent()), exponent);
  const Prefix * bestPrefix = (optimizePrefix) ? Prefix::EmptyPrefix() : nullptr;
  const Representative * bestRepresentative = node()->representative()->standardRepresentative(baseValue, exponent, reductionContext, &bestPrefix);
  if (!optimizePrefix) {
    bestPrefix = bestRepresentative->basePrefix();
  }

  if (bestRepresentative != node()->representative()) {
    *value = *value * std::pow(node()->representative()->ratio() / bestRepresentative->ratio() * std::pow(10., bestRepresentative->basePrefix()->exponent() - node()->representative()->basePrefix()->exponent()), exponent);
    node()->setRepresentative(bestRepresentative);
  }
  if (bestPrefix != node()->prefix()) {
    *value = *value * std::pow(10., exponent * (node()->prefix()->exponent() - bestPrefix->exponent()));
    node()->setPrefix(bestPrefix);
  }
}

template Evaluation<float> UnitNode::templatedApproximate<float>(const ApproximationContext& approximationContext) const;
template Evaluation<double> UnitNode::templatedApproximate<double>(const ApproximationContext& approximationContext) const;

}
