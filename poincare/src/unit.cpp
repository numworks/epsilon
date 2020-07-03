#include <poincare/unit.h>
#include <poincare/addition.h>
#include <poincare/division.h>
#include <poincare/float.h>
#include <poincare/ieee754.h>
#include <poincare/infinity.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>
#include <poincare/rational.h>
#include <poincare/layout_helper.h>
#include <limits.h>
#include <cmath>
#include <assert.h>
#include <string.h>
#include <utility>
#include <algorithm>

namespace Poincare {

int UnitNode::Prefix::serialize(char * buffer, int bufferSize) const {
  assert(bufferSize >= 0);
  return std::min<int>(strlcpy(buffer, m_symbol, bufferSize), bufferSize - 1);
}

bool UnitNode::Representative::canParse(const char * symbol, size_t length,
    const Prefix * * prefix) const
{
  if (!isPrefixable()) {
    *prefix = &Unit::EmptyPrefix;
    return length == 0;
  }
  size_t numberOfPrefixes = sizeof(Unit::AllPrefixes)/sizeof(Unit::Prefix *);
  for (size_t i = 0; i < numberOfPrefixes; i++) {
    const Prefix * pre = Unit::AllPrefixes[i];
    const char * prefixSymbol = pre->symbol();
    if (strncmp(symbol, prefixSymbol, length) == 0 &&
        prefixSymbol[length] == 0)
    {
      *prefix = pre;
      return true;
    }
    pre++;
  }
  return false;
}

int UnitNode::Representative::serialize(char * buffer, int bufferSize, const Prefix * prefix) const {
  int length = 0;
  length += prefix->serialize(buffer, bufferSize);
  assert(length == 0 || isPrefixable());
  assert(length < bufferSize);
  buffer += length;
  bufferSize -= length;
  assert(bufferSize >= 0);
  length += std::min<int>(strlcpy(buffer, m_rootSymbol, bufferSize), bufferSize - 1);
  return length;
}

static bool compareMagnitudeOrders(float order, float otherOrder) {
  /* Precision can be lost (with a year conversion for instance), so the order
   * value is rounded */
  if (std::fabs(order) < Expression::Epsilon<float>()) {
    order = 0.0f;
  }
  if (std::fabs(otherOrder) < Expression::Epsilon<float>()) {
    otherOrder = 0.0f;
  }
  if (std::fabs(std::fabs(order) - std::fabs(otherOrder)) < 3.0f && order * otherOrder < 0.0f) {
    /* If the two values are close, and their sign are opposed, the positive
     * order is preferred */
    return (order >= 0.0f);
  }
  // Otherwise, the closest order to 0 is preferred
  return (std::fabs(order) < std::fabs(otherOrder));
}

const UnitNode::Prefix * UnitNode::Representative::bestPrefixForValue(double & value, const float exponent) const {
  if (!isPrefixable()) {
    return &Unit::EmptyPrefix;
  }
  float bestOrder;
  const Prefix * bestPre = nullptr;
  /* Find the 'Prefix' with the most adequate 'exponent' for the order of
   * magnitude of 'value'.
   */
  const float orderOfMagnitude = std::log10(std::fabs(value));
  for (size_t i = 0; i < m_outputPrefixesLength; i++) {
    const Prefix * pre = m_outputPrefixes[i];
    float order = orderOfMagnitude - pre->exponent() * exponent;
    if (bestPre == nullptr || compareMagnitudeOrders(order, bestOrder)) {
      bestOrder = order;
      bestPre = pre;
    }
  }
  value *= std::pow(10.0, -bestPre->exponent() * exponent);
  return bestPre;
}

template<>
size_t UnitNode::Dimension::Vector<int>::supportSize() const {
  size_t supportSize = 0;
  for (const int * i = reinterpret_cast<const int*>(this); i < reinterpret_cast<const int*>(this) + NumberOfBaseUnits; i++) {
    int coefficient = *i;
    if (coefficient == 0) {
      continue;
    }
    supportSize++;
  }
  return supportSize;
}

template<>
Unit::Dimension::Vector<int> UnitNode::Dimension::Vector<int>::FromBaseUnits(const Expression baseUnits) {
  /* Returns the vector of Base units with integer exponents. If rational, the
   * closest integer will be used. */
  Vector<int> vector = {
    .time               = 0,
    .distance           = 0,
    .mass               = 0,
    .current            = 0,
    .temperature        = 0,
    .amountOfSubstance  = 0,
    .luminuousIntensity = 0,
  };
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
    const ptrdiff_t indexInTable = static_cast<UnitNode *>(factor.node())->dimension() - Unit::DimensionTable;
    assert(0 <= indexInTable && indexInTable < NumberOfBaseUnits);
    vector.setCoefficientAtIndex(indexInTable, exponent);
    if (++factorIndex >= numberOfFactors) {
      break;
    }
    factor = baseUnits.childAtIndex(factorIndex);
  } while (true);
  return vector;
}

