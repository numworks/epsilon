#include "unit.h"

#include <omg/round.h>
#include <poincare_junior/src/memory/pattern_matching.h>
#include <poincare_junior/src/n_ary.h>
#include <poincare_junior/src/numeric/float.h>

#include "approximation.h"
#include "integer.h"
#include "simplification.h"
#include "unit_representatives.h"

namespace PoincareJ {
namespace Units {

// Prefix
const Prefix* Prefix::Prefixes() { return Unit::k_prefixes; }

const Prefix* Prefix::EmptyPrefix() {
  return Prefixes() + Unit::k_emptyPrefixIndex;
}

uint8_t Prefix::ToId(const Prefix* prefix) {
  uint8_t id = (prefix - Prefixes());
  assert(FromId(id) == prefix);
  return id;
}

const Prefix* Prefix::FromId(uint8_t id) {
  assert(id < k_numberOfPrefixes);
  return Prefixes() + id;
}

#if 0
int Prefix::serialize(char* buffer, int bufferSize) const {
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
  if (baseUnits->isMultiplication()) {
    numberOfFactors = baseUnits->numberOfChildren();
    factor = baseUnits->child(0);
  } else {
    numberOfFactors = 1;
    factor = baseUnits;
  }
  do {
    // Get the unit's exponent
    int8_t exponent = 1;
    if (factor->isPower()) {
      const Tree* exp = factor->child(1);
      assert(exp->isRational());
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
      factor = factor->child(0);
    }
    // Fill the vector with the unit's exponent
    assert(factor->isUnit());
    vector.addAllCoefficients(
        Unit::GetRepresentative(factor)->dimensionVector(), exponent);
    if (++factorIndex >= numberOfFactors) {
      break;
    }
    factor = baseUnits->child(factorIndex);
  } while (true);
  return vector;
}

Tree* DimensionVector::toBaseUnits() const {
  Tree* result = SharedTreeStack->push<Type::Multiplication>(0);
  int numberOfChildren = 0;
  for (int i = 0; i < k_numberOfBaseUnits; i++) {
    // We require the base units to be the first seven in DefaultRepresentatives
    const Representative* representative =
        Representative::DefaultRepresentatives()[i];
    assert(representative);
    int8_t exponent = coefficientAtIndex(i);
    if (exponent == 0) {
      continue;
    }
    if (exponent != 1) {
      SharedTreeStack->push(Type::Power);
    }
    Unit::Push(representative);
    if (exponent != 1) {
      Integer::Push(exponent);
    }
    NAry::SetNumberOfChildren(result, ++numberOfChildren);
  }
  // assert(numberOfChildren > 0);
  NAry::SquashIfUnary(result) || NAry::SquashIfEmpty(result);
  return result;
}

// Representative
const Representative* const* Representative::DefaultRepresentatives() {
  static const Representative* defaultRepresentatives[k_numberOfDimensions] = {
      Time::representatives,
      Distance::representatives,
      Angle::representatives,
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
  };
  return defaultRepresentatives;
}

uint8_t Representative::ToId(const Representative* representative) {
  uint8_t id = 0;
  const Representative* list = representative->representativesOfSameDimension();
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

static bool CanSimplifyUnitProduct(const DimensionVector* unitsExponents,
                                   size_t& unitsSupportSize,
                                   const DimensionVector* entryUnitExponents,
                                   int entryUnitExponent,
                                   int8_t& bestUnitExponent,
                                   DimensionVector& bestRemainderExponents,
                                   size_t& bestRemainderSupportSize) {
  /* This function tries to simplify a Unit product (given as the
   * 'unitsExponents' int array), by applying a given operation. If the
   * result of the operation is simpler, 'bestUnit' and
   * 'bestRemainder' are updated accordingly. */
  DimensionVector simplifiedExponents;

  for (size_t i = 0; i < DimensionVector::k_numberOfBaseUnits; i++) {
    // Simplify unitsExponents with base units from derived unit
    simplifiedExponents.setCoefficientAtIndex(
        i, unitsExponents->coefficientAtIndex(i) -
               entryUnitExponent * entryUnitExponents->coefficientAtIndex(i));
  }
  size_t simplifiedSupportSize = simplifiedExponents.supportSize();
  /* Note: A metric is considered simpler if the support size (number of
   * symbols) is reduced. A norm taking coefficients into account is possible.
   * One could use the sum of all coefficients to favor _C_s from _A_s^2.
   * However, replacing _m_s^-2 with _N_kg^-1 should be avoided. */
  bool isSimpler = (1 + simplifiedSupportSize < unitsSupportSize);

  if (isSimpler) {
    bestUnitExponent = entryUnitExponent;
    bestRemainderExponents = simplifiedExponents;
    bestRemainderSupportSize = simplifiedSupportSize;
    /* unitsSupportSize is updated and will be taken into
     * account in next iterations of CanSimplifyUnitProduct. */
    unitsSupportSize = 1 + simplifiedSupportSize;
  }
  return isSimpler;
}

Tree* ChooseBestDerivedUnits(DimensionVector* unitsExponents) {
  /* Recognize derived units
   * - Look up in the table of derived units, the one which itself or its
   * inverse simplifies 'units' the most.
   * - If an entry is found, simplify 'units' and add the corresponding unit
   * or its inverse in 'unitsAccu'.
   * - Repeat those steps until no more simplification is possible.
   */
  Tree* unitsAccu = KMult()->clone();
  /* If exponents are not integers, FromBaseUnits will return a null
   * vector, preventing any attempt at simplification. This protects us
   * against undue "simplifications" such as _C^1.3 -> _C*_A^0.3*_s^0.3 */
  size_t unitsSupportSize = unitsExponents->supportSize();
  DimensionVector bestRemainderExponents;
  size_t bestRemainderSupportSize;
  while (unitsSupportSize > 1) {
    const Representative* bestDim = nullptr;
    int8_t bestUnitExponent = 0;
    // Look up in the table of derived units.
    for (int i = DimensionVector::k_numberOfBaseUnits;
         i < Representative::k_numberOfDimensions - 1; i++) {
      const Representative* dim = Representative::DefaultRepresentatives()[i];
      const DimensionVector entryUnitExponents = dim->dimensionVector();
      // A simplification is tried by either multiplying or dividing
      if (CanSimplifyUnitProduct(unitsExponents, unitsSupportSize,
                                 &entryUnitExponents, 1, bestUnitExponent,
                                 bestRemainderExponents,
                                 bestRemainderSupportSize) ||
          CanSimplifyUnitProduct(unitsExponents, unitsSupportSize,
                                 &entryUnitExponents, -1, bestUnitExponent,
                                 bestRemainderExponents,
                                 bestRemainderSupportSize)) {
        /* If successful, unitsSupportSize, bestUnitExponent,
         * bestRemainderExponents and bestRemainderSupportSize have been
         * updated*/
        bestDim = dim;
      }
    }
    if (bestDim == nullptr) {
      // No simplification could be performed
      break;
    }
    // Build and add the best derived unit
    Tree* derivedUnit = Unit::Push(bestDim->representativesOfSameDimension());

    assert(bestUnitExponent == 1 || bestUnitExponent == -1);
    if (bestUnitExponent == -1) {
      PatternMatching::MatchAndReplace(derivedUnit, KA, KPow(KA, -1_e));
    }

    const int position = unitsAccu->numberOfChildren();
    NAry::AddChildAtIndex(unitsAccu, derivedUnit, position);
    // Update remainder units and their exponents for next simplifications
    *unitsExponents = bestRemainderExponents;
    unitsSupportSize = bestRemainderSupportSize;
  }
  NAry::SquashIfEmpty(unitsAccu);
  return unitsAccu;
}

const Representative* Representative::RepresentativeForDimension(
    DimensionVector vector) {
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

const Representative* Representative::defaultFindBestRepresentative(
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
    *prefix = Prefix::EmptyPrefix();
  }
  return result;
}

#if 0
int Representative::serialize(char* buffer, int bufferSize,
                              const Prefix* prefix) const {
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

bool Representative::canPrefix(const Prefix* prefix, bool input) const {
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

const Prefix* Representative::findBestPrefix(double value,
                                             double exponent) const {
  if (!isOutputPrefixable()) {
    return Prefix::EmptyPrefix();
  }
  if (value < Float<double>::EpsilonLax()) {
    return Prefix::EmptyPrefix();
  }
  const Prefix* res = Prefix::EmptyPrefix();
  const float magnitude = std::log10(std::fabs(value));
  float bestOrder = magnitude;
  for (int i = 0; i < Prefix::k_numberOfPrefixes; i++) {
    if (!canPrefix(Prefix::Prefixes() + i, false)) {
      continue;
    }
    float order = magnitude - Prefix::Prefixes()[i].exponent() * exponent;
    if (compareMagnitudeOrders(order, bestOrder)) {
      bestOrder = order;
      res = Prefix::Prefixes() + i;
    }
  }
  return res;
}

#if 0
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
bool Unit::CanParse(UnicodeDecoder* name, const Representative** representative,
                    const Prefix** prefix) {
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
  for (int i = 0; i < Representative::k_numberOfDimensions; i++) {
    if (Representative::DefaultRepresentatives()[i]->canParseWithEquivalents(
            symbol + offset, length - offset, representative, prefix)) {
      return true;
    }
  }
  return false;
}

static void ChooseBestRepresentativeAndPrefixForValueOnSingleUnit(
    Tree* unit, double* value, UnitFormat unitFormat, bool optimizePrefix) {
  double exponent = 1.f;
  Tree* factor = unit;
  if (factor->isPower()) {
    Tree* childExponent = factor->child(1);
    assert(factor->child(0)->isUnit());
    assert(factor->child(1)->isRational());
    exponent = Approximation::To<double>(childExponent);
    factor = factor->child(0);
  }
  if (!factor->isUnit()) {
    return;
  }
  if (exponent == 0.f) {
    /* Finding the best representative for a unit with exponent 0 doesn't
     * really make sense, and should only happen with a weak ReductionTarget
     * (such as in Graph app), that only rely on approximations. We keep the
     * unit unchanged as it will approximate to undef anyway. */
    return;
  }
  Unit::ChooseBestRepresentativeAndPrefix(factor, value, exponent, unitFormat,
                                          optimizePrefix);
}

void Unit::ChooseBestRepresentativeAndPrefixForValue(Tree* units, double* value,
                                                     UnitFormat unitFormat) {
  int numberOfFactors;
  Tree* factor;
  if (units->isMultiplication()) {
    numberOfFactors = units->numberOfChildren();
    factor = units->child(0);
  } else {
    numberOfFactors = 1;
    factor = units;
  }
  ChooseBestRepresentativeAndPrefixForValueOnSingleUnit(factor, value,
                                                        unitFormat, true);
  for (int i = 1; i < numberOfFactors; i++) {
    ChooseBestRepresentativeAndPrefixForValueOnSingleUnit(
        units->child(i), value, unitFormat, false);
  }
}

#if 0
bool Unit::ShouldDisplayAdditionalOutputs(double value, Expression unit,
                                          Preferences::UnitFormat unitFormat) {
  if (unit.isUninitialized() || !std::isfinite(value)) {
    return false;
  }
  DimensionVector vector = DimensionVector::FromBaseUnits(unit);
  const Representative* representative =
      Representative::RepresentativeForDimension(vector);

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
  const Representative* representative =
      units.type() == ExpressionNode::Type::Unit
          ? static_cast<Unit&>(units).node()->representative()
          : Representative::RepresentativeForDimension(
                DimensionVector::FromBaseUnits(units));
  if (!representative) {
    return 0;
  }
  if (representative->dimensionVector() ==
      AngleRepresentative::Dimension) {
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

Expression Unit::ConvertTemperatureUnits(
    Expression e, Unit unit, const ReductionContext& reductionContext) {
  const Representative* targetRepr = unit.representative();
  const Prefix* targetPrefix = unit.node()->prefix();
  assert(unit.representative()->dimensionVector() ==
         TemperatureRepresentative::Dimension);

  Expression startUnit;
  e = e.removeUnit(&startUnit);
  if (startUnit.isUninitialized() ||
      startUnit.type() != ExpressionNode::Type::Unit) {
    return Undefined::Builder();
  }
  const Representative* startRepr =
      static_cast<Unit&>(startUnit).representative();
  if (startRepr->dimensionVector() !=
      TemperatureRepresentative::Dimension) {
    return Undefined::Builder();
  }

  const Prefix* startPrefix = static_cast<Unit&>(startUnit).node()->prefix();
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

bool Unit::AllowImplicitAddition(const Representative* smallestRepresentative,
                                 const Representative* biggestRepresentative) {
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

bool Unit::IsUnitOrPowerOfUnit(const Tree* expr) {
  return expr->isUnit() || (expr->isPower() && expr->child(0)->isUnit());
}

bool Unit::ForceMarginLeftOfUnit(const Tree* expr) {
  assert(IsUnitOrPowerOfUnit(expr));
  if (expr->isPower()) {
    expr = expr->child(0);
  }
  const Representative* representative = GetRepresentative(expr);
  for (const Representative* repr : k_representativesWithoutLeftMargin) {
    if (repr == representative) {
      return false;
    }
  }
  return true;
}

#if 0
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
      node()->representative() != &Representatives::Temperature::kelvin) {
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
  const Representative* representative = unitNode->representative();
  const Prefix* prefix = unitNode->prefix();

  Expression result = representative->toBaseUnits(reductionContext)
                          .deepReduce(reductionContext);
  if (prefix != Prefix::EmptyPrefix()) {
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
#endif

void Unit::ChooseBestRepresentativeAndPrefix(Tree* unit, double* value,
                                             double exponent,
                                             UnitFormat unitFormat,
                                             bool optimizePrefix) {
  assert(exponent != 0.f);

  if ((std::isinf(*value) ||
       (*value == 0.0 && GetRepresentative(unit)->dimensionVector() !=
                             Temperature::Dimension))) {
    /* Use the base unit to represent an infinite or null value, as all units
     * are equivalent.
     * This is not true for temperatures (0 K != 0°C != 0°F). */
    SetRepresentative(
        unit, GetRepresentative(unit)->representativesOfSameDimension());
    SetPrefix(unit, Prefix::EmptyPrefix());
    return;
  }
  // Convert value to base units
  double baseValue =
      *value * std::pow(GetRepresentative(unit)->ratio() *
                            std::pow(10., GetPrefix(unit)->exponent()),
                        exponent);
  const Prefix* bestPrefix = (optimizePrefix) ? Prefix::EmptyPrefix() : nullptr;
  const Representative* bestRepresentative =
      GetRepresentative(unit)->standardRepresentative(baseValue, exponent,
                                                      unitFormat, &bestPrefix);
  if (!optimizePrefix) {
    bestPrefix = Prefix::EmptyPrefix();
  }

  if (bestRepresentative != GetRepresentative(unit)) {
    *value = *value * std::pow(GetRepresentative(unit)->ratio() /
                                   bestRepresentative->ratio(),
                               exponent);
    SetRepresentative(unit, bestRepresentative);
  }
  if (bestPrefix != GetPrefix(unit)) {
    *value = *value * std::pow(10., exponent * (GetPrefix(unit)->exponent() -
                                                bestPrefix->exponent()));
    SetPrefix(unit, bestPrefix);
  }
}

void Unit::RemoveUnit(Tree* unit) {
  Tree* result = SharedTreeStack->push<Type::Multiplication>(2);
  GetRepresentative(unit)->ratioExpressionReduced()->clone();
  SharedTreeStack->push(Type::Power);
  Integer::Push(10);
  Integer::Push(GetPrefix(unit)->exponent());
  unit->moveTreeOverTree(result);
}

Tree* Unit::Push(const Representative* unitRepresentative,
                 const Prefix* unitPrefix) {
  uint8_t repId = Representative::ToId(unitRepresentative);
  uint8_t preId = Prefix::ToId(unitPrefix);
  return SharedTreeStack->push<Type::Unit>(repId, preId);
}

const Representative* Unit::GetRepresentative(const Tree* unit) {
  return Representative::FromId(unit->nodeValue(0));
}

const Prefix* Unit::GetPrefix(const Tree* unit) {
  return Prefix::FromId(unit->nodeValue(1));
}

void Unit::SetRepresentative(Tree* unit, const Representative* representative) {
  unit->setNodeValue(0, Representative::ToId(representative));
}

void Unit::SetPrefix(Tree* unit, const Prefix* prefix) {
  unit->setNodeValue(1, Prefix::ToId(prefix));
}

bool IsCombinationOfUnits(const Tree* expr) {
  if (expr->isUnit()) {
    return true;
  }
  if (expr->isMultiplication() || expr->isDivision()) {
    return !expr->hasChildSatisfying(
        [](const Tree* e) { return !IsCombinationOfUnits(e); });
  }
  if (expr->isPower()) {
    return IsCombinationOfUnits(expr->child(0));
  }
  return false;
}

bool HasUnit(const Tree* expr) {
  // TODO should HasUnit be replaced by dimensional analysis ?
  return expr->hasDescendantSatisfying(
      [](const Tree* e) { return e->isUnit() || e->isPhysicalConstant(); });
}

bool IsPureAngleUnit(const Tree* expr) {
  return expr->isUnit() &&
         Unit::GetRepresentative(expr)->dimensionVector() == Angle::Dimension;
}

}  // namespace Units
}  // namespace PoincareJ
