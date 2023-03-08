#ifndef POINCARE_INPUT_BEAUTIFICATION_H
#define POINCARE_INPUT_BEAUTIFICATION_H

#include <poincare/binomial_coefficient.h>
#include <poincare/comparison.h>
#include <poincare/conjugate.h>
#include <poincare/derivative.h>
#include <poincare/integral.h>
#include <poincare/layout.h>
#include <poincare/layout_cursor.h>
#include <poincare/layout_helper.h>
#include <poincare/logarithm.h>
#include <poincare/nth_root.h>
#include <poincare/piecewise_operator.h>
#include <poincare/power.h>
#include <poincare/square_root.h>
#include <poincare_layouts.h>

namespace Poincare {

class InputBeautification {
 public:
  typedef struct {
    bool beautifyIdentifiersBeforeInserting;
    bool beautifyAfterInserting;
  } InsertionBeautificationMethod;

  static InsertionBeautificationMethod BeautificationMethodWhenInsertingLayout(
      Layout insertedLayout);

  /* Both of the following functions return true if layouts were beautified.
   *
   * BeautifyIdentifiersLeftOfCursor will only apply the k_identifiersRules.
   * This is called either:
   *    - When moving out of an horizontal layout.
   *    - Just before inserting a non-identifier char.
   *
   * BeautifyLeftOfCursorAfterInsertion will apply:
   *    - k_onInsertionRules (all combinations of chars that are beautified
   *      immediatly when inserted).
   *    - k_logarithmRule and k_functionsRules (if a left parenthesis was
   *      just inserted).
   *    - BeautifyPipeKey, BeautifyFractionIntoDerivative and
   *      BeautifyFirstOrderDerivativeIntoNthOrder.
   * This is called only after an insertion, if the relevant char was inserted.
   */
  static bool BeautifyIdentifiersLeftOfCursor(LayoutCursor* layoutCursor,
                                              Context* context);
  static bool BeautifyLeftOfCursorAfterInsertion(LayoutCursor* layoutCursor,
                                                 Context* context);

 private:
  typedef Layout (*BeautifiedLayoutBuilder)(Layout builderParameter);
  struct BeautificationRule {
    AliasesList listOfBeautifiedAliases;
    BeautifiedLayoutBuilder layoutBuilder;
  };

  /* TODO : Beautification input is applied within HorizontalLayouts only.
   * This excludes beautification of single char inputs that have not yet been
   * placed within a HorizontalLayouts (such as |*_|, _ being the cursor). This
   * means that BeautificationRule on 1 char aliases isn't always ensured.
   * Currently, "*" is the only beautification affected. */
  constexpr static const BeautificationRule k_symbolsRules[] = {
      // Comparison operators
      {"<=",
       [](Layout builderParameter) {
         return static_cast<Layout>(ComparisonNode::ComparisonOperatorLayout(
             ComparisonNode::OperatorType::InferiorEqual));
       }},
      {">=",
       [](Layout builderParameter) {
         return static_cast<Layout>(ComparisonNode::ComparisonOperatorLayout(
             ComparisonNode::OperatorType::SuperiorEqual));
       }},
      {"!=",
       [](Layout builderParameter) {
         return static_cast<Layout>(ComparisonNode::ComparisonOperatorLayout(
             ComparisonNode::OperatorType::NotEqual));
       }},
      // Special char
      {"->",
       [](Layout builderParameter) {
         return static_cast<Layout>(
             CodePointLayout::Builder(UCodePointRightwardsArrow));
       }},
      {"*",
       [](Layout builderParameter) {
         return static_cast<Layout>(
             CodePointLayout::Builder(UCodePointMultiplicationSign));
       }},
  };

  /* WARNING: The following arrays (convertIdentifiers and convertFunctions)
   * will be beautified only if the expression can be parsed without being
   * beautified.
   * If you add any new identifiers to this list, they must be parsable.
   * This is because we must be able to distinguish "asqrt" = "a*sqrt" from
   * "asqlt" != "a*sqlt".
   * */

