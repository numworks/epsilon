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
#include <poincare/product.h>
#include <poincare/square_root.h>
#include <poincare/sum.h>
#include <poincare_layouts.h>

#include <array>

namespace Poincare {

class InputBeautification {
 public:
  struct BeautificationMethod {
    bool beautifyIdentifiersBeforeInserting;
    bool beautifyAfterInserting;
  };

  static BeautificationMethod BeautificationMethodWhenInsertingLayout(
      Layout insertedLayout);

  /* Both of the following functions return true if layouts were beautified.
   *
   * BeautifyLeftOfCursorBeforeCursorMove will only apply the
   * k_simpleIdentifiersRules. This is called either:
   *    - When moving out of an horizontal layout.
   *    - Just before inserting a non-identifier char.
   *
   * BeautifyLeftOfCursorAfterInsertion will apply:
   *    - k_onInsertionRules (all combinations of chars that are beautified
   *      immediatly when inserted).
   *    - k_logarithmRule and k_identifiersRules (if a left parenthesis was
   *      just inserted).
   *    - BeautifyPipeKey, BeautifyFractionIntoDerivative and
   *      BeautifyFirstOrderDerivativeIntoNthOrder.
   * This is called only after an insertion, if the relevant char was inserted.
   */
  static bool BeautifyLeftOfCursorBeforeCursorMove(LayoutCursor* layoutCursor,
                                                   Context* context);
  static bool BeautifyLeftOfCursorAfterInsertion(LayoutCursor* layoutCursor,
                                                 Context* context);

 private:
  using BeautifiedLayoutBuilder = Layout (*)(Layout* parameters);
  constexpr static int k_maxNumberOfParameters = 4;
  struct BeautificationRule {
    AliasesList listOfBeautifiedAliases;
    int numberOfParameters;
    BeautifiedLayoutBuilder layoutBuilder;
  };

  /* TODO : Beautification input is applied within HorizontalLayouts only.
   * This excludes beautification of single char inputs that have not yet been
   * placed within a HorizontalLayouts (such as |*_|, _ being the cursor). This
   * means that BeautificationRule on 1 char aliases isn't always ensured.
   * Currently, "*" is the only beautification affected. */
  constexpr static const BeautificationRule k_symbolsRules[] = {
      // Comparison operators
      {"<=", 0,
       [](Layout* parameters) {
         return static_cast<Layout>(ComparisonNode::ComparisonOperatorLayout(
             ComparisonNode::OperatorType::InferiorEqual));
       }},
      {">=", 0,
       [](Layout* parameters) {
         return static_cast<Layout>(ComparisonNode::ComparisonOperatorLayout(
             ComparisonNode::OperatorType::SuperiorEqual));
       }},
      {"!=", 0,
       [](Layout* parameters) {
         return static_cast<Layout>(ComparisonNode::ComparisonOperatorLayout(
             ComparisonNode::OperatorType::NotEqual));
       }},
      // Special char
      {"->", 0,
       [](Layout* parameters) {
         return static_cast<Layout>(
             CodePointLayout::Builder(UCodePointRightwardsArrow));
       }},
      {"*", 0,
       [](Layout* parameters) {
         return static_cast<Layout>(
             CodePointLayout::Builder(UCodePointMultiplicationSign));
       }},
  };

  constexpr static BeautificationRule k_infRule = {
      "inf", 0, [](Layout* parameters) {
        return static_cast<Layout>(
            CodePointLayout::Builder(UCodePointInfinity));
      }};

  constexpr static BeautificationRule k_piRule = {
      "pi", 0, [](Layout* parameters) {
        return static_cast<Layout>(
            CodePointLayout::Builder(UCodePointGreekSmallLetterPi));
      }};
  constexpr static BeautificationRule k_thetaRule = {
      "theta", 0, [](Layout* parameters) {
        return static_cast<Layout>(
            CodePointLayout::Builder(UCodePointGreekSmallLetterTheta));
      }};

