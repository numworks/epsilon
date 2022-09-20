#ifndef POINCARE_INPUT_BEAUTIFICATION_H
#define POINCARE_INPUT_BEAUTIFICATION_H

#include <poincare/layout.h>
#include <poincare/binomial_coefficient.h>
#include <poincare/conjugate.h>
#include <poincare/derivative.h>
#include <poincare/integral.h>
#include <poincare/power.h>
#include <poincare/nth_root.h>
#include <poincare/square_root.h>
#include <poincare_layouts.h>

namespace Poincare {

class InputBeautification {
public:
 /* Returns the index of the leftest beautified layout in parent
   * returns -1 if the parent was altered. */
  static int ApplyBeautification(Layout lastAddedLayout, LayoutCursor * layoutCursor, Context * context, bool forceCursorRightOfText = false);

private:
  typedef Layout (*BeautifiedLayoutBuilder) ();
  struct BeautificationRule {
    AliasesList listOfBeautifiedAliases;
    BeautifiedLayoutBuilder layoutBuilder;
  };

  constexpr static const BeautificationRule convertWhenInputted[] = {
    // Angle units
    {"\"\"", []() { return static_cast<Layout>(CodePointLayout::Builder('\'')); }},
    {"'\"", []() { return static_cast<Layout>(CodePointLayout::Builder(UCodePointDegreeSign)); }},
    // Comparison operators
    {"<=", []() { return static_cast<Layout>(CodePointLayout::Builder(UCodePointInferiorEqual)); }},
    {">=", []() { return static_cast<Layout>(CodePointLayout::Builder(UCodePointSuperiorEqual)); }},
    // Special char
    {"->", []() { return static_cast<Layout>(CodePointLayout::Builder(UCodePointRightwardsArrow)); }},
    {"*", []() { return static_cast<Layout>(CodePointLayout::Builder(UCodePointMultiplicationSign)); }},
  };

  constexpr static BeautificationRule k_derivativeFractionRule = {""/* Name does not matter here */, []() { return static_cast<Layout>(FirstOrderDerivativeLayout::Builder(EmptyLayout::Builder(),CodePointLayout::Builder('x'),EmptyLayout::Builder())); }};

  /* WARNING: The two following arrays will be beautified only if
   * it can be parsed without being beautified.
   * If you add any new identifiers to this list, they must be parsable.
   * This is because we must be able to distinguish "asqrt" = "a*sqrt" from
   * "asqlt" != "a*sqlt".
   * */

  /* Sorted in alphabetical order like in parsing/helper.h
   * "If the function has multiple aliases, take the first alias
   * in alphabetical order to choose position in list." */
  constexpr static const BeautificationRule convertWhenFollowedByANonIdentifierChar[] = {
    // Greek letters
    {AliasesLists::k_piAliases, []() { return static_cast<Layout>(CodePointLayout::Builder(UCodePointGreekSmallLetterPi)); }},
    {AliasesLists::k_thetaAliases, []() { return static_cast<Layout>(CodePointLayout::Builder(UCodePointGreekSmallLetterTheta)); }},
  };

  /* Sorted in alphabetical order like in parsing/helper.h
   * "If the function has multiple aliases, take the first alias
   * in alphabetical order to choose position in list." */
  constexpr static const BeautificationRule convertWhenFollowedByParentheses[] = {
    // Functions
    {AbsoluteValue::s_functionHelper.aliasesList(), []() { return static_cast<Layout>(AbsoluteValueLayout::Builder(EmptyLayout::Builder())); }},
    {BinomialCoefficient::s_functionHelper.aliasesList(), []() { return static_cast<Layout>(BinomialCoefficientLayout::Builder(EmptyLayout::Builder(), EmptyLayout::Builder())); }},
    {Conjugate::s_functionHelper.aliasesList(), []() { return static_cast<Layout>(ConjugateLayout::Builder(EmptyLayout::Builder())); }},
    {Ceiling::s_functionHelper.aliasesList(), []() { return static_cast<Layout>(CeilingLayout::Builder(EmptyLayout::Builder())); }},
    {Derivative::s_functionHelper.aliasesList(), []() { return static_cast<Layout>(FirstOrderDerivativeLayout::Builder(EmptyLayout::Builder(),CodePointLayout::Builder('x'),EmptyLayout::Builder())); }},
    {Power::s_exponentialFunctionHelper.aliasesList(), []() { return static_cast<Layout>(HorizontalLayout::Builder(CodePointLayout::Builder('e'), VerticalOffsetLayout::Builder(EmptyLayout::Builder(), VerticalOffsetLayoutNode::Position::Superscript))); }},
    {Floor::s_functionHelper.aliasesList(), []() { return static_cast<Layout>(FloorLayout::Builder(EmptyLayout::Builder())); }},
    {Integral::s_functionHelper.aliasesList(), []() { return static_cast<Layout>(IntegralLayout::Builder(EmptyLayout::Builder(),CodePointLayout::Builder('x'),EmptyLayout::Builder(),EmptyLayout::Builder())); }},
    {VectorNorm::s_functionHelper.aliasesList(), []() { return static_cast<Layout>(VectorNormLayout::Builder(EmptyLayout::Builder())); }},
    {NthRoot::s_functionHelper.aliasesList(), []() { return static_cast<Layout>(NthRootLayout::Builder(EmptyLayout::Builder(), EmptyLayout::Builder())); }},
    {SquareRoot::s_functionHelper.aliasesList(), []() { return static_cast<Layout>(NthRootLayout::Builder(EmptyLayout::Builder())); }},
  };
};

}

#endif