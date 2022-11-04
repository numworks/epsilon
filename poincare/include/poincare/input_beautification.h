#ifndef POINCARE_INPUT_BEAUTIFICATION_H
#define POINCARE_INPUT_BEAUTIFICATION_H

#include <poincare/layout.h>
#include <poincare/binomial_coefficient.h>
#include <poincare/conjugate.h>
#include <poincare/derivative.h>
#include <poincare/integral.h>
#include <poincare/layout_helper.h>
#include <poincare/logarithm.h>
#include <poincare/piecewise_operator.h>
#include <poincare/power.h>
#include <poincare/nth_root.h>
#include <poincare/square_root.h>
#include <poincare_layouts.h>

namespace Poincare {

class InputBeautification {
public:
  static void ApplyBeautificationBetweenIndexes(HorizontalLayout parent, int firstIndex, int lastIndex, LayoutCursor * layoutCursor, Context * context, bool forceCursorRightOfText = false, bool forceBeautification = false);

  /* Will apply "convertWhenInputted" rules on lastAddedLayout
   * Will apply "convertWhenFollowedByANonIdentifierChar" and
   * "convertWhenFollowedByParentheses" to layouts left of lastAddedLayout,
   * except if forceBeautification = true. In this case, these are also
   * applied to lastAddedLayout.
   *
   * Returns the index of the leftmost beautified layout in parent
   * returns -1 if the parent was altered. */
  static int ApplyBeautificationLeftOfLastAddedLayout(Layout lastAddedLayout, LayoutCursor * layoutCursor, Context * context, bool forceCursorRightOfText = false, bool forceBeautification = false, bool preventAlteringParent = false);

private:
  typedef Layout (*BeautifiedLayoutBuilder) (Layout builderParameter);
  struct BeautificationRule {
    AliasesList listOfBeautifiedAliases;
    BeautifiedLayoutBuilder layoutBuilder;
  };

  /* TODO : Beautification input is applied within HorizontalLayouts only.
   * This excludes beautification of single char inputs that have not yet been
   * placed within a HorizontalLayouts (such as |*_|, _ being the cursor). This
   * means that BeautificationRule on 1 char aliases isn't always ensured.
   * Currently, "*" is the only beautification affected. */
  constexpr static const BeautificationRule convertWhenInputted[] = {
    // Comparison operators
    {"<=", [](Layout builderParameter) { return static_cast<Layout>(CodePointLayout::Builder(UCodePointInferiorEqual)); }},
    {">=", [](Layout builderParameter) { return static_cast<Layout>(CodePointLayout::Builder(UCodePointSuperiorEqual)); }},
    // Special char
    {"->", [](Layout builderParameter) { return static_cast<Layout>(CodePointLayout::Builder(UCodePointRightwardsArrow)); }},
    {"*", [](Layout builderParameter) { return static_cast<Layout>(CodePointLayout::Builder(UCodePointMultiplicationSign)); }},
  };

  /* WARNING: The following arrays ("convertWhenFollowedByANonIdentifierChar"
   * and "convertWhenFollowedByParentheses") will be beautified only if
   * the expression can be parsed without being beautified.
   * If you add any new identifiers to this list, they must be parsable.
   * This is because we must be able to distinguish "asqrt" = "a*sqrt" from
   * "asqlt" != "a*sqlt".
   * */

  /* Sorted in alphabetical order like in parsing/helper.h
   * "If the function has multiple aliases, take the first alias
   * in alphabetical order to choose position in list." */
  constexpr static const BeautificationRule convertWhenFollowedByANonIdentifierChar[] = {
    // inf
    {"inf", [](Layout builderParameter) { return static_cast<Layout>(CodePointLayout::Builder(UCodePointInfinity)); }},
    // Greek letters
    {"pi", [](Layout builderParameter) { return static_cast<Layout>(CodePointLayout::Builder(UCodePointGreekSmallLetterPi)); }},
    {"theta", [](Layout builderParameter) { return static_cast<Layout>(CodePointLayout::Builder(UCodePointGreekSmallLetterTheta)); }},
  };

  constexpr static BeautificationRule k_absoluteValueRule = {
    AbsoluteValue::s_functionHelper.aliasesList(),
    [](Layout builderParameter) {
      return static_cast<Layout>(AbsoluteValueLayout::Builder(builderParameter.isUninitialized() ? EmptyLayout::Builder() : builderParameter));
    }
  };

