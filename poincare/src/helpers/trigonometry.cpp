#include <poincare/helpers/trigonometry.h>
#include <poincare/k_tree.h>
#include <poincare/src/expression/angle.h>
#include <poincare/src/expression/dimension.h>
#include <poincare/src/expression/units/representatives.h>

namespace Poincare {

UserExpression Trigonometry::Period(Preferences::AngleUnit angleUnit) {
  return UserExpression::Builder(Internal::Angle::Period(angleUnit));
}

static void addAngleUnitToExpression(Internal::Tree* e,
                                     Preferences::AngleUnit angleUnit) {
  Internal::TreeRef unit = Internal::Units::Unit::Push(angleUnit);
  e->moveTreeOverTree(
      Internal::PatternMatching::Create(KMult(KA, KB), {.KA = e, .KB = unit}));
  unit->removeTree();
}

static void addAngleUnitToDirectFunctionIfNeeded(
    Internal::Tree* e, Preferences::AngleUnit angleUnit) {
  assert(e->isDirectTrigonometryFunction() ||
         e->isAdvancedTrigonometryFunction());

  Internal::Tree* child = e->child(0);

  if (child->isZero()) {
    return;
  }

  if (child->hasDescendantSatisfying([](const Internal::Tree* e) {
        switch (e->type()) {
          case Internal::Type::Add:
          case Internal::Type::Sub:
          case Internal::Type::Mult:
          case Internal::Type::Div:
          case Internal::Type::Pow:
            return false;
          default:
            return !e->isNumber();
        }
      })) {
    return;
  }

  if ((angleUnit == Preferences::AngleUnit::Radian) ==
      child->hasDescendantSatisfying(
          [](const Internal::Tree* e) { return e->isPi(); })) {
    /* Do not add angle units if the child contains Pi and the angle is in Rad
     * or if the child does not contain Pi and the angle unit is other. */
    return;
  }
  addAngleUnitToExpression(child, angleUnit);
}

static void privateDeepAddAngleUnitToAmbiguousDirectFunctions(
    Internal::Tree* e, Preferences::AngleUnit angleUnit) {
  if (e->isDirectTrigonometryFunction() ||
      e->isDirectAdvancedTrigonometryFunction()) {
    return addAngleUnitToDirectFunctionIfNeeded(e, angleUnit);
  }
  for (Internal::Tree* child : e->children()) {
    privateDeepAddAngleUnitToAmbiguousDirectFunctions(child, angleUnit);
  }
}

void Trigonometry::DeepAddAngleUnitToAmbiguousDirectFunctions(
    UserExpression& e, Preferences::AngleUnit angleUnit) {
  Internal::Tree* clone = e.tree()->cloneTree();
  if (clone->isUnitConversion() &&
      Internal::Dimension::DeepCheck(clone->child(0)) &&
      !Internal::Dimension::Get(clone->child(0)).isUnit()) {
    addAngleUnitToExpression(clone->child(0), angleUnit);
  }
  privateDeepAddAngleUnitToAmbiguousDirectFunctions(clone, angleUnit);
  e = UserExpression::Builder(clone);
}

}  // namespace Poincare
