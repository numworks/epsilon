#ifndef POINCARE_INPUT_BEAUTIFICATION_H
#define POINCARE_INPUT_BEAUTIFICATION_H

#include <poincare/layout.h>
#include <poincare/binomial_coefficient.h>
#include <poincare/conjugate.h>
#include <poincare/derivative.h>
#include <poincare/integral.h>
#include <poincare/layout_helper.h>
#include <poincare/logarithm.h>
#include <poincare/power.h>
#include <poincare/nth_root.h>
#include <poincare/square_root.h>
#include <poincare_layouts.h>

namespace Poincare {

class InputBeautification {
public:
  static void ApplyBeautificationBetweenIndexes(Layout parent, int firstIndex, int lastIndex, LayoutCursor * layoutCursor, Context * context, bool forceCursorRightOfText = false, bool forceBeautification = false);

  /* Will apply "convertWhenInputted" rules on lastAddedLayout
   * Will apply "convertWhenFollowedByANonIdentifierChar" and
   * "convertWhenFollowedByParentheses" to layouts left of lastAddedLayout,
   * except if forceBeautification = true. In this case, these are also
   * applied to lastAddedLayout.
   *
   * Returns the index of the leftest beautified layout in parent
   * returns -1 if the parent was altered. */
  static int ApplyBeautificationLeftOfLastAddedLayout(Layout lastAddedLayout, LayoutCursor * layoutCursor, Context * context, bool forceCursorRightOfText = false, bool forceBeautification = false);

private:
  typedef Layout (*BeautifiedLayoutBuilder) (Layout builderParameter);
  struct BeautificationRule {
    AliasesList listOfBeautifiedAliases;
    BeautifiedLayoutBuilder layoutBuilder;
  };

  constexpr static const BeautificationRule convertWhenInputted[] = {
    // Angle units
    {"\"\"", [](Layout builderParameter) { return static_cast<Layout>(CodePointLayout::Builder('\'')); }},
    {"'\"", [](Layout builderParameter) { return static_cast<Layout>(CodePointLayout::Builder(UCodePointDegreeSign)); }},
    // Comparison operators
    {"<=", [](Layout builderParameter) { return static_cast<Layout>(CodePointLayout::Builder(UCodePointInferiorEqual)); }},
    {">=", [](Layout builderParameter) { return static_cast<Layout>(CodePointLayout::Builder(UCodePointSuperiorEqual)); }},
    // Special char
    {"->", [](Layout builderParameter) { return static_cast<Layout>(CodePointLayout::Builder(UCodePointRightwardsArrow)); }},
    {"*", [](Layout builderParameter) { return static_cast<Layout>(CodePointLayout::Builder(UCodePointMultiplicationSign)); }},
  };
  static bool ShouldBeBeautifiedWhenInputted(Layout parent, int indexOfLastAddedLayout, BeautificationRule beautificationRule);

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
    {AliasesLists::k_piAliases, [](Layout builderParameter) { return static_cast<Layout>(CodePointLayout::Builder(UCodePointGreekSmallLetterPi)); }},
    {AliasesLists::k_thetaAliases, [](Layout builderParameter) { return static_cast<Layout>(CodePointLayout::Builder(UCodePointGreekSmallLetterTheta)); }},
  };

  constexpr static BeautificationRule k_absoluteValueRule = {
    AbsoluteValue::s_functionHelper.aliasesList(),
    [](Layout builderParameter) {
      return static_cast<Layout>(AbsoluteValueLayout::Builder(builderParameter.isUninitialized() ? EmptyLayout::Builder() : builderParameter));
    }
  };
  static int BeautifyPipeKey(Layout parent, int indexOfPipeKey, LayoutCursor * cursor, bool forceCursorRightOfText);