  constexpr static BeautificationRule k_derivativeRule = {
    Derivative::s_functionHelper.aliasesList(),
    [](Layout builderParameter) {
      return static_cast<Layout>(FirstOrderDerivativeLayout::Builder(EmptyLayout::Builder(),CodePointLayout::Builder('x'),EmptyLayout::Builder()));
    }
  };

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
      /* abs( */ k_absoluteValueRule,
    { /* binomial( */
      BinomialCoefficient::s_functionHelper.aliasesList(),
      [](Layout builderParameter) {
        return static_cast<Layout> (BinomialCoefficientLayout::Builder(EmptyLayout::Builder(), EmptyLayout::Builder()));
      }
    },
    { /* ceil( */
      Ceiling::s_functionHelper.aliasesList(),
      [](Layout builderParameter) {
        return static_cast<Layout>(CeilingLayout::Builder(EmptyLayout::Builder()));
      }
    },
    { /* conj( */
      Conjugate::s_functionHelper.aliasesList(),
      [](Layout builderParameter) {
        return static_cast<Layout>(ConjugateLayout::Builder(EmptyLayout::Builder()));
      }
    },
      /* diff( */ k_derivativeRule,
    { /* exp( */
      Power::s_exponentialFunctionHelper.aliasesList(),
      [](Layout builderParameter) {
        return static_cast<Layout>(HorizontalLayout::Builder(CodePointLayout::Builder('e'), VerticalOffsetLayout::Builder(EmptyLayout::Builder(), VerticalOffsetLayoutNode::VerticalPosition::Superscript)));
      }
    },
    { /* floor( */
      Floor::s_functionHelper.aliasesList(),
      [](Layout builderParameter) {
        return static_cast<Layout>(FloorLayout::Builder(EmptyLayout::Builder()));
      }
    },
    { /* int( */
      Integral::s_functionHelper.aliasesList(),
      [](Layout builderParameter) {
        return static_cast<Layout>(IntegralLayout::Builder(EmptyLayout::Builder(),CodePointLayout::Builder('x'),EmptyLayout::Builder(),EmptyLayout::Builder()));
      }
    },
    { /* norm( */
      VectorNorm::s_functionHelper.aliasesList(),
      [](Layout builderParameter) {
        return static_cast<Layout>(VectorNormLayout::Builder(EmptyLayout::Builder()));
      }
    },
    {/* piecewise( */
      PiecewiseOperator::s_functionHelper.aliasesList(),
      [](Layout builderParameter) {
        /* WARNING: The implementation of ReplaceEmptyLayoutsWithParameters
         * needs the created layout to have empty layouts where the parameters
         * should be inserted. Since Piecewise operator does not have a fixed
         * number of children, the implementation is not perfect.
         * Indeed, if the layout_field is currently filled with "4, x>0, 5",
         * and "piecewise(" is inserted left of it, "piecewise(4, x>0, 5)"
         * won't be beautified, since the piecewise layout does not have
         * 3 empty children.
         * This is a fringe case though, and everything works fine when
         * "piecewise(" is inserted with nothing on its right. */
        PiecewiseOperatorLayout layout = PiecewiseOperatorLayout::Builder();
        layout.addRow(EmptyLayout::Builder());
        return static_cast<Layout>(layout);
      }
    },
    { /* root( */
      NthRoot::s_functionHelper.aliasesList(),
      [](Layout builderParameter) {
        return static_cast<Layout>(NthRootLayout::Builder(EmptyLayout::Builder(), EmptyLayout::Builder()));
      }
    },
    { /* sqrt( */
      SquareRoot::s_functionHelper.aliasesList(),
      [](Layout builderParameter) {
        return static_cast<Layout>(NthRootLayout::Builder(EmptyLayout::Builder()));
      }
    },
  };

  static bool ShouldBeBeautifiedWhenInputted(Layout parent, int indexOfLastAddedLayout, BeautificationRule beautificationRule);

  static int BeautifyPipeKey(Layout parent, int indexOfPipeKey, LayoutCursor * cursor, bool forceCursorRightOfText);

  static bool BeautifyFractionIntoDerivativeIfPossible(Layout parent, int indexOfLastAddedLayout, LayoutCursor * layoutCursor, bool forceCursorRightOfText);
  static bool BeautifyFirstOrderDerivativeIntoNthOrderDerivativeIfPossible(Layout parent, int indexOfLastAddedLayout, LayoutCursor * layoutCursor, bool forceCursorRightOfText);

  // Returns result of comparison
  static int CompareAndBeautifyIdentifier(const char * identifier, size_t identifierLength, BeautificationRule beautificationRule, Layout parent, int startIndex, int * numberOfLayoutsAddedOrRemoved, LayoutCursor * layoutCursor, bool isBeautifyingFunction, bool forceCursorRightOfText);

  // Returns the number of layouts added or removed
  static int RemoveLayoutsBetweenIndexAndReplaceWithPattern(Layout parent, int startIndex, int endIndex, BeautificationRule beautificationRule, LayoutCursor * layoutCursor, bool isBeautifyingFunction, bool forceCursorRightOfText, Layout builderParameter = Layout());

  static Layout ReplaceEmptyLayoutsWithParameters(Layout layoutToModify, Layout parent, int parenthesisIndexInParent, bool isParameteredExpression);
};

}

#endif
