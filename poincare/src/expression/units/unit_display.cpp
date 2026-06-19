// TODO: replace with config flavor
#if !POINCARE_UNIT_ONLY_ANGLES

#include "unit_display.h"

#include <omg/float.h>
#include <poincare/print_float.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/properties.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/memory/tree_stack.h>

#include "../approximation.h"
#include "../dimension.h"
#include "../integer.h"
#include "../systematic_reduction.h"
#include "best_unit_selection.h"
#include "helpers.h"
#include "prefix.h"
#include "representative.h"
#include "representatives.h"
#include "temperature.h"
#include "unit.h"

namespace Poincare::Internal {
namespace Units {

static bool HasPhysicalConstant(const Tree* e) {
  return e->hasDescendantSatisfying(
      [](const Tree* e) { return e->isPhysicalConstant(); });
}

// Find one or two units in expression, return false if there are more than two
static bool GetUnits(Tree* extractedUnits, Tree** unit1, Tree** unit2) {
  assert(!*unit1 && !*unit2);
  for (Tree* e : extractedUnits->selfAndDescendants()) {
    if (e->isUnitOrPhysicalConstant()) {
      if (*unit2) {
        assert(*unit1);
        return false;
      }
      (*unit1 ? *unit2 : *unit1) = e;
    }
  }
  assert(*unit1);
  return true;
}

static void ApplyMainOutputDisplay(Tree* e, TreeRef& inputUnits,
                                   Dimension dimension, AngleUnit angleUnit);
static void ApplyAutomaticDisplay(Tree* e, TreeRef& inputUnits,
                                  Dimension dimension, UnitDisplay unitDisplay);
static bool ApplyAutomaticInputDisplay(Tree* e, TreeRef& inputUnits);
static bool ApplyEquivalentDisplay(Tree* e, TreeRef& inputUnits,
                                   Dimension dimension,
                                   Preferences::UnitFormat unitFormat);
static bool ApplyDecompositionDisplay(Tree* e, TreeRef& inputUnits,
                                      Dimension dimension,
                                      Preferences::UnitFormat unitFormat);

// From a projected tree, gather units and use best representatives/prefixes.
bool ProjectToBestUnits(Tree* e, Dimension dimension,
                        const UnitParameters& unitParameters) {
  if (unitParameters.unitDisplay == UnitDisplay::None &&
      !e->isUnitConversion()) {
    // TODO_PCJ : Remove units that cancel themselves
    return false;
  }
  if (!dimension.isUnit()) {
    // There may remain units that cancel themselves, remove them.
    return Tree::ApplyShallowTopDown(e, Unit::ShallowRemoveUnit);
  }
  UnitDisplay unitDisplay = unitParameters.unitDisplay;
  if (HasPhysicalConstant(e) &&
      !e->hasDescendantSatisfying([](const Tree* e) { return e->isUnit(); }) &&
      unitParameters.unitDisplay != UnitDisplay::MainOutput) {
    // Display only SI and derived units for inputs with physical constants only
    unitDisplay = UnitDisplay::BasicSI;
  }
  TreeRef extractedUnits = e->cloneTree();
  if (e->isUnitConversion()) {
    // Use second child for target units and first one for value.
    extractedUnits->moveTreeOverTree(extractedUnits->child(1));
    e->moveTreeOverTree(e->child(0));
    unitDisplay = UnitDisplay::AutomaticInput;
    assert(!Dimension::Get(e).isScalar());
  }
  if (IsNonKelvinTemperature(dimension.unit.representative)) {
    // Temperature units must be removed from root expression
    RemoveTemperatureUnitFromRoot(e);
  }
  // Turn e into its SI value. 2_m + _yd -> 3.8288
  Tree::ApplyShallowTopDown(e, Unit::ShallowRemoveUnit);
  /* Reduce the dimension-less expression, this could be skipped since e will
   * just be approximated, but it allows better approximation, especially when
   * the expected result is 0. */
  SystematicReduction::DeepReduce(e);
  if (GetComplexProperties(e).isNonReal()) {
    // Forbid complex numbers with units
    extractedUnits->removeTree();
    e->cloneTreeOverTree(KUndefUnit);
    return true;
  }
  assert(extractedUnits && e->nextTree() == extractedUnits);
  [[maybe_unused]] bool treeRemoved =
      Unit::RemoveNonUnits(extractedUnits, true);
  // Warning : extractedUnits isn't just e's dimension. 2_m + _yd -> _m + _yd
  assert(!treeRemoved);
  // Take advantage of e being last tree.
  assert(extractedUnits);
  assert(dimension.unit.vector == Dimension::Get(extractedUnits).unit.vector);
  bool foundResult = true;
  switch (unitDisplay) {
    case UnitDisplay::Forbidden:
      extractedUnits->removeTree();
      foundResult = false;
      e->cloneTreeOverTree(KUndefUnhandledDimension);
      return true;
    case UnitDisplay::MainOutput:
      ApplyMainOutputDisplay(e, extractedUnits, dimension,
                             unitParameters.angleUnit);
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
      foundResult = ApplyEquivalentDisplay(e, extractedUnits, dimension,
                                           unitParameters.unitFormat);
      break;
    case UnitDisplay::Decomposition:
      foundResult = ApplyDecompositionDisplay(e, extractedUnits, dimension,
                                              unitParameters.unitFormat);
      break;
    case UnitDisplay::BasicSI:
      extractedUnits->moveTreeOverTree(
          Unit::GetBaseUnits(dimension.unit.vector));
      assert(e->nextTree() == extractedUnits);
      e->cloneNodeAtNode(KMult.node<2>);
      break;
    case UnitDisplay::None:
      OMG_UNREACHABLE;
  }
  if (!foundResult) {
    e->cloneTreeOverTree(KUndefUnit);
  }
  return true;
}

// Return true if Imperial units should be used in output
static bool DisplayImperialUnitsInOutput(const Tree* inputUnits) {
  bool hasImperialUnits = false;
  for (const Tree* e : inputUnits->selfAndDescendants()) {
    if (e->isUnit()) {
      if (Unit::GetRepresentative(e)->isImperial()) {
        hasImperialUnits = true;
      } else if (Unit::GetRepresentative(e) != &Volume::representatives.liter) {
        // Use metric representatives if any other metric unit is involved.
        return false;
      }
      // Liter representative is tolerated for imperial unit input.
    }
  }
  return hasImperialUnits;
}

static void ApplyMainOutputDisplay(Tree* e, TreeRef& inputUnits,
                                   Dimension dimension, AngleUnit angleUnit) {
  if (dimension.isAngleUnit()) {
    // Replace inputUnits to target angle unit.
    Tree* newExtractedUnits = KPow->cloneNode();
    Unit::Push(angleUnit);
    Integer::Push(dimension.unit.vector.angle);
    assert(Dimension::Get(newExtractedUnits) == Dimension::Get(inputUnits));
    inputUnits->moveTreeOverTree(newExtractedUnits);
    e->moveTreeOverTree(Unit::ExactConvertToUnit(e, inputUnits));
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
    double value = Approximation::To<double>(
        e, Approximation::Parameters{.policy = {.optimize = true}});
    if (IsNonKelvinTemperature(Unit::GetRepresentative(unit1))) {
      value = TemperatureFromKelvin(value, Unit::GetRepresentative(unit1));
    } else {
      // Correct the value since e is in basic SI
      value /= Approximation::To<double>(
          inputUnits, Approximation::Parameters{.policy = {.optimize = true}});
    }
    if (std::isnan(value)) {
      // Return undef to avoid displaying undef * unit
      inputUnits->removeTree();
      e->cloneTreeOverTree(KUndefUnit);
      return;
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

static bool IsVolumeAndHasOnlyVolumeUnits(const Tree* e) {
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
static void ApplyAutomaticDisplay(Tree* e, TreeRef& inputUnits,
                                  Dimension dimension,
                                  UnitDisplay unitDisplay) {
  assert(dimension.isUnit() && !e->isUndefined());
  Units::SIVector vector = dimension.unit.vector;
  assert(!vector.isEmpty());
  TreeRef units;
  double value = Approximation::To<double>(
      e, Approximation::Parameters{.policy = {.optimize = true}});
  if (dimension.isAngleUnit()) {
    if (IsPureAngleUnit(inputUnits)) {
      // Keep input representative
      e->moveTreeOverTree(Unit::ExactConvertToUnit(e, inputUnits));
    } else {
      // Return base unit
      inputUnits->moveTreeOverTree(Unit::GetBaseUnits(vector));
      e->moveTreeOverTree(Unit::ExactConvertToUnit(e, inputUnits));
    }
    inputUnits->removeTree();
    return;
  } else {
    if (IsVolumeAndHasOnlyVolumeUnits(inputUnits)) {
      // Use volume representative
      units = Unit::Push(&Volume::representatives.liter);
      value = value / Unit::GetRepresentative(units)->ratio();
    } else {
      units = SharedTreeStack->pushMult(2);
      ChooseBestDerivedUnits(&vector);
      Unit::GetBaseUnits(vector);
      NAry::Flatten(units);
      NAry::SquashIfPossible(units);
    }
    if (unitDisplay != UnitDisplay::AutomaticPrefixFreeMetric) {
      ChooseBestRepresentativeAndPrefixForValue(
          units, &value,
          unitDisplay == UnitDisplay::AutomaticMetric
              ? Preferences::UnitFormat::Metric
              : Preferences::UnitFormat::Imperial);
    }
  }
  TreeRef approximated = SharedTreeStack->pushDoubleFloat(value);
  e->moveTreeOverTree(PatternMatching::Create(
      KMult(KA, KB), {.KA = approximated, .KB = units}));
  approximated->removeTree();
  units->removeTree();
  inputUnits->removeTree();
}

// Use only one of the input unit and converts e to it.
static bool ApplyAutomaticInputDisplay(Tree* e, TreeRef& inputUnits) {
  /* TODO: Select the best possible choice if there are multiple units
           With _mm*_Hz+(_m+_km)*_s^-1 : _mm*_Hz, _m_s^-1 or _km_s^-1 ?
  */
  [[maybe_unused]] bool treeRemoved = Unit::RemoveNonUnits(inputUnits, false);
  assert(!treeRemoved);
  if (inputUnits->isUnit() &&
      IsNonKelvinTemperature(Unit::GetRepresentative(inputUnits))) {
    // Handle non kelvin temperature conversion separately.
    e->moveTreeOverTree(SharedTreeStack->pushDoubleFloat(TemperatureFromKelvin(
        Approximation::To<double>(
            e, Approximation::Parameters{.policy = {.optimize = true}}),
        Unit::GetRepresentative(inputUnits))));
    e->cloneNodeAtNode(KMult.node<2>);
    return true;
  }
  // Multiply e, inputUnits and inverse of inputUnits's SI value.
  e->moveTreeOverTree(Unit::ExactConvertToUnit(e, inputUnits));
  inputUnits->removeTree();
  return true;
}

// Use an equivalent representative for surface and volumes
static bool ApplyEquivalentDisplay(Tree* e, TreeRef& inputUnits,
                                   Dimension dimension,
                                   Preferences::UnitFormat unitFormat) {
  SIVector vector = dimension.unit.vector;
  bool isSurface = vector.supportSize() == 1 && vector.distance == 2;
  bool isVolume = vector.supportSize() == 1 && vector.distance == 3;
  bool isAngle = vector.supportSize() == 1 && vector.angle == 1;
  bool isTemperature = vector.supportSize() == 1 && vector.temperature == 1;
  bool isSpeedWithHour =
      vector.supportSize() == 2 && vector.distance == 1 && vector.time == -1 &&
      inputUnits->hasDescendantSatisfying([](const Tree* e) {
        return e->isUnit() &&
               Unit::GetRepresentative(e) == &Time::representatives.hour;
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
               Unit::GetRepresentative(e)->siVector() == Distance::Dimension;
      });
  bool isImperial = unitFormat == Preferences::UnitFormat::Imperial;

  inputUnits->removeTree();
  const Representative* targetRepresentative;
  Tree* units;
  bool optimizePrefix = true;
  if (isTemperature) {
    // Equivalence to °C and °F
    targetRepresentative = isImperial ? &Temperature::representatives.fahrenheit
                                      : &Temperature::representatives.celsius;
    units = Unit::Push(targetRepresentative, Prefix::EmptyPrefix());
    assert(e->nextTree() == units);
    e->moveTreeOverTree(SharedTreeStack->pushDoubleFloat(TemperatureFromKelvin(
        Approximation::To<double>(
            e, Approximation::Parameters{.policy = {.optimize = true}}),
        targetRepresentative)));
    e->cloneNodeAtNode(KMult.node<2>);
    return true;
  } else if (isAngle) {
    // Equivalence to °
    targetRepresentative = &Angle::representatives.degree;
    units = Unit::Push(targetRepresentative, Prefix::EmptyPrefix());
    optimizePrefix = false;
  } else if (isSpeedWithHour) {
    // Equivalence to mph and km/h
    targetRepresentative = isImperial ? &Distance::representatives.mile
                                      : &Distance::representatives.meter;
    const Prefix* distancePrefix =
        isImperial ? Prefix::EmptyPrefix()
                   : &Prefix::Prefixes()[Prefix::k_kiloPrefixIndex];
    units = KMult.node<2>->cloneNode();
    Unit::Push(targetRepresentative, distancePrefix);
    KPow->cloneNode();
    Unit::Push(&Time::representatives.hour, Prefix::EmptyPrefix());
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
      units = Unit::Push(targetRepresentative, Prefix::EmptyPrefix());
    } else {
      targetRepresentative = isImperial ? &Distance::representatives.foot
                                        : &Distance::representatives.meter;
      // Only display imperial areas in ft^2
      optimizePrefix = isVolume || !isImperial;
      units = KPow->cloneNode();
      Unit::Push(targetRepresentative, Prefix::EmptyPrefix());
      Integer::Push(isVolume ? 3 : 2);
    }
  } else {
    OMG_UNREACHABLE;
  }
  assert(e->nextTree() == units);
  // Select best prefix
  double value =
      Approximation::To<double>(
          e, Approximation::Parameters{.policy = {.optimize = true}}) /
      Approximation::To<double>(
          units, Approximation::Parameters{.policy = {.optimize = true}});
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

static bool ApplyDecompositionDisplay(Tree* e, TreeRef& inputUnits,
                                      Dimension dimension,
                                      Preferences::UnitFormat unitFormat) {
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
  } else if (unitFormat == Preferences::UnitFormat::Imperial &&
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
  double value = Approximation::To<double>(
      e, Approximation::Parameters{.policy = {.optimize = true}});
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

#endif