  /* Sorted in alphabetical order like in parsing/helper.h
   * "If the function has multiple aliases, take the first alias
   * in alphabetical order to choose position in list." */
  constexpr static const BeautificationRule k_identifiersRules[] = {
      // inf
      {"inf",
       [](Layout builderParameter) {
         return static_cast<Layout>(
             CodePointLayout::Builder(UCodePointInfinity));
       }},
      // Greek letters
      {"pi",
       [](Layout builderParameter) {
         return static_cast<Layout>(
             CodePointLayout::Builder(UCodePointGreekSmallLetterPi));
       }},
      {"theta",
       [](Layout builderParameter) {
         return static_cast<Layout>(
             CodePointLayout::Builder(UCodePointGreekSmallLetterTheta));
       }},
  };

  constexpr static BeautificationRule k_absoluteValueRule = {
      AbsoluteValue::s_functionHelper.aliasesList(),
      [](Layout builderParameter) {
        return static_cast<Layout>(AbsoluteValueLayout::Builder(
            builderParameter.isUninitialized() ? HorizontalLayout::Builder()
                                               : builderParameter));
      }};

  constexpr static BeautificationRule k_derivativeRule = {
      Derivative::s_functionHelper.aliasesList(), [](Layout builderParameter) {
        return static_cast<Layout>(FirstOrderDerivativeLayout::Builder(
            HorizontalLayout::Builder(), CodePointLayout::Builder('x'),
            HorizontalLayout::Builder()));
      }};

  constexpr static BeautificationRule k_logarithmRule = {
      Logarithm::s_functionHelper.aliasesList(), [](Layout builderParameter) {
        return static_cast<Layout>(
            LayoutHelper::Logarithm(HorizontalLayout::Builder(),
                                    builderParameter.isUninitialized()
                                        ? HorizontalLayout::Builder()
                                        : builderParameter)
                .makeEditable());
      }};

  /* Sorted in alphabetical order like in parsing/helper.h
   * "If the function has multiple aliases, take the first alias
   * in alphabetical order to choose position in list." */
  constexpr static const BeautificationRule k_functionsRules[] = {
      // Functions
      /* abs( */ k_absoluteValueRule,
      {/* binomial( */
       BinomialCoefficient::s_functionHelper.aliasesList(),
       [](Layout builderParameter) {
         return static_cast<Layout>(BinomialCoefficientLayout::Builder(
             HorizontalLayout::Builder(), HorizontalLayout::Builder()));
       }},
      {/* ceil( */
       Ceiling::s_functionHelper.aliasesList(),
       [](Layout builderParameter) {
         return static_cast<Layout>(
             CeilingLayout::Builder(HorizontalLayout::Builder()));
       }},
      {/* conj( */
       Conjugate::s_functionHelper.aliasesList(),
       [](Layout builderParameter) {
         return static_cast<Layout>(
             ConjugateLayout::Builder(HorizontalLayout::Builder()));
       }},
      /* diff( */ k_derivativeRule,
      {/* exp( */
       Power::s_exponentialFunctionHelper.aliasesList(),
       [](Layout builderParameter) {
         return static_cast<Layout>(HorizontalLayout::Builder(
             CodePointLayout::Builder('e'),
             VerticalOffsetLayout::Builder(
                 HorizontalLayout::Builder(),
                 VerticalOffsetLayoutNode::VerticalPosition::Superscript)));
       }},
      {/* floor( */
       Floor::s_functionHelper.aliasesList(),
       [](Layout builderParameter) {
         return static_cast<Layout>(
             FloorLayout::Builder(HorizontalLayout::Builder()));
       }},
      {/* int( */
       Integral::s_functionHelper.aliasesList(),
       [](Layout builderParameter) {
         return static_cast<Layout>(IntegralLayout::Builder(
             HorizontalLayout::Builder(), CodePointLayout::Builder('x'),
             HorizontalLayout::Builder(), HorizontalLayout::Builder()));
       }},
      {/* norm( */
       VectorNorm::s_functionHelper.aliasesList(),
       [](Layout builderParameter) {
         return static_cast<Layout>(
             VectorNormLayout::Builder(HorizontalLayout::Builder()));
       }},
      {/* piecewise( */
       PiecewiseOperator::s_functionHelper.aliasesList(),
       [](Layout builderParameter) {
         /* WARNING: The implementation of ReplaceEmptyLayoutsWithParameters
          * needs the created layout to have empty layouts where the
          * parameters should be inserted. Since Piecewise operator does not
          * have a fixed number of children, the implementation is not
          * perfect. Indeed, if the layout_field is currently filled with
          * "4, x>0, 5", and "piecewise(" is inserted left of it,
          * "piecewise(4, x>0, 5)" won't be beautified, since the piecewise
          * layout does not have 3 empty children. This is a fringe case
          * though, and everything works fine when "piecewise(" is inserted
          * with nothing on its right. */
         PiecewiseOperatorLayout layout = PiecewiseOperatorLayout::Builder();
         layout.addRow(HorizontalLayout::Builder());
         return static_cast<Layout>(layout);
       }},
      {/* root( */
       NthRoot::s_functionHelper.aliasesList(),
       [](Layout builderParameter) {
         return static_cast<Layout>(NthRootLayout::Builder(
             HorizontalLayout::Builder(), HorizontalLayout::Builder()));
       }},
      {/* sqrt( */
       SquareRoot::s_functionHelper.aliasesList(),
       [](Layout builderParameter) {
         return static_cast<Layout>(
             NthRootLayout::Builder(HorizontalLayout::Builder()));
       }},
  };

