#include <assert.h>
#include <float.h>
#include <ion.h>
#include <limits.h>
#include <omg/float.h>
#include <poincare/expression.h>
#include <poincare/k_tree.h>
#include <poincare/old/addition.h>
#include <poincare/old/arc_cosine.h>
#include <poincare/old/arc_sine.h>
#include <poincare/old/arc_tangent.h>
#include <poincare/old/constant.h>
#include <poincare/old/cosecant.h>
#include <poincare/old/cosine.h>
#include <poincare/old/cotangent.h>
#include <poincare/old/decimal.h>
#include <poincare/old/dependency.h>
#include <poincare/old/derivative.h>
#include <poincare/old/division.h>
#include <poincare/old/multiplication.h>
#include <poincare/old/opposite.h>
#include <poincare/old/parenthesis.h>
#include <poincare/old/power.h>
#include <poincare/old/rational.h>
#include <poincare/old/secant.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/sign_function.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/sine.h>
#include <poincare/old/subtraction.h>
#include <poincare/old/symbol.h>
#include <poincare/old/tangent.h>
#include <poincare/old/trigonometry.h>
#include <poincare/old/undefined.h>
#include <poincare/old/unit.h>
#include <poincare/preferences.h>
#include <poincare/src/expression/units/representatives.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/memory/tree.h>

#include <cmath>

namespace Poincare {

/* The values must be in the order defined in poincare/preferences:
 * Radians / Degrees / Gradians */

UserExpression Trigonometry::PiExpressionInAngleUnit(
    Preferences::AngleUnit angleUnit) {
  switch (angleUnit) {
    case Preferences::AngleUnit::Radian:
      return Constant::PiBuilder();
      ;
    case Preferences::AngleUnit::Degree:
      return Rational::Builder(180);
    default:
      assert(angleUnit == Preferences::AngleUnit::Gradian);
      return Rational::Builder(200);
  }
}

UserExpression Trigonometry::UnitConversionFactor(
    Preferences::AngleUnit fromUnit, Preferences::AngleUnit toUnit) {
  if (fromUnit == toUnit) {
    // Just an optimisation to gain some time at reduction
    return Rational::Builder(1);
  }
  return Division::Builder(PiExpressionInAngleUnit(toUnit),
                           PiExpressionInAngleUnit(fromUnit));
}

// TODO_PCJ: Delete these method
#if 0
Expression Trigonometry::ReplaceWithAdvancedFunction(Expression& e,
                                                     Expression& denominator) {
  /* Replace direct trigonometric function with their advanced counterpart.
   * This function must be called within a denominator. */
  assert(e.type() == ExpressionNode::Type::Power &&
         !denominator.isUninitialized());
  assert(denominator.tree()->isDirectTrigonometryFunction());
  Expression result;
  switch (denominator.type()) {
    case ExpressionNode::Type::Cosine:
      result = Secant::Builder(denominator.childAtIndex(0));
      break;
    case ExpressionNode::Type::Sine:
      result = Cosecant::Builder(denominator.childAtIndex(0));
      break;
    default:
      assert(denominator.type() == ExpressionNode::Type::Tangent);
      result = Cotangent::Builder(denominator.childAtIndex(0));
      break;
  }
  e.replaceWithInPlace(result);
  return result;
}
#endif

static void AddAngleUnitToDirectFunctionIfNeeded(
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

void PrivateDeepAddAngleUnitToAmbiguousDirectFunctions(
    Internal::Tree* e, Preferences::AngleUnit angleUnit) {
  if (e->isDirectTrigonometryFunction() ||
      e->isDirectAdvancedTrigonometryFunction()) {
    return AddAngleUnitToDirectFunctionIfNeeded(e, angleUnit);
  }
  for (Internal::Tree* child : e->children()) {
    PrivateDeepAddAngleUnitToAmbiguousDirectFunctions(child, angleUnit);
  }
}

void Trigonometry::DeepAddAngleUnitToAmbiguousDirectFunctions(
    UserExpression& e, Preferences::AngleUnit angleUnit) {
  Internal::Tree* clone = e.tree()->cloneTree();
  PrivateDeepAddAngleUnitToAmbiguousDirectFunctions(clone, angleUnit);
  e = UserExpression::Builder(clone);
  return;
}

}  // namespace Poincare