  constexpr static BeautificationRule k_absoluteValueRule = {
      AbsoluteValue::s_functionHelper.aliasesList(), 1, [](Layout* parameters) {
        return static_cast<Layout>(AbsoluteValueLayout::Builder(parameters[0]));
      }};

  constexpr static BeautificationRule k_derivativeRule = {
      Derivative::s_functionHelper.aliasesList(), 3, [](Layout* parameters) {
        if (parameters[1].isEmpty()) {  // This preserves cursor
          parameters[1].addChildAtIndexInPlace(CodePointLayout::Builder('x'), 0,
                                               0);
        }
        return static_cast<Layout>(FirstOrderDerivativeLayout::Builder(
            parameters[0], parameters[1], parameters[2]));
      }};

  /* WARNING 1: The following arrays (k_simpleIdentifiersRules and
   * k_identifiersRules) will be beautified only if the expression can be parsed
   * without being beautified. If you add any new identifiers to one of these
   * lists, they must be parsable. This is because we must be able to
   * distinguish "asqrt" = "a*sqrt" from "asqlt" != "a*sqlt".
   *
   * WARNING 2: These need to be sorted in alphabetical order like in
   * parsing/helper.h:
   * "If the function has multiple aliases, take the first alias
   * in alphabetical order to choose position in list." */
  constexpr static const BeautificationRule k_simpleIdentifiersRules[] = {
      k_infRule, k_piRule, k_thetaRule};
  constexpr static size_t k_lenOfSimpleIdentifiersRules =
      std::size(k_simpleIdentifiersRules);

  // simpleIdentifiersRules are included in identifiersRules
  constexpr static const BeautificationRule k_identifiersRules[] = {
      /* abs( */ k_absoluteValueRule,
      {/* binomial( */
       BinomialCoefficient::s_functionHelper.aliasesList(), 2,
       [](Layout* parameters) {
         return static_cast<Layout>(
             BinomialCoefficientLayout::Builder(parameters[0], parameters[1]));
       }},
      {/* ceil( */
       Ceiling::s_functionHelper.aliasesList(), 1,
       [](Layout* parameters) {
         return static_cast<Layout>(CeilingLayout::Builder(parameters[0]));
       }},
      {/* conj( */
       Conjugate::s_functionHelper.aliasesList(), 1,
       [](Layout* parameters) {
         return static_cast<Layout>(ConjugateLayout::Builder(parameters[0]));
       }},
      /* diff( */ k_derivativeRule,
      {/* exp( */
       Power::s_exponentialFunctionHelper.aliasesList(), 1,
       [](Layout* parameters) {
         return static_cast<Layout>(HorizontalLayout::Builder(
             CodePointLayout::Builder('e'),
             VerticalOffsetLayout::Builder(
                 parameters[0],
                 VerticalOffsetLayoutNode::VerticalPosition::Superscript)));
       }},
      {/* floor( */
       Floor::s_functionHelper.aliasesList(), 1,
       [](Layout* parameters) {
         return static_cast<Layout>(FloorLayout::Builder(parameters[0]));
       }},
      /* inf */ k_infRule,
      {/* int( */
       Integral::s_functionHelper.aliasesList(), 4,
       [](Layout* parameters) {
         if (parameters[1].isEmpty()) {  // This preserves cursor
           parameters[1].addChildAtIndexInPlace(CodePointLayout::Builder('x'),
                                                0, 0);
         }
         return static_cast<Layout>(IntegralLayout::Builder(
             parameters[0], parameters[1], parameters[2], parameters[3]));
       }},
      {/* norm( */
       VectorNorm::s_functionHelper.aliasesList(), 1,
       [](Layout* parameters) {
         return static_cast<Layout>(VectorNormLayout::Builder(parameters[0]));
       }},
      /* pi */ k_piRule,
      {/* piecewise( */
       PiecewiseOperator::s_functionHelper.aliasesList(), 2,
       [](Layout* parameters) {
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
         layout.addRow(parameters[0],
                       parameters[1].isEmpty() ? Layout() : parameters[1]);
         return static_cast<Layout>(layout);
       }},
      {/* product( */
       Product::s_functionHelper.aliasesList(), 4,
       [](Layout* parameters) {
         if (parameters[1].isEmpty()) {  // This preserves cursor
           parameters[1].addChildAtIndexInPlace(CodePointLayout::Builder('k'),
                                                0, 0);
         }
         return static_cast<Layout>(ProductLayout::Builder(
             parameters[0], parameters[1], parameters[2], parameters[3]));
       }},
      {/* root( */
       NthRoot::s_functionHelper.aliasesList(), 2,
       [](Layout* parameters) {
         return static_cast<Layout>(
             NthRootLayout::Builder(parameters[0], parameters[1]));
       }},
      {/* sqrt( */
       SquareRoot::s_functionHelper.aliasesList(), 1,
       [](Layout* parameters) {
         return static_cast<Layout>(NthRootLayout::Builder(parameters[0]));
       }},
      /* theta */ k_thetaRule};

