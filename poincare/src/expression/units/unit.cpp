#include "unit.h"

#include <poincare/src/expression/k_tree.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/memory/tree_stack.h>

#include "../approximation.h"
#include "../dimension.h"
#include "../integer.h"
#include "../parametric.h"
#include "../physical_constant.h"
#include "prefix.h"
#include "representative.h"
#include "representatives.h"

namespace Poincare::Internal {
namespace Units {

/* This function tries to simplify a unit ('unitCoefficients') by multiplying
 * with another unit ('simplifyingUnitCoefficients') at a given exponent
 * ('simplifyingUnitExponent'). If the result of the operation is simpler,
 * 'unitSupportSize', 'bestRemainderExponents' and 'bestRemainderSupportSize'
 * are updated accordingly. */
static bool CanSimplifyUnitProduct(const SIVector* unitCoefficients,
                                   size_t& unitSupportSize,
                                   const SIVector* simplifyingUnitCoefficients,
                                   int simplifyingUnitExponent,
                                   SIVector& bestRemainderExponents,
                                   size_t& bestRemainderSupportSize) {
  SIVector simplifiedExponents;

  for (size_t i = 0; i < SIVector::k_numberOfBaseUnits; i++) {
    // Simplify unitCoefficients with base units from derived unit
    if (!simplifiedExponents.setCoefficientAtIndex(
            i, unitCoefficients->coefficientAtIndex(i) +
                   simplifyingUnitExponent *
                       simplifyingUnitCoefficients->coefficientAtIndex(i))) {
      // Unit vector overflowed
      return false;
    }
  }
  size_t simplifiedSupportSize = simplifiedExponents.supportSize();
  /* Note: A metric is considered simpler if the support size (number of
   * symbols) is reduced. A norm taking coefficients into account is possible.
   * One could use the sum of all coefficients to favor _C_s from _A_s^2.
   * However, replacing _m_s^-2 with _N_kg^-1 should be avoided. */
  bool isSimpler = (1 + simplifiedSupportSize < unitSupportSize);

  if (isSimpler) {
    bestRemainderExponents = simplifiedExponents;
    bestRemainderSupportSize = simplifiedSupportSize;
    /* unitSupportSize is updated and will be taken into
     * account in next iterations of CanSimplifyUnitProduct. */
    unitSupportSize = 1 + simplifiedSupportSize;
  }
  return isSimpler;
}

Tree* ChooseBestDerivedUnits(SIVector* unitCoefficients) {
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
  size_t unitSupportSize = unitCoefficients->supportSize();
  SIVector bestRemainderExponents;
  size_t bestRemainderSupportSize;
  while (unitSupportSize > 1) {
    const Representative* bestDim = nullptr;
    int8_t bestUnitExponent = 0;
    // Look up in the table of derived units.
    for (int i = SIVector::k_numberOfBaseUnits;
         i < Representative::k_numberOfDimensions - 1; i++) {
      const Representative* dim = Representative::DefaultRepresentatives()[i];
      const SIVector simplifyingUnitCoefficients = dim->siVector();
      // A simplification is tried by either dividing or multiplying.
      if (CanSimplifyUnitProduct(
              unitCoefficients, unitSupportSize, &simplifyingUnitCoefficients,
              -1, bestRemainderExponents, bestRemainderSupportSize)) {
        bestUnitExponent = 1;
        bestDim = dim;
      } else if (CanSimplifyUnitProduct(unitCoefficients, unitSupportSize,
                                        &simplifyingUnitCoefficients, 1,
                                        bestRemainderExponents,
                                        bestRemainderSupportSize)) {
        bestUnitExponent = -1;
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
    *unitCoefficients = bestRemainderExponents;
    unitSupportSize = bestRemainderSupportSize;
  }
  NAry::SquashIfEmpty(unitsAccu);
  return unitsAccu;
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

bool Unit::IsNameReserved(const Representative* representative) {
  return representative->siVector() == Units::Angle::Dimension;
}

// Return true if best representative and prefix have been chosen.
bool Unit::ChooseBestRepresentativeAndPrefixForValueOnSingleUnit(
    Tree* unit, double* value, Preferences::UnitFormat unitFormat,
    bool optimizeRepresentative) {
  double exponent = 1.f;
  Tree* factor = unit;
  if (factor->isPow()) {
    Tree* childExponent = factor->child(1);
    assert(factor->child(0)->isUnit());
    assert(factor->child(1)->isRational());
    exponent = Approximation::To<double>(
        childExponent, Approximation::Parameters{.policy = {.optimize = true}});
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

void Unit::ChooseBestRepresentativeAndPrefixForValue(
    Tree* units, double* value, Preferences::UnitFormat unitFormat) {
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
                                             Preferences::UnitFormat unitFormat,
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
         Approximation::To<double>(
             isCelsius ? Temperature::celsiusOrigin
                       : Temperature::fahrenheitOrigin,
             Approximation::Parameters{.policy = {.optimize = true}});
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
  assert(baseUnits->isUnit());
  SIVector vector = SIVector::Empty();
  Units::SIVector posUnitVector = Units::SIVector::Empty();
  Units::SIVector negUnitVector = Units::SIVector::Empty();
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
      float exponentFloat = Approximation::To<float>(
          exp, Approximation::Parameters{.policy = {.optimize = true}});
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
    [[maybe_unused]] bool success =
        vector.addAllCoefficients(GetRepresentative(factor)->siVector(),
                                  exponent, &posUnitVector, &negUnitVector);
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
  [[maybe_unused]] bool treeRemoved = RemoveNonUnits(targetUnit, false);
  assert(!treeRemoved);
  // Multiply e, targetUnit and inverse of targetUnit's SI value.
  Tree* unitClone = targetUnit->cloneTree();
  Tree::ApplyShallowTopDown(unitClone, Unit::ShallowRemoveUnit);
  TreeRef result =
      PatternMatching::Create(KMult(KA, KB, KPow(KC, -1_e)),
                              {.KA = e, .KB = targetUnit, .KC = unitClone});
  unitClone->removeTree();
  return result;
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
  if (e->isDep()) {
    // Ignoring Dep and DepList here
    e->cloneTreeOverTree(e->child(0));
  }
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
      /* If the above assertion breaks, it could be because DeepCheckDimensions
       * and Dimension::Get allow for a type of node that is not handled in this
       * function. */
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

}  // namespace Units
}  // namespace Poincare::Internal
