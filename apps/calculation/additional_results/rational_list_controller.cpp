#include "rational_list_controller.h"

#include <apps/shared/poincare_helpers.h>
#include <poincare/src/expression/integer.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/rational.h>
#include <poincare/src/memory/pattern_matching.h>
#include <string.h>

#include "../app.h"

using namespace Poincare;
using namespace Poincare::Internal;
using namespace Shared;

namespace Calculation {

IntegerHandler extractInteger(const Tree* e) {
  /* TODO_PCJ: is this usage of IntegerHandler correct ?
   * A quick experiment showed incorrect digits with large numbers ! */
  if (e->isOpposite()) {
    IntegerHandler i = extractInteger(e->child(0));
    i.setSign(InvertSign(i.sign()));
    return i;
  }
  assert(e->isInteger());
  return Integer::Handler(e);
}

static bool isIntegerInput(Expression e) {
  return (e.type() == ExpressionNode::Type::BasedInteger ||
          (e.type() == ExpressionNode::Type::Opposite &&
           isIntegerInput(e.childAtIndex(0))));
}

static bool isFractionInput(Expression e) {
  if (e.type() == ExpressionNode::Type::Opposite) {
    return isFractionInput(e.childAtIndex(0));
  }
  if (e.type() != ExpressionNode::Type::Division) {
    return false;
  }
  Expression num = e.childAtIndex(0);
  Expression den = e.childAtIndex(1);
  return isIntegerInput(num) && isIntegerInput(den);
}

// Take a rational a/b and create the euclidian division a=b*q+r
static Tree* CreateEuclideanDivision(const Tree* e) {
  IntegerHandler num = Rational::Numerator(e);
  IntegerHandler den = Rational::Denominator(e);
  DivisionResult<Tree*> division = IntegerHandler::Division(num, den);
  TreeRef numTree = num.pushOnTreeStack();
  TreeRef denTree = den.pushOnTreeStack();
  TreeRef result = PatternMatching::Create(KEqual(KA, KAdd(KMult(KB, KC), KD)),
                                           {.KA = numTree,
                                            .KB = denTree,
                                            .KC = division.quotient,
                                            .KD = division.remainder});
  denTree->removeTree();
  numTree->removeTree();
  division.remainder->removeTree();
  division.quotient->removeTree();
  return result;
}

void RationalListController::computeAdditionalResults(
    const UserExpression input, const UserExpression exactOutput,
    const UserExpression approximateOutput) {
  assert(AdditionalResultsType::HasRational(exactOutput));
  Expression e = isFractionInput(input) ? input : exactOutput;
  assert(!e.isUninitialized());
  static_assert(k_maxNumberOfRows >= 2,
                "k_maxNumberOfRows must be greater than 2");

  bool negative = e.type() == ExpressionNode::Type::Opposite;
  Expression div = negative ? e.childAtIndex(0) : e;
  assert(div.type() == ExpressionNode::Type::Division);
  IntegerHandler numerator = extractInteger(div.childAtIndex(0));
  if (negative) {
    numerator.setSign(InvertSign(numerator.sign()));
  }
  IntegerHandler denominator = extractInteger(div.childAtIndex(1));
  Expression rational =
      Expression::Builder(Rational::Push(numerator, denominator));
  SystemExpression mixedFraction =
      SystemExpression::Builder(Rational::CreateMixedFraction(
          rational,
          GlobalPreferences::SharedGlobalPreferences()->mixedFractions() ==
              Preferences::MixedFractions::Enabled));
  SystemExpression euclideanDiv =
      SystemExpression::Builder(CreateEuclideanDivision(rational));

  int index = 0;
  m_layouts[index++] = PoincareHelpers::CreateLayout(
      mixedFraction.cloneAndBeautify({}), App::app()->localContext());
  m_layouts[index++] = PoincareHelpers::CreateLayout(
      euclideanDiv.cloneAndBeautify({}), App::app()->localContext());
}

I18n::Message RationalListController::messageAtIndex(int index) {
  switch (index) {
    case 0:
      return I18n::Message::MixedFraction;
    default:
      return I18n::Message::EuclideanDivision;
  }
}

Poincare::Layout RationalListController::layoutAtIndex(
    Escher::HighlightCell* cell, int index) {
  return ExpressionsListController::layoutAtIndex(cell, index);
#if 0  // TODO_PCJ
  if (index == 1) {
    // Get rid of the left part of the equality
    char *equalPosition = strchr(buffer, '=');
    assert(equalPosition != nullptr);
    strlcpy(buffer, equalPosition + 1, bufferSize);
    return buffer + length - 1 - equalPosition;
  }
  return length;
#endif
}

}  // namespace Calculation
