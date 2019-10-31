#include <poincare/unit.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>
#include <poincare/rational.h>
#include <poincare/layout_helper.h>
#include <assert.h>
#include <string.h>

namespace Poincare {

static inline int minInt(int x, int y) { return x < y ? x : y; }

size_t UnitNode::Prefix::serialize(char * buffer, size_t length) const {
  return minInt(strlcpy(buffer, m_symbol, length), length - 1);
}

bool UnitNode::Representative::canParse(const char * symbol, size_t length,
    const Prefix * * prefix) const
{
  const Prefix * pre = m_allowedPrefixes;
  while (pre < m_allowedPrefixesUpperBound) {
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

size_t UnitNode::Representative::serialize(char * buffer, size_t length, const Prefix * prefix) const {
  size_t prefixLength = prefix->serialize(buffer, length);
  buffer += prefixLength;
  length -= prefixLength;
  return prefixLength + minInt(strlcpy(buffer, m_rootSymbol, length), length - 1);
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

int UnitNode::simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool canBeInterrupted) const {
  if (!ascending) {
    return e->simplificationOrderSameType(this, true, canBeInterrupted);
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
  static constexpr size_t bufferSize = 10;
  char buffer[bufferSize];
  int length = serialize(buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits);
  assert(length < bufferSize);
  return LayoutHelper::String(buffer, length);
}

int UnitNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return m_representative->serialize(buffer, bufferSize, m_prefix);
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
  Unit::AllPrefixes[];
constexpr size_t
  Unit::NoPrefixCount,
  Unit::NegativeLongScalePrefixesCount,
  Unit::PositiveLongScalePrefixesCount,
  Unit::AllPrefixesCount;
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
  Expression result = *this;
  if (rep->definition() != nullptr) {
    result = Expression::Parse(rep->definition(), nullptr).deepReduce(reductionContext);
  }
  if (prefixMultiplier != 0) {
    Expression multiplier = Power::Builder(Rational::Builder(10), Rational::Builder(prefixMultiplier));
    if (result.type() != ExpressionNode::Type::Multiplication) {
      result = Multiplication::Builder(multiplier, result.clone());
    } else {
      static_cast<Multiplication &>(result).addChildAtIndexInPlace(
          multiplier,
          0,
          result.numberOfChildren());
    }
  }
  replaceWithInPlace(result);
  return result;
}

Expression Unit::shallowBeautify(ExpressionNode::ReductionContext reductionContext) {
  Expression ancestor = parent();
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

template Evaluation<float> UnitNode::templatedApproximate<float>(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
template Evaluation<double> UnitNode::templatedApproximate<double>(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;

}