bool UnitNode::Dimension::canParse(const char * symbol, size_t length,
    const Representative * * representative, const Prefix * * prefix) const
{
  const Representative * rep = m_representatives;
  while (rep < m_representativesUpperBound) {
    const char * rootSymbol = rep->rootSymbol();
    size_t rootSymbolLength = strlen(rootSymbol);
    int potentialPrefixLength = length - rootSymbolLength;
    if (potentialPrefixLength >= 0 &&
        strncmp(rootSymbol, symbol + potentialPrefixLength, rootSymbolLength) == 0 &&
        rep->canParse(symbol, potentialPrefixLength, prefix))
    {
      *representative = rep;
      return true;
    }
    rep++;
  }
  return false;
}

ExpressionNode::Sign UnitNode::sign(Context * context) const {
  return Sign::Positive;
}

Expression UnitNode::removeUnit(Expression * unit) {
  return Unit(this).removeUnit(unit);
}

int UnitNode::simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool canBeInterrupted, bool ignoreParentheses) const {
  if (!ascending) {
    return e->simplificationOrderSameType(this, true, canBeInterrupted, ignoreParentheses);
  }
  assert(type() == e->type());
  const UnitNode * eNode = static_cast<const UnitNode *>(e);
  // This works because dimensions are ordered in a table
  const ptrdiff_t dimdiff = eNode->dimension() - m_dimension;
  if (dimdiff != 0) {
    return dimdiff;
  }
  // This works because representatives are ordered in a table
  const ptrdiff_t repdiff = eNode->representative() - m_representative;
  if (repdiff != 0) {
    /* We order representatives in the reverse order as how they're stored in
     * the representatives table. This enables to sort addition of time as:
     * year + month + days + hours + minutes + seconds. */
    return -repdiff;
  }
  const ptrdiff_t prediff = eNode->prefix()->exponent() - m_prefix->exponent();
  return prediff;
}

Layout UnitNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  /* TODO: compute the bufferSize more precisely... So far the longest unit is
   * "month" of size 6 but later, we might add unicode to represent ohm or µ
   * which would change the required size?*/
  static constexpr size_t bufferSize = 10;
  char buffer[bufferSize];
  int length = serialize(buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits);
  assert(length < bufferSize);
  return LayoutHelper::String(buffer, length);
}

int UnitNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  assert(bufferSize >= 0);
  int underscoreLength = std::min<int>(strlcpy(buffer, "_", bufferSize), bufferSize - 1);
  buffer += underscoreLength;
  bufferSize -= underscoreLength;
  return underscoreLength + m_representative->serialize(buffer, bufferSize, m_prefix);
}

template<typename T>
Evaluation<T> UnitNode::templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  return Complex<T>::Undefined();
}

Expression UnitNode::shallowReduce(ReductionContext reductionContext) {
  return Unit(this).shallowReduce(reductionContext);
}