  constexpr static BeautificationRule k_derivativeRule = {Derivative::s_functionHelper.aliasesList(), [](Layout builderParameter) { return static_cast<Layout>(FirstOrderDerivativeLayout::Builder(EmptyLayout::Builder(),CodePointLayout::Builder('x'),EmptyLayout::Builder())); }};
  static bool BeautifyFractionIntoDerivativeIfPossible(Layout parent, int indexOfLastAddedLayout, LayoutCursor * layoutCursor, bool forceCursorRightOfText);
  static bool BeautifyFirstOrderDerivativeIntoNthOrderDerivativeIfPossible(Layout parent, int indexOfLastAddedLayout, LayoutCursor * layoutCursor, bool forceCursorRightOfText);

  constexpr static BeautificationRule k_logarithmRule = {
    Logarithm::s_functionHelper.aliasesList(),
    [](Layout builderParameter) {
      return static_cast<Layout>(LayoutHelper::Logarithm(EmptyLayout::Builder(), builderParameter.isUninitialized() ? EmptyLayout::Builder() : builderParameter).makeEditable());
    }
  };

  /* Sorted in alphabetical order like in parsing/helper.h
   * "If the function has multiple aliases, take the first alias
   * in alphabetical order to choose position in list." */
  constexpr static const BeautificationRule convertWhenFollowedByParentheses[] = {
    // Functions
    k_absoluteValueRule,
    {BinomialCoefficient::s_functionHelper.aliasesList(), [](Layout builderParameter) { return static_cast<Layout>(BinomialCoefficientLayout::Builder(EmptyLayout::Builder(), EmptyLayout::Builder())); }},
    {Conjugate::s_functionHelper.aliasesList(), [](Layout builderParameter) { return static_cast<Layout>(ConjugateLayout::Builder(EmptyLayout::Builder())); }},
    {Ceiling::s_functionHelper.aliasesList(), [](Layout builderParameter) { return static_cast<Layout>(CeilingLayout::Builder(EmptyLayout::Builder())); }},
    k_derivativeRule,
    {Power::s_exponentialFunctionHelper.aliasesList(), [](Layout builderParameter) { return static_cast<Layout>(HorizontalLayout::Builder(CodePointLayout::Builder('e'), VerticalOffsetLayout::Builder(EmptyLayout::Builder(), VerticalOffsetLayoutNode::Position::Superscript))); }},
    {Floor::s_functionHelper.aliasesList(), [](Layout builderParameter) { return static_cast<Layout>(FloorLayout::Builder(EmptyLayout::Builder())); }},
    {Integral::s_functionHelper.aliasesList(), [](Layout builderParameter) { return static_cast<Layout>(IntegralLayout::Builder(EmptyLayout::Builder(),CodePointLayout::Builder('x'),EmptyLayout::Builder(),EmptyLayout::Builder())); }},
    {VectorNorm::s_functionHelper.aliasesList(), [](Layout builderParameter) { return static_cast<Layout>(VectorNormLayout::Builder(EmptyLayout::Builder())); }},
    {NthRoot::s_functionHelper.aliasesList(), [](Layout builderParameter) { return static_cast<Layout>(NthRootLayout::Builder(EmptyLayout::Builder(), EmptyLayout::Builder())); }},
    {SquareRoot::s_functionHelper.aliasesList(), [](Layout builderParameter) { return static_cast<Layout>(NthRootLayout::Builder(EmptyLayout::Builder())); }},
  };
  // Returns result of comparison
  static int CompareAndBeautifyIdentifier(const char * identifier, size_t identifierLength, BeautificationRule beautificationRule, Layout parent, int startIndex, int * numberOfLayoutsAddedOrRemoved, LayoutCursor * layoutCursor, bool isBeautifyingFunction, bool forceCursorRightOfText);

  // Returns the number of layouts added or removed
  static int RemoveLayoutsBetweenIndexAndReplaceWithPattern(Layout parent, int startIndex, int endIndex, BeautificationRule beautificationRule, LayoutCursor * layoutCursor, bool isBeautifyingFunction, bool forceCursorRightOfText, Layout builderParameter = Layout());

  static Layout ReplaceEmptyLayoutsWithParameters(Layout layoutToModify, Layout parent, int parenthesisIndexInParent, bool isParameteredExpression);
};

}

#endif