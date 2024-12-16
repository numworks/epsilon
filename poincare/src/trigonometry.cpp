#include <poincare/k_tree.h>
#include <poincare/src/expression/angle.h>
#include <poincare/src/expression/units/representatives.h>
#include <poincare/trigonometry.h>

namespace Poincare {

double Trigonometry::PiInAngleUnit(Preferences::AngleUnit angleUnit) {
  switch (angleUnit) {
    case Preferences::AngleUnit::Radian:
      return M_PI;
    case Preferences::AngleUnit::Degree:
      return 180.0;
    default:
      assert(angleUnit == Preferences::AngleUnit::Gradian);
      return 200.0;
  }
}

double Trigonometry::ConvertAngleToRadian(double angle,
                                          Preferences::AngleUnit angleUnit) {
  return angleUnit != Preferences::AngleUnit::Radian
             ? angle * M_PI / PiInAngleUnit(angleUnit)
             : angle;
}

template <typename T>
std::complex<T> Trigonometry::ConvertToRadian(
    const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  if (angleUnit != Preferences::AngleUnit::Radian) {
    return c * std::complex<T>((T)M_PI / (T)PiInAngleUnit(angleUnit));
  }
  return c;
}

template <typename T>
std::complex<T> Trigonometry::ConvertRadianToAngleUnit(
    const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  if (angleUnit != Preferences::AngleUnit::Radian) {
    return c * std::complex<T>((T)PiInAngleUnit(angleUnit) / (T)M_PI);
  }
  return c;
}

UserExpression Trigonometry::Period(Preferences::AngleUnit angleUnit) {
  return UserExpression::Builder(Internal::Angle::Period(angleUnit));
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

  if (child->isAdd() || child->isSub()) {
    child->cloneNodeAtNode(KParentheses);
  }

  Internal::TreeRef unit = Internal::Units::Unit::Push(angleUnit);

  child->moveTreeOverTree(Internal::PatternMatching::Create(
      KMult(KA, KB), {.KA = child, .KB = unit}));
  unit->removeTree();
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
  privateDeepAddAngleUnitToAmbiguousDirectFunctions(clone, angleUnit);
  e = UserExpression::Builder(clone);
  return;
}

template std::complex<float> Trigonometry::ConvertToRadian<float>(
    std::complex<float>, Preferences::AngleUnit);
template std::complex<double> Trigonometry::ConvertToRadian<double>(
    std::complex<double>, Preferences::AngleUnit);
template std::complex<float> Trigonometry::ConvertRadianToAngleUnit<float>(
    std::complex<float>, Preferences::AngleUnit);
template std::complex<double> Trigonometry::ConvertRadianToAngleUnit<double>(
    std::complex<double>, Preferences::AngleUnit);

}  // namespace Poincare