Expression UnitNode::shallowBeautify(ReductionContext reductionContext) {
  return Unit(this).shallowBeautify(reductionContext);
}

constexpr const Unit::Prefix
  Unit::PicoPrefix,
  Unit::NanoPrefix,
  Unit::MicroPrefix,
  Unit::MilliPrefix,
  Unit::CentiPrefix,
  Unit::DeciPrefix,
  Unit::EmptyPrefix,
  Unit::DecaPrefix,
  Unit::HectoPrefix,
  Unit::KiloPrefix,
  Unit::MegaPrefix,
  Unit::GigaPrefix,
  Unit::TeraPrefix;
constexpr const Unit::Prefix * const Unit::NoPrefix[];
constexpr const Unit::Prefix * const Unit::NegativeLongScalePrefixes[];
constexpr const Unit::Prefix * const Unit::PositiveLongScalePrefixes[];
constexpr const Unit::Prefix * const Unit::LongScalePrefixes[];
constexpr const Unit::Prefix * const Unit::NegativePrefixes[];
constexpr const Unit::Prefix * const Unit::AllPrefixes[];
constexpr const Unit::Representative
  Unit::TimeRepresentatives[],
  Unit::DistanceRepresentatives[],
  Unit::MassRepresentatives[],
  Unit::CurrentRepresentatives[],
  Unit::TemperatureRepresentatives[],
  Unit::AmountOfSubstanceRepresentatives[],
  Unit::LuminousIntensityRepresentatives[],
  Unit::FrequencyRepresentatives[],
  Unit::ForceRepresentatives[],
  Unit::PressureRepresentatives[],
  Unit::EnergyRepresentatives[],
  Unit::PowerRepresentatives[],
  Unit::ElectricChargeRepresentatives[],
  Unit::ElectricPotentialRepresentatives[],
  Unit::ElectricCapacitanceRepresentatives[],
  Unit::ElectricResistanceRepresentatives[],
  Unit::ElectricConductanceRepresentatives[],
  Unit::MagneticFluxRepresentatives[],
  Unit::MagneticFieldRepresentatives[],
  Unit::InductanceRepresentatives[],
  Unit::CatalyticActivityRepresentatives[],
  Unit::SurfaceRepresentatives[],
  Unit::VolumeRepresentatives[];
const Unit::Representative constexpr * Unit::SecondRepresentative;
const Unit::Representative constexpr * Unit::HourRepresentative;
const Unit::Representative constexpr * Unit::MeterRepresentative;
constexpr const Unit::Dimension Unit::DimensionTable[];
const Unit::Dimension constexpr * Unit::TimeDimension;
const Unit::Dimension constexpr * Unit::DistanceDimension;
constexpr const Unit::Dimension * Unit::DimensionTableUpperBound;

bool Unit::CanParse(const char * symbol, size_t length,
    const Dimension * * dimension, const Representative * * representative, const Prefix * * prefix)
{
  for (const Dimension * dim = DimensionTable; dim < DimensionTableUpperBound; dim++) {
    if (dim->canParse(symbol, length, representative, prefix)) {
      *dimension = dim;
      return true;
    }
  }
  return false;
}

Unit Unit::Builder(const Dimension * dimension, const Representative * representative, const Prefix * prefix) {
  void * bufferNode = TreePool::sharedPool()->alloc(sizeof(UnitNode));
  UnitNode * node = new (bufferNode) UnitNode(dimension, representative, prefix);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  return static_cast<Unit &>(h);
}

