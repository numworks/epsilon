#include "unit.h"

#include <omg/float.h>
#include <omg/round.h>
#include <poincare/print_float.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/memory/tree_stack.h>

#include "../approximation.h"
#include "../integer.h"
#include "../parametric.h"
#include "../physical_constant.h"
#include "../simplification.h"
#include "representatives.h"

namespace Poincare::Internal {
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

static bool CanSimplifyUnitProduct(const SIVector* unitsExponents,
                                   size_t& unitsSupportSize,
                                   const SIVector* entryUnitExponents,
                                   int entryUnitExponent,
                                   int8_t& bestUnitExponent,
                                   SIVector& bestRemainderExponents,
                                   size_t& bestRemainderSupportSize) {
  /* This function tries to simplify a Unit product (given as the
   * 'unitsExponents' int array), by applying a given operation. If the
   * result of the operation is simpler, 'bestUnit' and
   * 'bestRemainder' are updated accordingly. */
  SIVector simplifiedExponents;

  for (size_t i = 0; i < SIVector::k_numberOfBaseUnits; i++) {
    // Simplify unitsExponents with base units from derived unit
    bool success = simplifiedExponents.setCoefficientAtIndex(
        i, unitsExponents->coefficientAtIndex(i) -
               entryUnitExponent * entryUnitExponents->coefficientAtIndex(i));
    assert(success);
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

Tree* ChooseBestDerivedUnits(SIVector* unitsExponents) {
  /* Recognize derived units
   * - Look up in the table of derived units, the one which itself or its
   * inverse simplifies 'units' the most.
   * - If an entry is found, simplify 'units' and add the corresponding unit
   * or its inverse in 'unitsAccu'.
   * - Repeat those steps until no more simplification is possible.
   */
  Tree* unitsAccu = KMult()->cloneTree();
  /* If exponents are not integers, GetSIVector will return a null
   * vector, preventing any attempt at simplification. This protects us
   * against undue "simplifications" such as _C^1.3 -> _C*_A^0.3*_s^0.3 */
  size_t unitsSupportSize = unitsExponents->supportSize();
  SIVector bestRemainderExponents;
  size_t bestRemainderSupportSize;
  while (unitsSupportSize > 1) {
    const Representative* bestDim = nullptr;
    int8_t bestUnitExponent = 0;
    // Look up in the table of derived units.
    for (int i = SIVector::k_numberOfBaseUnits;
         i < Representative::k_numberOfDimensions - 1; i++) {
      const Representative* dim = Representative::DefaultRepresentatives()[i];
      const SIVector entryUnitExponents = dim->siVector();
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
      PatternMatching::MatchReplace(derivedUnit, KA, KPow(KA, -1_e));
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

static bool compareMagnitudeOrders(float order, float otherOrder) {
  /* Precision can be lost (with a year conversion for instance), so the order
   * value is rounded */
  order = OMG::LaxToZero(order);
  otherOrder = OMG::LaxToZero(otherOrder);
  if (std::fabs(std::fabs(order) - std::fabs(otherOrder)) <=
          3.0f + OMG::Float::EpsilonLax<float>() &&
      order * otherOrder < 0.0f) {
    /* If the two values are close, and their sign are opposed, the positive
     * order is preferred */
    return (order >= 0.0f);
  }
  // Otherwise, the closest order to 0 is preferred
  return (std::fabs(order) < std::fabs(otherOrder));
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
  if (value < OMG::Float::EpsilonLax<double>()) {
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

Tree* Representative::pushReducedRatioExpression() const {
  Tree* result = ratioExpression()->cloneTree();
  /* Representatives's ratio expressions are quite simple and not dependant on
   * any context. */
  ProjectionContext ctx{.m_advanceReduce = false};
  Simplification::ProjectAndReduce(result, &ctx);
  return result;
}

// Unit
bool Unit::CanParse(ForwardUnicodeDecoder* name,
                    const Representative** representative,
                    const Prefix** prefix) {
  if (name->codePoint() == '_') {
    name->nextCodePoint();
    if (name->isEmpty()) {
      return false;
    }
  }
  // TODO: Better use of UnicodeDecoder. Here we assume units cannot be longer.
  constexpr static size_t bufferSize = 10;
  char symbol[bufferSize];
  size_t length = name->printInBuffer(symbol, bufferSize);
  if (length == 0) {
    // the name was too long for the buffer, it cannot be a unit
    return false;
  }
  assert(length < bufferSize);
  for (int i = 0; i < Representative::k_numberOfDimensions; i++) {
    if (Representative::DefaultRepresentatives()[i]->canParseWithEquivalents(
            symbol, length, representative, prefix)) {
      return true;
    }
  }
  return false;
}

// Return true if best representative and prefix have been chosen.
static bool ChooseBestRepresentativeAndPrefixForValueOnSingleUnit(
    Tree* unit, double* value, UnitFormat unitFormat,
    bool optimizeRepresentative) {
  double exponent = 1.f;
  Tree* factor = unit;
  if (factor->isPow()) {
    Tree* childExponent = factor->child(1);
    assert(factor->child(0)->isUnit());
    assert(factor->child(1)->isRational());
    exponent =
        Approximation::To<double>(childExponent, Approximation::Parameters{});
    factor = factor->child(0);
  }
  if (!factor->isUnit()) {
    return false;
  }
  if (exponent == 0.f) {
    /* Finding the best representative for a unit with exponent 0 doesn't
     * really make sense, and should only happen with a weak ReductionTarget
     * (such as in Graph app), that only rely on approximations. We keep the
     * unit unchanged as it will approximate to undef anyway. */
    return false;
  }
  Unit::ChooseBestRepresentativeAndPrefix(factor, value, exponent, unitFormat,
                                          optimizeRepresentative);
  return true;
}

void Unit::ChooseBestRepresentativeAndPrefixForValue(Tree* units, double* value,
                                                     UnitFormat unitFormat) {
  if (units->isMult()) {
    for (Tree* factor : units->children()) {
      ChooseBestRepresentativeAndPrefixForValueOnSingleUnit(factor, value,
                                                            unitFormat, true);
    }
  } else {
    ChooseBestRepresentativeAndPrefixForValueOnSingleUnit(units, value,
                                                          unitFormat, true);
  }
}

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

bool Unit::IsUnitOrPowerOfUnit(const Tree* e) {
  return e->isUnitOrPhysicalConstant() ||
         (e->isPow() && e->child(0)->isUnitOrPhysicalConstant());
}

bool Unit::ForceMarginLeftOfUnit(const Tree* e) {
  assert(IsUnitOrPowerOfUnit(e));
  if (e->isPow()) {
    e = e->child(0);
  }
  const Representative* representative = GetRepresentative(e);
  for (const Representative* repr : k_representativesWithoutLeftMargin) {
    if (repr == representative) {
      return false;
    }
  }
  return true;
}

void Unit::ChooseBestRepresentativeAndPrefix(Tree* unit, double* value,
                                             double exponent,
                                             UnitFormat unitFormat,
                                             bool optimizeRepresentative) {
  assert(exponent != 0.f);

  if ((std::isinf(*value) ||
       (*value == 0.0 &&
        GetRepresentative(unit)->siVector() != Temperature::Dimension))) {
    /* Use the base unit to represent an infinite or null value, as all units
     * are equivalent.
     * This is not true for temperatures (0 K != 0°C != 0°F). */
    SetRepresentative(
        unit, GetRepresentative(unit)->representativesOfSameDimension());
    SetPrefix(unit, Prefix::EmptyPrefix());
    return;
  }
  // Convert value to base units
  double baseValue = *value * std::pow(GetValue(unit), exponent);
  const Prefix* bestPrefix = Prefix::EmptyPrefix();
  const Representative* bestRepresentative =
      GetRepresentative(unit)->bestRepresentativeAndPrefix(
          baseValue, exponent, unitFormat, &bestPrefix,
          optimizeRepresentative ? nullptr : GetRepresentative(unit));

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

bool Unit::IsNonKelvinTemperature(const Representative* representative) {
  return representative == &Temperature::representatives.celsius ||
         representative == &Temperature::representatives.fahrenheit;
}

void Unit::RemoveUnit(Tree* unit) {
  const Representative* representative = GetRepresentative(unit);
  // Temperature units should have been escaped before.
  assert(!IsNonKelvinTemperature(representative));
  Tree* result = SharedTreeStack->pushMult(2);
  representative->pushReducedRatioExpression();
  SharedTreeStack->pushPow();
  Integer::Push(10);
  Integer::Push(GetPrefix(unit)->exponent());
  unit->moveTreeOverTree(result);
}

void Unit::RemoveTemperatureUnit(Tree* root) {
  assert(Dimension::DeepCheck(root));
  // Find and remove the unit.
  const Representative* representative = nullptr;
  for (Tree* child : root->selfAndDescendants()) {
    if (child->isUnit()) {
      const Representative* childRepresentative =
          Units::Unit::GetRepresentative(child);
      if (IsNonKelvinTemperature(childRepresentative)) {
        assert(representative == nullptr);
        representative = childRepresentative;
        child->cloneTreeOverTree(1_e);
#if !ASSERTIONS
        break;
#endif
      }
    }
  }
  assert(IsNonKelvinTemperature(representative));
  bool isCelsius = (representative == &Temperature::representatives.celsius);
  // A -> (A + origin) * ratio
  TreeRef ratio = representative->pushReducedRatioExpression();
  root->moveTreeOverTree(PatternMatching::Create(
      KMult(KAdd(KA, KB), KC), {.KA = root,
                                .KB = isCelsius ? Temperature::celsiusOrigin
                                                : Temperature::fahrenheitOrigin,
                                .KC = ratio}));
  ratio->removeTree();
}

double Unit::KelvinValueToRepresentative(double value,
                                         const Representative* representative) {
  if (representative == &Temperature::representatives.kelvin) {
    return value;
  }
  assert(IsNonKelvinTemperature(representative));
  bool isCelsius = (representative == &Temperature::representatives.celsius);
  // A -> (A / ratio) - origin
  return (value / representative->ratio()) -
         Approximation::To<double>(isCelsius ? Temperature::celsiusOrigin
                                             : Temperature::fahrenheitOrigin,
                                   Approximation::Parameters{});
}

Tree* Unit::Push(const Representative* unitRepresentative,
                 const Prefix* unitPrefix) {
  uint8_t repId = Representative::ToId(unitRepresentative);
  uint8_t preId = Prefix::ToId(unitPrefix);
  return SharedTreeStack->pushUnit(repId, preId);
}

Tree* Unit::Push(AngleUnit angleUnit) {
  return Push(Angle::DefaultRepresentativeForAngleUnit(angleUnit),
              Prefix::EmptyPrefix());
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

double Unit::GetValue(const Tree* unit) {
  return GetRepresentative(unit)->ratio() *
         std::pow(10., GetPrefix(unit)->exponent());
}

SIVector Unit::GetSIVector(const Tree* baseUnits) {
  SIVector vector = SIVector::Empty();
  int numberOfFactors;
  int factorIndex = 0;
  const Tree* factor;
  if (baseUnits->isMult()) {
    numberOfFactors = baseUnits->numberOfChildren();
    factor = baseUnits->child(0);
  } else {
    numberOfFactors = 1;
    factor = baseUnits;
  }
  do {
    // Get the unit's exponent
    int8_t exponent = 1;
    if (factor->isPow()) {
      const Tree* exp = factor->child(1);
      assert(exp->isRational());
      // Using the closest integer to the exponent.
      float exponentFloat =
          Approximation::To<float>(exp, Approximation::Parameters{});
      if (exponentFloat != std::round(exponentFloat)) {
        /* If non-integer exponents are found, we round a null vector so that
         * Multiplication::shallowBeautify will not attempt to find derived
         * units. */
        return SIVector::Empty();
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
    bool success = vector.addAllCoefficients(
        GetRepresentative(factor)->siVector(), exponent);
    assert(success);
    if (++factorIndex >= numberOfFactors) {
      break;
    }
    factor = baseUnits->child(factorIndex);
  } while (true);
  return vector;
}

Tree* Unit::GetBaseUnits(SIVector vector) {
  Tree* result = SharedTreeStack->pushMult(0);
  int numberOfChildren = 0;
  for (int i = 0; i < k_numberOfBaseUnits; i++) {
    // We require the base units to be the first seven in DefaultRepresentatives
    const Representative* representative =
        Representative::DefaultRepresentatives()[i];
    assert(representative);
    int8_t exponent = vector.coefficientAtIndex(i);
    if (exponent == 0) {
      continue;
    }
    if (exponent != 1) {
      SharedTreeStack->pushPow();
    }
    Push(representative);
    if (exponent != 1) {
      Integer::Push(exponent);
    }
    NAry::SetNumberOfChildren(result, ++numberOfChildren);
  }
  // assert(numberOfChildren > 0);
  NAry::SquashIfUnary(result) || NAry::SquashIfEmpty(result);
  return result;
}

bool HasPhysicalConstant(const Tree* e) {
  return e->hasDescendantSatisfying(
      [](const Tree* e) { return e->isPhysicalConstant(); });
}

/* TODO_PCJ: Confirm this logic that has not been re-implemented in additional
 * results is no longer expected. Implement it otherwise.
 * - Angle unit displayed as degree and radians
 * - Temperature units displayed as Kelvin, Celsius and Fahrenheit
 * - Energy units displayed as J, Wh and eV
 * - Surface units displayed as ha (and acres if imperial)
 * - Volume units displayed as L
 * - Speed units displayed as km/h (and mph if imperial)
 */

// From a projected tree, gather units and use best representatives/prefixes.
bool Unit::ProjectToBestUnits(Tree* e, Dimension dimension,
                              UnitDisplay unitDisplay, AngleUnit angleUnit,
                              UnitFormat unitFormat) {
  if (unitDisplay == UnitDisplay::None && !e->isUnitConversion()) {
    // TODO_PCJ : Remove units that cancel themselves
    return false;
  }
  if (!dimension.isUnit()) {
    // There may remain units that cancel themselves, remove them.
    return Tree::ApplyShallowTopDown(e, ShallowRemoveUnit);
  }
  if (HasPhysicalConstant(e) &&
      !e->hasDescendantSatisfying([](const Tree* e) { return e->isUnit(); }) &&
      unitDisplay != UnitDisplay::MainOutput) {
    // Display only SI and derived units for inputs with physical constants only
    unitDisplay = UnitDisplay::BasicSI;
  }
  TreeRef extractedUnits = e->cloneTree();
  if (e->isUnitConversion()) {
    // Use second child for target units and first one for value.
    MoveTreeOverTree(extractedUnits, extractedUnits->child(1));
    e->moveTreeOverTree(e->child(0));
    unitDisplay = UnitDisplay::AutomaticInput;
    assert(!Dimension::Get(e).isScalar());
  }
  if (IsNonKelvinTemperature(dimension.unit.representative)) {
    // Temperature units must be removed from root expression
    RemoveTemperatureUnit(e);
  }
  // Turn e into its SI value. 2_m + _yd -> 3.8288
  Tree::ApplyShallowTopDown(e, ShallowRemoveUnit);
  assert(extractedUnits && e->nextTree() == extractedUnits);
  bool treeRemoved = RemoveNonUnits(extractedUnits, true);
  // Warning : extractedUnits isn't just e's dimension. 2_m + _yd -> _m + _yd
  assert(!treeRemoved);
  (void)treeRemoved;
  // Take advantage of e being last tree.
  assert(extractedUnits);
  assert(dimension.unit.vector == Dimension::Get(extractedUnits).unit.vector);
  bool foundResult = true;
  switch (unitDisplay) {
    case UnitDisplay::Forbidden:
      extractedUnits->removeTree();
      foundResult = false;
      break;
    case UnitDisplay::MainOutput:
      ApplyMainOutputDisplay(e, extractedUnits, dimension, angleUnit);
      break;
    case UnitDisplay::AutomaticMetric:
    case UnitDisplay::AutomaticImperial:
    case UnitDisplay::AutomaticPrefixFreeMetric:
      ApplyAutomaticDisplay(e, extractedUnits, dimension, unitDisplay);
      break;
    case UnitDisplay::AutomaticInput:
      // TODO: Handle power of same dimension better 1ft* 1ft* 1in -> in^3 */
      foundResult = ApplyAutomaticInputDisplay(e, extractedUnits);
      break;
    case UnitDisplay::Equivalent:
      foundResult =
          ApplyEquivalentDisplay(e, extractedUnits, dimension, unitFormat);
      break;
    case UnitDisplay::Decomposition:
      foundResult =
          ApplyDecompositionDisplay(e, extractedUnits, dimension, unitFormat);
      break;
    case UnitDisplay::BasicSI:
      MoveTreeOverTree(extractedUnits, GetBaseUnits(dimension.unit.vector));
      assert(e->nextTree() == extractedUnits);
      e->cloneNodeAtNode(KMult.node<2>);
      break;
    case UnitDisplay::None:
      OMG::unreachable();
  }
  if (!foundResult) {
    e->cloneTreeOverTree(KUndef);
  }
  return true;
}

// Find one or two units in expression, return false if there are more than two
bool GetUnits(Tree* extractedUnits, Tree** unit1, Tree** unit2) {
  assert(!*unit1 && !*unit2);
  for (Tree* e : extractedUnits->selfAndDescendants()) {
    if (e->isUnitOrPhysicalConstant()) {
      if (*unit2) {
        return false;
      }
      (*unit1 ? *unit2 : *unit1) = e;
    }
  }
  return true;
}

bool Unit::DisplayImperialUnitsInOutput(const Tree* inputUnits) {
  bool hasImperialUnits = false;
  for (const Tree* e : inputUnits->selfAndDescendants()) {
    if (e->isUnit()) {
      if (GetRepresentative(e)->isImperial()) {
        hasImperialUnits = true;
      } else if (GetRepresentative(e) != &Volume::representatives.liter) {
        // Use metric representatives if any other metric unit is involved.
        return false;
      }
      // Liter representative is tolerated for imperial unit input.
    }
  }
  return hasImperialUnits;
}

Tree* Unit::ExactConvertToUnit(const Tree* e, TreeRef& targetUnit) {
  // Make sure targetUnit does not contain non-units or additions of units.
  RemoveNonUnits(targetUnit, false);
  // Multiply e, targetUnit and inverse of targetUnit's SI value.
  Tree* unitClone = targetUnit->cloneTree();
  Tree::ApplyShallowTopDown(unitClone, Unit::ShallowRemoveUnit);
  TreeRef result =
      PatternMatching::Create(KMult(KA, KB, KPow(KC, -1_e)),
                              {.KA = e, .KB = targetUnit, .KC = unitClone});
  unitClone->removeTree();
  return result;
}

void Unit::ApplyMainOutputDisplay(Tree* e, TreeRef& inputUnits,
                                  Dimension dimension, AngleUnit angleUnit) {
  if (dimension.isAngleUnit()) {
    // Replace inputUnits to target angle unit.
    Tree* newExtractedUnits = KPow->cloneNode();
    Unit::Push(angleUnit);
    Integer::Push(dimension.unit.vector.angle);
    assert(Dimension::Get(newExtractedUnits) == Dimension::Get(inputUnits));
    MoveTreeOverTree(inputUnits, newExtractedUnits);
    e->moveTreeOverTree(ExactConvertToUnit(e, inputUnits));
    inputUnits->removeTree();
    return;
  }

  Tree* unit1 = nullptr;
  Tree* unit2 = nullptr;
  /* If the input is made of one single unit, preserve it.
   * Consider speed as a single unit but not physical constants. */
  if (GetUnits(inputUnits, &unit1, &unit2) &&
      ((!unit2 && !unit1->isPhysicalConstant()) ||
       (unit2 && dimension.unit.vector == Speed::Dimension))) {
    assert(!dimension.isAngleUnit());
    double value = Approximation::To<double>(e, Approximation::Parameters{});
    if (IsNonKelvinTemperature(GetRepresentative(unit1))) {
      value = KelvinValueToRepresentative(value, GetRepresentative(unit1));
    } else {
      // Correct the value since e is in basic SI
      value /=
          Approximation::To<double>(inputUnits, Approximation::Parameters{});
    }
    e->moveTreeOverTree(SharedTreeStack->pushDoubleFloat(value));
    // Multiply value and inputUnits.
    assert(e->nextTree() == inputUnits);
    e->cloneNodeAtNode(KMult.node<2>);
    return;
  }
  // Fallback on automatic imperial display or prefix free metric display
  UnitDisplay display = DisplayImperialUnitsInOutput(inputUnits)
                            ? UnitDisplay::AutomaticImperial
                            : UnitDisplay::AutomaticPrefixFreeMetric;
  assert(display == UnitDisplay::AutomaticPrefixFreeMetric ||
         !HasPhysicalConstant(e));
  ApplyAutomaticDisplay(e, inputUnits, dimension, display);
}

bool Unit::ShallowRemoveUnit(Tree* e, void*) {
  switch (e->type()) {
    case Type::Unit:
      // RemoveUnit replace with SI ratio expression.
      RemoveUnit(e);
      return true;
    case Type::PhysicalConstant: {
      e->moveTreeOverTree(SharedTreeStack->pushDoubleFloat(
          PhysicalConstant::GetProperties(e).m_value));
      return true;
    }
    default:
      return false;
  }
}

bool Unit::RemoveNonUnits(Tree* e, bool preserveAdd) {
#if ASSERTIONS
  bool wasUnit = Dimension::Get(e).isUnit();
#endif
  bool didRemovedTree;
  switch (e->type()) {
    case Type::Unit:
    case Type::PhysicalConstant:
      didRemovedTree = false;
      break;
    case Type::Add:
    case Type::Mult: {
      int n = e->numberOfChildren();
      int remainingChildren = n;
      Tree* child = e->child(0);
      // If !preserveAdd, replace _m + _km + _yd with _m.
      bool removeRemainingChildren = false;
      for (int i = 0; i < n; i++) {
        if (removeRemainingChildren) {
          child->removeTree();
          remainingChildren--;
        } else if (!RemoveNonUnits(child, preserveAdd)) {
          child = child->nextTree();
          removeRemainingChildren = !preserveAdd && e->isAdd();
        } else {
          remainingChildren--;
        }
      }
      if (child == e->nextNode()) {
        // All children have been removed.
        e->removeNode();
        didRemovedTree = true;
        break;
      }
      assert(remainingChildren > 0);
      if (remainingChildren == 1) {
        e->removeNode();
      } else {
        NAry::SetNumberOfChildren(e, remainingChildren);
      }
      didRemovedTree = false;
      break;
    }
    case Type::Sum:
    case Type::Product:
      e->moveTreeOverTree(e->child(Parametric::FunctionIndex(e->type())));
      didRemovedTree = RemoveNonUnits(e, preserveAdd);
      break;
    case Type::Abs:
      e->removeNode();
      didRemovedTree = RemoveNonUnits(e, preserveAdd);
      break;
    case Type::PowReal:
      // Ignore PowReal nodes
      e->cloneNodeOverNode(KPow);
    case Type::Pow:
      // If there are units in base, keep the tree.
      if (!RemoveNonUnits(e->child(0), preserveAdd)) {
        didRemovedTree = false;
        break;
      }
      // Otherwise, remove node and index, ignoring their units.
      e->removeNode();
      e->removeTree();
      didRemovedTree = true;
      break;
    default:
      // By default ignore units contained in tree.
      assert(!wasUnit);
      e->removeTree();
      didRemovedTree = true;
      break;
  }
  assert(didRemovedTree != wasUnit);
  return didRemovedTree;
}

bool IsPureAngleUnit(const Tree* e) {
  return e->isUnit() &&
         Unit::GetRepresentative(e)->siVector() == Angle::Dimension;
}

bool IsVolumeAndHasOnlyVolumeUnits(const Tree* e) {
  if (Dimension::Get(e).unit.vector != Volume::Dimension) {
    return false;
  }
  bool hasOnlyVolumeUnits = false;
  for (const Tree* child : e->selfAndDescendants()) {
    if (child->isPhysicalConstant()) {
      return false;
    }
    if (child->isUnit()) {
      if (Dimension::Get(child)
              .unit.representative->representativesOfSameDimension() ==
          &Volume::representatives.liter) {
        hasOnlyVolumeUnits = true;
      } else {
        return false;
      }
    }
  }
  return hasOnlyVolumeUnits;
}

/* TODO_PCJ: Added temperature unit used to depend on the input (5°C should
 *           output 5°C, 41°F should output 41°F). */
void Unit::ApplyAutomaticDisplay(Tree* e, TreeRef& inputUnits,
                                 Dimension dimension, UnitDisplay unitDisplay) {
  assert(dimension.isUnit() && !e->isUndefined());
  Units::SIVector vector = dimension.unit.vector;
  assert(!vector.isEmpty());
  TreeRef units;
  double value = Approximation::To<double>(e, Approximation::Parameters{});
  if (dimension.isAngleUnit()) {
    if (IsPureAngleUnit(inputUnits)) {
      // Keep input representative
      e->moveTreeOverTree(ExactConvertToUnit(e, inputUnits));
    } else {
      // Return base unit
      inputUnits->moveTreeOverTree(GetBaseUnits(vector));
      e->moveTreeOverTree(ExactConvertToUnit(e, inputUnits));
    }
    inputUnits->removeTree();
    return;
  } else {
    if (IsVolumeAndHasOnlyVolumeUnits(inputUnits)) {
      // Use volume representative
      units = Push(&Volume::representatives.liter);
      value = value / GetRepresentative(units)->ratio();
    } else {
      units = SharedTreeStack->pushMult(2);
      ChooseBestDerivedUnits(&vector);
      GetBaseUnits(vector);
      NAry::Flatten(units);
      NAry::SquashIfPossible(units);
    }
    if (unitDisplay != UnitDisplay::AutomaticPrefixFreeMetric) {
      ChooseBestRepresentativeAndPrefixForValue(
          units, &value,
          unitDisplay == UnitDisplay::AutomaticMetric ? UnitFormat::Metric
                                                      : UnitFormat::Imperial);
    }
  }
  TreeRef approximated = SharedTreeStack->pushDoubleFloat(value);
  e->moveTreeOverTree(PatternMatching::Create(
      KMult(KA, KB), {.KA = approximated, .KB = units}));
  approximated->removeTree();
  units->removeTree();
  inputUnits->removeTree();
}

bool IsCombinationOfUnits(const Tree* e) {
  if (e->isUnit()) {
    return true;
  }
  if (e->isMult() || e->isDiv()) {
    return !e->hasChildSatisfying(
        [](const Tree* e) { return !IsCombinationOfUnits(e); });
  }
  if (e->isPow()) {
    return IsCombinationOfUnits(e->child(0));
  }
  return false;
}

bool HasUnit(const Tree* e) {
  // TODO should HasUnit be replaced by dimensional analysis ?
  return e->hasDescendantSatisfying(
      [](const Tree* e) { return e->isUnitOrPhysicalConstant(); });
}

// Use only one of the input unit and converts e to it.
bool Unit::ApplyAutomaticInputDisplay(Tree* e, TreeRef& inputUnits) {
  /* TODO: Select the best possible choice if there are multiple units
           With _mm*_Hz+(_m+_km)*_s^-1 : _mm*_Hz, _m_s^-1 or _km_s^-1 ?
  */
  RemoveNonUnits(inputUnits, false);
  if (inputUnits->isUnit() &&
      IsNonKelvinTemperature(GetRepresentative(inputUnits))) {
    // Handle non kelvin temperature conversion separately.
    e->moveTreeOverTree(
        SharedTreeStack->pushDoubleFloat(KelvinValueToRepresentative(
            Approximation::To<double>(e, Approximation::Parameters{}),
            GetRepresentative(inputUnits))));
    e->cloneNodeAtNode(KMult.node<2>);
    return true;
  }
  // Multiply e, inputUnits and inverse of inputUnits's SI value.
  e->moveTreeOverTree(ExactConvertToUnit(e, inputUnits));
  inputUnits->removeTree();
  return true;
}

// Use an equivalent representative for surface and volumes
bool Unit::ApplyEquivalentDisplay(Tree* e, TreeRef& inputUnits,
                                  Dimension dimension, UnitFormat unitFormat) {
  SIVector vector = dimension.unit.vector;
  bool isSurface = vector.supportSize() == 1 && vector.distance == 2;
  bool isVolume = vector.supportSize() == 1 && vector.distance == 3;
  bool isAngle = vector.supportSize() == 1 && vector.angle == 1;
  bool isTemperature = vector.supportSize() == 1 && vector.temperature == 1;
  bool isSpeedWithHour =
      vector.supportSize() == 2 && vector.distance == 1 && vector.time == -1 &&
      inputUnits->hasDescendantSatisfying([](const Tree* e) {
        return e->isUnit() &&
               GetRepresentative(e) == &Time::representatives.hour;
      });
  if (!isSurface && !isVolume && !isAngle && !isTemperature &&
      !isSpeedWithHour) {
    inputUnits->removeTree();
    return false;
  }

  bool hasDistanceUnit =
      (isSurface || isVolume) &&
      inputUnits->hasDescendantSatisfying([](const Tree* e) {
        return e->isUnit() &&
               GetRepresentative(e)->siVector() == Distance::Dimension;
      });
  bool isImperial = unitFormat == UnitFormat::Imperial;

  inputUnits->removeTree();
  const Representative* targetRepresentative;
  Tree* units;
  bool optimizePrefix = true;
  if (isTemperature) {
    // Equivalence to °C and °F
    targetRepresentative = isImperial ? &Temperature::representatives.fahrenheit
                                      : &Temperature::representatives.celsius;
    units = Push(targetRepresentative, Prefix::EmptyPrefix());
    assert(e->nextTree() == units);
    e->moveTreeOverTree(
        SharedTreeStack->pushDoubleFloat(KelvinValueToRepresentative(
            Approximation::To<double>(e, Approximation::Parameters{}),
            targetRepresentative)));
    e->cloneNodeAtNode(KMult.node<2>);
    return true;
  } else if (isAngle) {
    // Equivalence to °
    targetRepresentative = &Angle::representatives.degree;
    units = Push(targetRepresentative, Prefix::EmptyPrefix());
    optimizePrefix = false;
  } else if (isSpeedWithHour) {
    // Equivalence to mph and km/h
    targetRepresentative = isImperial ? &Distance::representatives.mile
                                      : &Distance::representatives.meter;
    const Prefix* distancePrefix = isImperial
                                       ? Prefix::EmptyPrefix()
                                       : &Prefix::Prefixes()[k_kiloPrefixIndex];
    units = KMult.node<2>->cloneNode();
    Push(targetRepresentative, distancePrefix);
    KPow->cloneNode();
    Push(&Time::representatives.hour, Prefix::EmptyPrefix());
    Integer::Push(-1);
    optimizePrefix = false;
  } else if (isSurface || isVolume) {
    // TODO: Maybe handle intermediary cases where multiple units are involved.
    // L <--> m3, ha <--> m2, gal <--> ft3, acre <--> ft2
    if (hasDistanceUnit) {
      if (isVolume) {
        targetRepresentative = isImperial ? &Volume::representatives.gallon
                                          : &Volume::representatives.liter;
      } else {
        targetRepresentative = isImperial ? &Surface::representatives.acre
                                          : &Surface::representatives.hectare;
      }
      units = Push(targetRepresentative, Prefix::EmptyPrefix());
    } else {
      targetRepresentative = isImperial ? &Distance::representatives.foot
                                        : &Distance::representatives.meter;
      // Only display imperial areas in ft^2
      optimizePrefix = isVolume || !isImperial;
      units = KPow->cloneNode();
      Push(targetRepresentative, Prefix::EmptyPrefix());
      Integer::Push(isVolume ? 3 : 2);
    }
  } else {
    OMG::unreachable();
  }
  assert(e->nextTree() == units);
  // Select best prefix
  double value = Approximation::To<double>(e, Approximation::Parameters{}) /
                 Approximation::To<double>(units, Approximation::Parameters{});
  if (targetRepresentative == &Surface::representatives.acre && value < 1.0f) {
    // Do not display values of less than 1 acre
    units->removeTree();
    return false;
  }
  if (optimizePrefix) {
    ChooseBestRepresentativeAndPrefixForValueOnSingleUnit(units, &value,
                                                          unitFormat, false);
  }
  e->moveTreeOverTree(SharedTreeStack->pushDoubleFloat(value));
  // Multiply e with units
  e->cloneNodeAtNode(KMult.node<2>);
  return true;
}

/* Round value to max displayed precision :
 * With a value of 10.0000000000001, err is 10^12 and value is rounded to 10 */
double roundToMaxPrecision(double value) {
  if (value == 0.0) {
    return value;
  }
  double err = std::pow(10.0, PrintFloat::k_maxNumberOfSignificantDigits -
                                  (std::floor(log10(std::fabs(value))) + 1));
  return std::round(value * err) / err;
}

// Given a SI value, build decomposition along multiple representatives
Tree* BuildDecomposition(double value, const Representative* const* list,
                         int length) {
  assert(length > 0);
  Tree* result = SharedTreeStack->pushAdd(0);
  double smallestRatio = list[length - 1]->ratio();
  // Instead of SI, use value at smallest representative
  value = roundToMaxPrecision(value / smallestRatio);

  for (int i = 0; i < length; i++) {
    bool lastUnit = i == length - 1;
    const Representative* representative = list[i];
    // Compute ratio from smallest representative to this one
    double ratio = std::round(representative->ratio() / smallestRatio);
    // Representatives must be ordered and have the same dimension.
    assert(i == 0 || (list[i]->ratio() < list[i - 1]->ratio()) &&
                         list[i]->siVector() == list[i - 1]->siVector());
    double representativeValue = value / ratio;
    double lax = OMG::Float::EpsilonLax<double>() / ratio;
    if (!lastUnit) {
      // 1.6 -> 1 and -1.6 -> -1
      representativeValue = (representativeValue > 0.0)
                                ? std::floor(representativeValue + lax)
                                : std::ceil(representativeValue - lax);
    }
    assert(!lastUnit || result->numberOfChildren() > 0);
    if (std::abs(representativeValue) > lax) {
      // Add decomposed unit
      SharedTreeStack->pushMult(2);
      SharedTreeStack->pushFloat(representativeValue);
      Unit::Push(list[i]);
      NAry::SetNumberOfChildren(result, result->numberOfChildren() + 1);
    }
    // Round at each step to prevent accumulation of floating point errors.
    value = roundToMaxPrecision(value - ratio * representativeValue);
  }
  assert(std::abs(value) <= OMG::Float::EpsilonLax<double>() &&
         result->numberOfChildren() > 0);
  NAry::SquashIfUnary(result);
  return result;
}

// Decomposition representatives list, ordered from biggest to smallest ratio.
constexpr const Representative* const timeRepresentativesList[] = {
    &Time::representatives.year,   &Time::representatives.month,
    &Time::representatives.day,    &Time::representatives.hour,
    &Time::representatives.minute, &Time::representatives.second};

constexpr const Representative* const angleRepresentativesList[] = {
    &Angle::representatives.degree, &Angle::representatives.arcMinute,
    &Angle::representatives.arcSecond};

constexpr const Representative* const massRepresentativesList[] = {
    &Mass::representatives.shortTon, &Mass::representatives.pound,
    &Mass::representatives.ounce};

constexpr const Representative* const distanceRepresentativesList[] = {
    &Distance::representatives.mile, &Distance::representatives.yard,
    &Distance::representatives.foot, &Distance::representatives.inch};

constexpr const Representative* const volumeRepresentativesList[] = {
    &Volume::representatives.gallon, &Volume::representatives.quart,
    &Volume::representatives.pint, &Volume::representatives.cup};

bool Unit::ApplyDecompositionDisplay(Tree* e, TreeRef& inputUnits,
                                     Dimension dimension,
                                     UnitFormat unitFormat) {
  // Decompose time, angle, and imperial volume, mass and length
  SIVector vector = dimension.unit.vector;
  const Representative* const* list = nullptr;
  int length;
  if (vector == Time::Dimension) {
    list = timeRepresentativesList;
    length = std::size(timeRepresentativesList);
  } else if (vector == Angle::Dimension) {
    list = angleRepresentativesList;
    length = std::size(angleRepresentativesList);
  } else if (unitFormat == UnitFormat::Imperial &&
             DisplayImperialUnitsInOutput(inputUnits)) {
    if (vector == Mass::Dimension) {
      list = massRepresentativesList;
      length = std::size(massRepresentativesList);
    } else if (vector == Distance::Dimension) {
      list = distanceRepresentativesList;
      length = std::size(distanceRepresentativesList);
    } else if (vector == Volume::Dimension) {
      list = volumeRepresentativesList;
      length = std::size(volumeRepresentativesList);
    }
  }
  if (!list) {
    inputUnits->removeTree();
    return false;
  }
  double value = Approximation::To<double>(e, Approximation::Parameters{});
  // Skip decomposition if value is smaller than second smallest representative.
  assert(length > 1);
  if (std::isnan(value) || std::isinf(value) ||
      std::abs(value) <= list[length - 2]->ratio()) {
    inputUnits->removeTree();
    return false;
  }
  // inputUnits are no longer necessary
  inputUnits->removeTree();
  e->moveTreeOverTree(BuildDecomposition(value, list, length));
  return true;
}

}  // namespace Units
}  // namespace Poincare::Internal