  constexpr static size_t k_lenOfIdentifiersRules =
      std::size(k_identifiersRules);

  constexpr static BeautificationRule k_sumRule = {
      Sum::s_functionHelper.aliasesList(), 4, [](Layout* parameters) {
        if (parameters[1].isEmpty()) {  // This preserves cursor
          parameters[1].addChildAtIndexInPlace(CodePointLayout::Builder('k'), 0,
                                               0);
        }
        return static_cast<Layout>(SumLayout::Builder(
            parameters[0], parameters[1], parameters[2], parameters[3]));
      }};

  constexpr static BeautificationRule k_logarithmRule = {
      Logarithm::s_functionHelper.aliasesList(), 2, [](Layout* parameters) {
        return static_cast<Layout>(
            LayoutHelper::Logarithm(parameters[0], parameters[1])
                .makeEditable());
      }};
  constexpr static int k_indexOfBaseOfLog = 1;

  static bool LayoutIsIdentifierMaterial(Layout l);

  // All following methods return true if layout was beautified

  /* Apply k_symbolsRules  */
  static bool BeautifySymbols(HorizontalLayout h, int rightmostIndexToBeautify,
                              LayoutCursor* layoutCursor);

  /* Apply the rules passed in rulesList as long as they match a tokenizable
   * identifiers. */
  static bool TokenizeAndBeautifyIdentifiers(
      HorizontalLayout h, int rightmostIndexToBeautify,
      const BeautificationRule* rulesList, size_t numberOfRules,
      Context* context, LayoutCursor* layoutCursor,
      bool logBeautification = false);

  static bool BeautifyPipeKey(HorizontalLayout h, int indexOfPipeKey,
                              LayoutCursor* cursor);

  static bool BeautifyFractionIntoDerivative(HorizontalLayout h,
                                             int indexOfFraction,
                                             LayoutCursor* layoutCursor);
  static bool BeautifyFirstOrderDerivativeIntoNthOrder(
      HorizontalLayout h, int indexOfSuperscript, LayoutCursor* layoutCursor);

  static bool BeautifySum(HorizontalLayout h, int indexOfComma,
                          Context* context, LayoutCursor* layoutCursor);

  static bool CompareAndBeautifyIdentifier(
      const char* identifier, size_t identifierLength,
      BeautificationRule beautificationRule, HorizontalLayout h, int startIndex,
      LayoutCursor* layoutCursor, int* comparisonResult,
      int* numberOfLayoutsAddedOrRemoved);

  static bool RemoveLayoutsBetweenIndexAndReplaceWithPattern(
      HorizontalLayout h, int startIndex, int endIndex,
      BeautificationRule beautificationRule, LayoutCursor* layoutCursor,
      int* numberOfLayoutsAddedOrRemoved = nullptr,
      Layout preProcessedParameter = Layout(),
      int indexOfPreProcessedParameter = -1);

  // Return false if there are too many parameters
  static bool CreateParametersList(Layout* parameters, HorizontalLayout h,
                                   int parenthesisIndexInParent,
                                   BeautificationRule beautificationRule,
                                   LayoutCursor* layoutCursor);
};

}  // namespace Poincare

#endif