Expression Unit::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  if (reductionContext.unitConversion() == ExpressionNode::UnitConversion::None) {
    return *this;
  }
  UnitNode * unitNode = node();
  const Dimension * dim = unitNode->dimension();
  const Representative * rep = unitNode->representative();
  const Prefix * pre = unitNode->prefix();
  int8_t prefixMultiplier = pre->exponent();
  if (rep == dim->stdRepresentative()) {
    const Prefix * stdPre = dim->stdRepresentativePrefix();
    unitNode->setPrefix(stdPre);
    prefixMultiplier -= stdPre->exponent();
  }
  Expression result;
  if (rep->definition() == nullptr) {
    result = clone();
  } else {
    result = Expression::Parse(rep->definition(), nullptr, false).deepReduce(reductionContext);
  }
  if (prefixMultiplier != 0) {
    Expression multiplier = Power::Builder(Rational::Builder(10), Rational::Builder(prefixMultiplier)).shallowReduce(reductionContext);
    result = Multiplication::Builder(multiplier, result).shallowReduce(reductionContext);
  }
  replaceWithInPlace(result);
  return result;
}

Expression Unit::shallowBeautify(ExpressionNode::ReductionContext reductionContext) {
  // Force Float(1) in front of an orphan Unit
  if (parent().isUninitialized() || parent().type() == ExpressionNode::Type::Opposite) {
    Multiplication m = Multiplication::Builder(Float<double>::Builder(1.0));
    replaceWithInPlace(m);
    m.addChildAtIndexInPlace(*this, 1, 1);
    return std::move(m);
  }
  return *this;
}

void Unit::ChooseBestMultipleForValue(Expression * units, double * value, bool tuneRepresentative, ExpressionNode::ReductionContext reductionContext) {
  // Identify the first Unit factor and its exponent
  Expression firstFactor = *units;
  float exponent = 1.0f;
  if (firstFactor.type() == ExpressionNode::Type::Multiplication) {
    firstFactor = firstFactor.childAtIndex(0);
  }
  if (firstFactor.type() == ExpressionNode::Type::Power) {
    Expression exp = firstFactor.childAtIndex(1);
    firstFactor = firstFactor.childAtIndex(0);
    assert(exp.type() == ExpressionNode::Type::Rational);
    exponent = static_cast<const Rational &>(exp).node()->templatedApproximate<float>();
  }
  assert(firstFactor.type() == ExpressionNode::Type::Unit);
  // Choose its multiple and update value accordingly
  if (exponent != 0.0f) {
    static_cast<Unit&>(firstFactor).chooseBestMultipleForValue(value, exponent, tuneRepresentative, reductionContext);
  }
}

void Unit::chooseBestMultipleForValue(double * value, const float exponent, bool tuneRepresentative, ExpressionNode::ReductionContext reductionContext) {
  assert(!std::isnan(*value) && exponent != 0.0f);
  if (*value == 0.0 || *value == 1.0 || std::isinf(*value)) {
    return;
  }
  UnitNode * unitNode = node();
  const Dimension * dim = unitNode->dimension();
  /* Find in the Dimension 'dim' which unit (Prefix and optionally
   * Representative) make the value closer to 1.
   */
  const Representative * bestRep = unitNode->representative();
  const Prefix * bestPre = unitNode->prefix();
  double bestVal = *value;

  // Test all representatives if tuneRepresentative is on. Otherwise, force current representative
  const Representative * startRep = tuneRepresentative ? dim->stdRepresentative() : bestRep;
  const Representative * endRep = tuneRepresentative ? dim->representativesUpperBound() : bestRep + 1;
  for (const Representative * rep = startRep; rep < endRep; rep++) {
    // evaluate quotient
    double val = *value * std::pow(Division::Builder(clone(), Unit::Builder(dim, rep, &EmptyPrefix)).deepReduce(reductionContext).approximateToScalar<double>(reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit()), exponent);
    // Get the best prefix and update val accordingly
    const Prefix * pre = rep->bestPrefixForValue(val, exponent);
    if (compareMagnitudeOrders(std::log10(std::fabs(val)), std::log10(std::fabs(bestVal)))) {
      /* At this point, val is closer to one than bestVal is.*/
      bestRep = rep;
      bestPre = pre;
      bestVal = val;
    }
  }
  unitNode->setRepresentative(bestRep);
  unitNode->setPrefix(bestPre);
  *value = bestVal;
}