  static bool LayoutIsIdentifierMaterial(Layout l);

  // All following mathos return true if layout was beautified

  /* Apply k_symbolsRules  */
  static bool BeautifySymbols(HorizontalLayout h, int rightMostIndexToBeautify,
                              LayoutCursor* layoutCursor);

  /* Apply k_identifiersRules.
   * If afterLeftParenthesisInsertion is true, also apply k_functionsRules and
   * k_logarithmRule. */
  static bool BeautifyIdentifiers(HorizontalLayout h,
                                  int rightMostIndexToBeautify,
                                  Context* context, LayoutCursor* layoutCursor,
                                  bool afterLeftParenthesisInsertion);

  static bool BeautifyPipeKey(HorizontalLayout h, int indexOfPipeKey,
                              LayoutCursor* cursor);

  static bool BeautifyFractionIntoDerivative(HorizontalLayout h,
                                             int indexOfFraction,
                                             LayoutCursor* layoutCursor);
  static bool BeautifyFirstOrderDerivativeIntoNthOrder(
      HorizontalLayout h, int indexOfSuperscript, LayoutCursor* layoutCursor);

  static bool CompareAndBeautifyIdentifier(
      const char* identifier, size_t identifierLength,
      BeautificationRule beautificationRule, HorizontalLayout h, int startIndex,
      LayoutCursor* layoutCursor, bool isBeautifyingFunction,
      int* comparisonResult, int* numberOfLayoutsAddedOrRemoved);

  static bool RemoveLayoutsBetweenIndexAndReplaceWithPattern(
      HorizontalLayout h, int startIndex, int endIndex,
      BeautificationRule beautificationRule, LayoutCursor* layoutCursor,
      bool isBeautifyingFunction, int* numberOfLayoutsAddedOrRemoved = nullptr,
      Layout builderParameter = Layout());

  static Layout ReplaceEmptyLayoutsWithParameters(Layout layoutToModify,
                                                  HorizontalLayout h,
                                                  int parenthesisIndexInParent,
                                                  LayoutCursor* layoutCursor,
                                                  bool isParameteredExpression);
};

}  // namespace Poincare

#endif
