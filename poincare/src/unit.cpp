#include <poincare/unit.h>
#include <poincare/division.h>
#include <poincare/float.h>
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
  if (isPrefixable()) {
    length += prefix->serialize(buffer, bufferSize);
    assert(length < bufferSize);
    buffer += length;
    bufferSize -= length;
  }
  assert(bufferSize >= 0);
  length += std::min<int>(strlcpy(buffer, m_rootSymbol, bufferSize), bufferSize - 1);
  return length;
}

const UnitNode::Prefix * UnitNode::Representative::bestPrefixForValue(double & value, const int exponent) const {
  const Prefix * bestPre = &Unit::EmptyPrefix;
  if (isPrefixable()) {
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
  }
  return bestPre;
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

Expression UnitNode::getUnit() const {
  return Unit(this).getUnit();
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
constexpr const Unit::Dimension Unit::DimensionTable[];
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
  if (reductionContext.symbolicComputation() == ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithUndefinedAndDoNotReplaceUnits) {
    return *this;
  }
  UnitNode * unitNode = static_cast<UnitNode *>(node());
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
  Expression ancestor = parent();
  // Force Float(1) in front of an orphan Unit
  if (ancestor.isUninitialized()) {
    Multiplication m = Multiplication::Builder(Float<double>::Builder(1.0));
    replaceWithInPlace(m);
    m.addChildAtIndexInPlace(*this, 1, 1);
    return std::move(m);
  }
  // Check that the exponent, if any, of a Unit is an integer
  if (!ancestor.isUninitialized() && ancestor.type() == ExpressionNode::Type::Power) {
    Expression exponent = ancestor.childAtIndex(1);
    if (!(exponent.type() == ExpressionNode::Type::Rational && static_cast<Rational &>(exponent).isInteger())) {
      goto UnitCheckUnsuccessful;
    }
    ancestor = ancestor.parent();
  }
  /* Check homogeneity: at this point, ancestor must be
   *  - either uninitialized
   *  - or a Multiplication whose parent is uninitialized.
   */
  if (!ancestor.isUninitialized() && ancestor.type() == ExpressionNode::Type::Multiplication) {
    ancestor = ancestor.parent();
  }
  if (!ancestor.isUninitialized() && ancestor.type() == ExpressionNode::Type::Opposite) {
    ancestor = ancestor.parent();
  }
  if (ancestor.isUninitialized()) {
    return *this;
  }
  UnitCheckUnsuccessful:
  /* If the latter checks are not successfully passed, then the function
   * returns replaceWithUndefinedInPlace.
   * TODO Something else should be returned in order to report a more
   * specific error. For instance: inhomogeneous expression.
   */
  return replaceWithUndefinedInPlace();
}

void Unit::chooseBestMultipleForValue(double & value, const int exponent, ExpressionNode::ReductionContext reductionContext) {
  assert(value != 0 && !std::isnan(value) && !std::isinf(value) && exponent != 0);
  UnitNode * unitNode = static_cast<UnitNode *>(node());
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

template Evaluation<float> UnitNode::templatedApproximate<float>(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
template Evaluation<double> UnitNode::templatedApproximate<double>(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;

}