Expression Unit::removeUnit(Expression * unit) {
  *unit = *this;
  Expression one = Rational::Builder(1);
  replaceWithInPlace(one);
  return one;
}

bool Unit::isSecond() const {
  // TODO: comparing pointers suffices because all time dimension are built from the same pointers. This should be asserted some way at compile-time?
  return node()->dimension() == TimeDimension && node()->representative() == SecondRepresentative && node()->prefix() == &EmptyPrefix;
}

bool Unit::isMeter() const {
  // See comment on isSecond
  return node()->dimension() == DistanceDimension && node()->representative() == MeterRepresentative && node()->prefix() == &EmptyPrefix;
}


bool Unit::isKilogram() const {
  // See comment on isSecond
  return node()->dimension() == MassDimension && node()->representative() == KilogramRepresentative && node()->prefix() == &EmptyPrefix;
}

bool Unit::isSI() const {
  UnitNode * unitNode = node();
  const Dimension * dim = unitNode->dimension();
  const Representative * rep = unitNode->representative();
  return rep == dim->stdRepresentative() &&
         rep->definition() == nullptr &&
         unitNode->prefix() == dim->stdRepresentativePrefix();
}

bool Unit::IsSI(Expression & e) {
  if (e.type() == ExpressionNode::Type::Multiplication) {
    for (int i = 0; i < e.numberOfChildren(); i++) {
      Expression child = e.childAtIndex(i);
      assert(child.type() == ExpressionNode::Type::Power || child.type() == ExpressionNode::Type::Unit);
      if (!IsSI(child)) {
        return false;
      }
    }
    return true;
  }
  if (e.type() == ExpressionNode::Type::Power) {
    // Rational exponents are accepted in IS system
    assert(e.childAtIndex(1).type() == ExpressionNode::Type::Rational);
    Expression child = e.childAtIndex(0);
    assert(child.type() == ExpressionNode::Type::Unit);
    return IsSI(child);
  }
  assert(e.type() == ExpressionNode::Type::Unit);
  return static_cast<Unit &>(e).isSI();
}

bool Unit::IsSISpeed(Expression & e) {
  // Form m*s^-1
  return e.type() == ExpressionNode::Type::Multiplication && e.numberOfChildren() == 2 &&
    e.childAtIndex(0).type() == ExpressionNode::Type::Unit && e.childAtIndex(0).convert<Unit>().isMeter() &&
    e.childAtIndex(1).type() == ExpressionNode::Type::Power &&
    e.childAtIndex(1).childAtIndex(1).type() == ExpressionNode::Type::Rational && e.childAtIndex(1).childAtIndex(1).convert<const Rational>().isMinusOne() &&
    e.childAtIndex(1).childAtIndex(0).type() == ExpressionNode::Type::Unit && e.childAtIndex(1).childAtIndex(0).convert<Unit>().isSecond();
}

bool Unit::IsSIVolume(Expression & e) {
  // Form m^3
  return e.type() == ExpressionNode::Type::Power &&
    e.childAtIndex(0).type() == ExpressionNode::Type::Unit && e.childAtIndex(0).convert<Unit>().isMeter() &&
    e.childAtIndex(1).type() == ExpressionNode::Type::Rational && e.childAtIndex(1).convert<const Rational>().isThree();
}

