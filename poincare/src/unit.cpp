#include <poincare/unit.h>
#include <poincare/division.h>
#include <poincare/ieee754.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>
#include <poincare/rational.h>
#include <poincare/layout_helper.h>
#include <cmath>
#include <assert.h>
#include <string.h>
#include <utility>
#include <algorithm>

namespace Poincare {

static inline int absInt(int x) { return x >= 0 ? x : -x; }

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
  const Prefix * pre = Unit::AllPrefixes;
  while (pre < Unit::AllPrefixes + sizeof(Unit::AllPrefixes)/sizeof(Unit::Prefix)) {
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

const UnitNode::Prefix * UnitNode::Representative::bestPrefixForValue(double & value, const int exponent) const {
  if (!isPrefixable()) {
    return &Unit::EmptyPrefix;
  }
  const Prefix * bestPre = nullptr;
  unsigned int diff = -1;
  /* Find the 'Prefix' with the most adequate 'exponent' for the order of
   * magnitude of 'value'.
   */
  const int orderOfMagnitude = IEEE754<double>::exponentBase10(std::fabs(value));
  for (const Prefix * pre = m_outputPrefixes; pre < m_outputPrefixesUpperBound; pre++) {
    unsigned int newDiff = absInt(orderOfMagnitude - pre->exponent() * exponent);
    if (newDiff < diff) {
      diff = newDiff;
      bestPre = pre;
    }
  }
  value *= std::pow(10.0, -bestPre->exponent() * exponent);
  return bestPre;
}

template<>
Unit::Dimension::Vector<Integer>::Metrics UnitNode::Dimension::Vector<Integer>::metrics() const {
  size_t supportSize = 0;
  Integer norm(0);
  for (const Integer * i = reinterpret_cast<const Integer*>(this); i < reinterpret_cast<const Integer*>(this) + NumberOfBaseUnits; i++) {
    Integer coefficient = *i;
    if (coefficient.isZero()) {
      continue;
    }
    supportSize++;
    coefficient.setNegative(false);
    norm = Integer::Addition(norm, coefficient);
  }
  return {.supportSize = supportSize, .norm = norm};
}

template<>
Unit::Dimension::Vector<int8_t>::Metrics UnitNode::Dimension::Vector<int8_t>::metrics() const {
  size_t supportSize = 0;
  int8_t norm = 0;
  for (const int8_t * i = reinterpret_cast<const int8_t*>(this); i < reinterpret_cast<const int8_t*>(this) + NumberOfBaseUnits; i++) {
    int8_t coefficient = *i;
    if (coefficient == 0) {
      continue;
    }
    supportSize++;
    norm += coefficient > 0 ? coefficient : -coefficient;
  }
  return {.supportSize = supportSize, .norm = norm};
}

template<>
Unit::Dimension::Vector<Integer> UnitNode::Dimension::Vector<Integer>::FromBaseUnits(const Expression baseUnits) {
  Vector<Integer> vector;
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
    Integer exponent(1);
    if (factor.type() == ExpressionNode::Type::Power) {
      Expression exp = factor.childAtIndex(1);
      assert(exp.type() == ExpressionNode::Type::Rational && static_cast<Rational &>(exp).isInteger());
      exponent = static_cast<Rational &>(exp).signedIntegerNumerator();
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
  const ptrdiff_t dimdiff = eNode->dimension() - m_dimension;
  if (dimdiff != 0) {
    return dimdiff;
  }
  const ptrdiff_t repdiff = eNode->representative() - m_representative;
  if (repdiff != 0) {
    return repdiff;
  }
  const ptrdiff_t prediff = eNode->prefix() - m_prefix;
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
constexpr const Unit::Prefix
  Unit::NoPrefix[],
  Unit::NegativeLongScalePrefixes[],
  Unit::PositiveLongScalePrefixes[],
  Unit::LongScalePrefixes[],
  Unit::NegativePrefixes[],
  Unit::AllPrefixes[];
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
  if (parent().isUninitialized() && result.type() == ExpressionNode::Type::Unit) {
    // A Unit must not be orphan
    result = Multiplication::Builder(Rational::Builder(1), result);
  }
  replaceWithInPlace(result);
  return result;
}

void Unit::chooseBestMultipleForValue(double & value, const int exponent, ExpressionNode::ReductionContext reductionContext) {
  assert(value != 0 && !std::isnan(value) && !std::isinf(value) && exponent != 0);
  UnitNode * unitNode = node();
  const Dimension * dim = unitNode->dimension();
  /* Find in the Dimension 'dim' which unit (Representative and Prefix) make
   * the value closer to 1.
   */
  const Representative * bestRep = unitNode->representative();
  const Prefix * bestPre = unitNode->prefix();
  double bestVal = value;

  for (const Representative * rep = dim->stdRepresentative(); rep < dim->representativesUpperBound(); rep++) {
    // evaluate quotient
    double val = value * std::pow(Division::Builder(clone(), Unit::Builder(dim, rep, &EmptyPrefix)).deepReduce(reductionContext).approximateToScalar<double>(reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit()), exponent);
    // Get the best prefix and update val accordingly
    const Prefix * pre = rep->bestPrefixForValue(val, exponent);
    if (std::fabs(std::log10(std::fabs(bestVal))) - std::fabs(std::log10(std::fabs(val))) > Epsilon<double>()) {
      /* At this point, val is closer to one than bestVal is.*/
      bestRep = rep;
      bestPre = pre;
      bestVal = val;
    }
  }
  unitNode->setRepresentative(bestRep);
  unitNode->setPrefix(bestPre);
  value = bestVal;
}

Expression Unit::removeUnit(Expression * unit) {
  *unit = *this;
  Expression one = Rational::Builder(1);
  replaceWithInPlace(one);
  return one;
}

bool Unit::isSecond() const {
  return node()->dimension() == TimeDimension && node()->representative() == SecondRepresentative && node()->prefix() == &EmptyPrefix;
}

bool Unit::isMeter() const {
  return node()->dimension() == DistanceDimension && node()->representative() == MeterRepresentative && node()->prefix() == &EmptyPrefix;
}

bool Unit::IsISSpeed(Expression & e) {
  // Form m*s^-1
  return e.type() == ExpressionNode::Type::Multiplication && e.numberOfChildren() == 2 &&
    e.childAtIndex(0).type() == ExpressionNode::Type::Unit && e.childAtIndex(0).convert<Unit>().isMeter() &&
    e.childAtIndex(1).type() == ExpressionNode::Type::Power &&
    e.childAtIndex(1).childAtIndex(1).type() == ExpressionNode::Type::Rational && e.childAtIndex(1).childAtIndex(1).convert<const Rational>().isMinusOne() &&
    e.childAtIndex(1).childAtIndex(0).type() == ExpressionNode::Type::Unit && e.childAtIndex(1).childAtIndex(0).convert<Unit>().isSecond();
}

bool Unit::IsISVolume(Expression & e) {
  // Form m^3
  return e.type() == ExpressionNode::Type::Power &&
    e.childAtIndex(0).type() == ExpressionNode::Type::Unit && e.childAtIndex(0).convert<Unit>().isMeter() &&
    e.childAtIndex(1).type() == ExpressionNode::Type::Rational && e.childAtIndex(1).convert<const Rational>().isThree();
}

template Evaluation<float> UnitNode::templatedApproximate<float>(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
template Evaluation<double> UnitNode::templatedApproximate<double>(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;

}