bool Unit::IsSIEnergy(Expression & e) {
  // Form _kg*_m^2*_s^-2
  return e.type() == ExpressionNode::Type::Multiplication && e.numberOfChildren() == 3 &&
    e.childAtIndex(0).type() == ExpressionNode::Type::Unit && e.childAtIndex(0).convert<Unit>().isKilogram() &&
    e.childAtIndex(1).type() == ExpressionNode::Type::Power &&
    e.childAtIndex(1).childAtIndex(0).type() == ExpressionNode::Type::Unit && e.childAtIndex(1).childAtIndex(0).convert<Unit>().isMeter() &&
    e.childAtIndex(1).childAtIndex(1).type() == ExpressionNode::Type::Rational && e.childAtIndex(1).childAtIndex(1).convert<const Rational>().isTwo() &&
    e.childAtIndex(2).type() == ExpressionNode::Type::Power &&
    e.childAtIndex(2).childAtIndex(0).type() == ExpressionNode::Type::Unit && e.childAtIndex(2).childAtIndex(0).convert<Unit>().isSecond() &&
    e.childAtIndex(2).childAtIndex(1).type() == ExpressionNode::Type::Rational && e.childAtIndex(2).childAtIndex(1).convert<const Rational>().isMinusTwo();
}

bool Unit::IsSITime(Expression & e) {
  return e.type() == ExpressionNode::Type::Unit && static_cast<Unit &>(e).isSecond();
}

Expression Unit::BuildTimeSplit(double seconds, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) {
  assert(!std::isnan(seconds));
  if (std::isinf(seconds) || std::fabs(seconds) < Expression::Epsilon<double>()) {
    return Multiplication::Builder(Number::FloatNumber(seconds), Unit::Second());
  }
  /* Round the number of seconds to 13 significant digits
   * (= k_numberOfStoredSignificantDigits - 1).
     * Indeed, the user input has been converted to the most adequate unit
     * which might have led to approximating the value to 14 significants
     * digits. The number of seconds was then computed from this approximation.
     * We thus round it to avoid displaying small numbers of seconds that are
     * artifacts of the previous approximations. */
  double err = std::pow(10.0, Poincare::PrintFloat::k_numberOfStoredSignificantDigits - 1 - std::ceil(log10(std::fabs(seconds))));
  double remain = std::round(seconds*err)/err;

  constexpr static int numberOfTimeUnits = 6;
  constexpr static double timeFactors[numberOfTimeUnits] = {MonthPerYear*DaysPerMonth*HoursPerDay*MinutesPerHour*SecondsPerMinute, DaysPerMonth*HoursPerDay*MinutesPerHour*SecondsPerMinute, HoursPerDay*MinutesPerHour*SecondsPerMinute, MinutesPerHour*SecondsPerMinute, SecondsPerMinute, 1.0 };
  Unit units[numberOfTimeUnits] = {Unit::Year(), Unit::Month(), Unit::Day(), Unit::Hour(), Unit::Minute(), Unit::Second() };
  double valuesPerUnit[numberOfTimeUnits];
  Addition a = Addition::Builder();
  for (size_t i = 0; i < numberOfTimeUnits; i++) {
    valuesPerUnit[i] = remain/timeFactors[i];
    // Keep only the floor of the values except for the last unit (seconds)
    if (i < numberOfTimeUnits - 1) {
      valuesPerUnit[i] = valuesPerUnit[i] >= 0.0 ? std::floor(valuesPerUnit[i]) : std::ceil(valuesPerUnit[i]);
    }
    remain -= valuesPerUnit[i]*timeFactors[i];
    if (std::fabs(valuesPerUnit[i]) > Expression::Epsilon<double>()) {
      Multiplication m = Multiplication::Builder(Float<double>::Builder(valuesPerUnit[i]), units[i]);
      a.addChildAtIndexInPlace(m, a.numberOfChildren(), a.numberOfChildren());
    }
  }
  ExpressionNode::ReductionContext reductionContext(context, complexFormat, angleUnit, ExpressionNode::ReductionTarget::User, ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition, ExpressionNode::UnitConversion::None);
  // Beautify the addition into an subtraction if necessary
  return a.squashUnaryHierarchyInPlace().shallowBeautify(reductionContext);
}

template Evaluation<float> UnitNode::templatedApproximate<float>(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
template Evaluation<double> UnitNode::templatedApproximate<double>(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;

}
