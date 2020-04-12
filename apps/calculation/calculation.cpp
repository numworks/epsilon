#include "calculation.h"
#include "../shared/poincare_helpers.h"
#include "../global_preferences.h"
#include "../exam_mode_configuration.h"
#include <poincare/exception_checkpoint.h>
#include <poincare/undefined.h>
#include <poincare/unreal.h>
#include <string.h>
#include <cmath>
#include <algorithm>

using namespace Poincare;
using namespace Shared;

namespace Calculation {

bool Calculation::operator==(const Calculation& c) {
  return strcmp(inputText(), c.inputText()) == 0
      && strcmp(approximateOutputText(NumberOfSignificantDigits::Maximal), c.approximateOutputText(NumberOfSignificantDigits::Maximal)) == 0
      && strcmp(approximateOutputText(NumberOfSignificantDigits::UserDefined), c.approximateOutputText(NumberOfSignificantDigits::UserDefined)) == 0
      /* Some calculations can make appear trigonometric functions in their
       * exact output. Their argument will be different with the angle unit
       * preferences but both input and approximate output will be the same.
       * For example, i^(sqrt(3)) = cos(sqrt(3)*pi/2)+i*sin(sqrt(3)*pi/2) if
       * angle unit is radian and i^(sqrt(3)) = cos(sqrt(3)*90+i*sin(sqrt(3)*90)
       * in degree. */
      && strcmp(exactOutputText(), c.exactOutputText()) == 0;
}

Calculation * Calculation::next() const {
  const char * result = reinterpret_cast<const char *>(this) + sizeof(Calculation);
  for (int i = 0; i < k_numberOfExpressions; i++) {
    result = result + strlen(result) + 1; // Pass inputText, exactOutputText, ApproximateOutputText x2
  }
  return reinterpret_cast<Calculation *>(const_cast<char *>(result));
}

void Calculation::tidy() {
  /* Reset height memoization (the complex format could have changed when
   * re-entering Calculation app which would impact the heights). */
  m_height = -1;
  m_expandedHeight = -1;
}

const char * Calculation::approximateOutputText(NumberOfSignificantDigits numberOfSignificantDigits) const {
  const char * exactOutput = exactOutputText();
  const char * approximateOutputTextWithMaxNumberOfDigits = exactOutput + strlen(exactOutput) + 1;
  if (numberOfSignificantDigits == NumberOfSignificantDigits::Maximal) {
    return approximateOutputTextWithMaxNumberOfDigits;
  }
  return approximateOutputTextWithMaxNumberOfDigits + strlen(approximateOutputTextWithMaxNumberOfDigits) + 1;
}

Expression Calculation::input() {
  return Expression::Parse(m_inputText, nullptr);
}

Expression Calculation::exactOutput() {
  /* Because the angle unit might have changed, we do not simplify again. We
   * thereby avoid turning cos(Pi/4) into sqrt(2)/2 and displaying
   * 'sqrt(2)/2 = 0.999906' (which is totally wrong) instead of
   * 'cos(pi/4) = 0.999906' (which is true in degree). */
  Expression exactOutput = Expression::Parse(exactOutputText(), nullptr);
  assert(!exactOutput.isUninitialized());
  return exactOutput;
}

Expression Calculation::approximateOutput(Context * context, NumberOfSignificantDigits numberOfSignificantDigits) {
  Expression exp = Expression::Parse(approximateOutputText(numberOfSignificantDigits), nullptr);
  assert(!exp.isUninitialized());
  /* Warning:
   * Since quite old versions of Epsilon, the Expression 'exp' was used to be
   * approximated again to ensure its content was in the expected form - a
   * linear combination of Decimal.
   * However, since the approximate output may contain units and that a
   * Poincare::Unit approximates to undef, thus it must not be approximated
   * anymore.
   * We have to keep two serializations of the approximation outputs:
   * - one with the maximal significant digits, to be used by 'ans' or when
   *   handling 'OK' event on the approximation output.
   * - one with the displayed number of significant digits that we parse to
   *   create the displayed layout. If we used the other serialization to
   *   create the layout, the result of the parsing could be an Integer which
   *   does not take the number of significant digits into account when creating
   *   its layout. This would lead to wrong number of significant digits in the
   *   layout.
   *   For instance:
   *        Number of asked significant digits: 7
   *        Input: "123456780", Approximate output: "1.234567E8"
   *
   *  |--------------------------------------------------------------------------------------|
   *  | Number of significant digits | Approximate text | Parse expression    | Layout       |
   *  |------------------------------+------------------+---------------------+--------------|
   *  | Maximal                      | "123456780"      | Integer(123456780)  | "123456780"  |
   *  |------------------------------+------------------+---------------------+--------------|
   *  | User defined                 | "1.234567E8"     | Decimal(1.234567E8) | "1.234567E8" |
   *  |--------------------------------------------------------------------------------------|
   *
   */
   return exp;
}

Layout Calculation::createInputLayout() {
  return input().createLayout(Preferences::PrintFloatMode::Decimal, PrintFloat::k_numberOfStoredSignificantDigits);
}

Layout Calculation::createExactOutputLayout(bool * couldNotCreateExactLayout) {
  Poincare::ExceptionCheckpoint ecp;
  if (ExceptionRun(ecp)) {
    return PoincareHelpers::CreateLayout(exactOutput());
  } else {
    *couldNotCreateExactLayout = true;
    return Layout();
  }
}

Layout Calculation::createApproximateOutputLayout(Context * context, bool * couldNotCreateApproximateLayout) {
  Poincare::ExceptionCheckpoint ecp;
  if (ExceptionRun(ecp)) {
    return PoincareHelpers::CreateLayout(approximateOutput(context, NumberOfSignificantDigits::UserDefined));
  } else {
    *couldNotCreateApproximateLayout = true;
    return Layout();
  }
}

KDCoordinate Calculation::height(Context * context, bool expanded, bool allExpressionsInline) {
  KDCoordinate result = expanded ? m_expandedHeight : m_height;
  if (result >= 0) {
    // Height already computed
    return result;
  }

  // Get input height
  Layout inputLayout = createInputLayout();
  KDCoordinate inputHeight = inputLayout.layoutSize().height();
  KDCoordinate inputBaseline = inputLayout.baseline();

  // Get exact output height if needed
  Poincare::Layout exactLayout;
  bool couldNotCreateExactLayout = false;
  if (DisplaysExact(displayOutput(context))) {
    // Create the exact output layout
    exactLayout = createExactOutputLayout(&couldNotCreateExactLayout);
    if (couldNotCreateExactLayout) {
      if (displayOutput(context) != DisplayOutput::ExactOnly) {
        forceDisplayOutput(DisplayOutput::ApproximateOnly);
      } else {
        /* We should only display the exact result, but we cannot create it
         * -> raise an exception. */
        ExceptionCheckpoint::Raise();
      }
    }
  }

  if (displayOutput(context) == DisplayOutput::ExactOnly) {
    KDCoordinate exactOutputHeight = exactLayout.layoutSize().height();
    if (allExpressionsInline) {
      KDCoordinate exactOutputBaseline = exactLayout.baseline();
      result = std::max(inputBaseline, exactOutputBaseline) + std::max(inputHeight - inputBaseline, exactOutputHeight-exactOutputBaseline);
    } else {
      result = inputHeight+exactOutputHeight;
    }
  } else {
    bool couldNotCreateApproximateLayout = false;
    Layout approximateLayout = createApproximateOutputLayout(context, &couldNotCreateApproximateLayout);
    if (couldNotCreateApproximateLayout) {
      if (displayOutput(context) == DisplayOutput::ApproximateOnly) {
        Poincare::ExceptionCheckpoint::Raise();
      } else {
        /* Set the display output to ApproximateOnly, make room in the pool by
         * erasing the exact layout, and retry to create the approximate layout */
        forceDisplayOutput(DisplayOutput::ApproximateOnly);
        exactLayout = Poincare::Layout();
        couldNotCreateApproximateLayout = false;
        approximateLayout = createApproximateOutputLayout(context, &couldNotCreateApproximateLayout);
        if (couldNotCreateApproximateLayout) {
          Poincare::ExceptionCheckpoint::Raise();
        }
      }
    }

    KDCoordinate approximateOutputHeight = approximateLayout.layoutSize().height();
    if (displayOutput(context) == DisplayOutput::ApproximateOnly || (!expanded && displayOutput(context) == DisplayOutput::ExactAndApproximateToggle)) {
      if (allExpressionsInline) {
        KDCoordinate approximateOutputBaseline = approximateLayout.baseline();
        result = std::max(inputBaseline, approximateOutputBaseline) + std::max(inputHeight - inputBaseline, approximateOutputHeight-approximateOutputBaseline);
      } else {
        result = inputHeight+approximateOutputHeight;
      }
    } else {
      assert(displayOutput(context) == DisplayOutput::ExactAndApproximate || (displayOutput(context) == DisplayOutput::ExactAndApproximateToggle && expanded));
      KDCoordinate exactOutputHeight = exactLayout.layoutSize().height();
      KDCoordinate exactOutputBaseline = exactLayout.baseline();
      KDCoordinate approximateOutputBaseline = approximateLayout.baseline();
      if (allExpressionsInline) {
        result = std::max(inputBaseline, std::max(exactOutputBaseline, approximateOutputBaseline)) + std::max(inputHeight - inputBaseline, std::max(exactOutputHeight - exactOutputBaseline, approximateOutputHeight-approximateOutputBaseline));
      } else {
        KDCoordinate outputHeight = std::max(exactOutputBaseline, approximateOutputBaseline) + std::max(exactOutputHeight-exactOutputBaseline, approximateOutputHeight-approximateOutputBaseline);
        result = inputHeight + outputHeight;
      }
    }
  }

  /* For all display outputs except ExactAndApproximateToggle, the selected
   * height and the usual height are identical. We update both heights in
   * theses cases. */
  if (displayOutput(context) != DisplayOutput::ExactAndApproximateToggle) {
    m_height = result;
    m_expandedHeight = result;
  } else {
    if (expanded) {
      m_expandedHeight = result;
    } else {
      m_height = result;
    }
  }
  return result;
}

Calculation::DisplayOutput Calculation::displayOutput(Context * context) {
  if (m_displayOutput != DisplayOutput::Unknown) {
    return m_displayOutput;
  }
  if (shouldOnlyDisplayExactOutput()) {
    m_displayOutput = DisplayOutput::ExactOnly;
  } else if (
      /* If the exact and approximate outputs are equal (with the
       * UserDefined number of significant digits), do not display the exact
       * output. Indeed, in this case, the layouts are identical. */
      strcmp(exactOutputText(), approximateOutputText(NumberOfSignificantDigits::UserDefined)) == 0
      ||
      // If the approximate output is 'unreal' or the exact result is 'undef'
      strcmp(exactOutputText(), Undefined::Name()) == 0 ||
      strcmp(approximateOutputText(NumberOfSignificantDigits::Maximal), Unreal::Name()) == 0
      ||
      /* If the approximate output is 'undef' and the input and exactOutput are
       * equal */
      (strcmp(approximateOutputText(NumberOfSignificantDigits::Maximal), Undefined::Name()) == 0 &&
       strcmp(inputText(), exactOutputText()) == 0)
      ||
      // Force all outputs to be ApproximateOnly if required by the exam mode configuration
      ExamModeConfiguration::exactExpressionsAreForbidden(GlobalPreferences::sharedGlobalPreferences()->examMode())
      ||
      /* If the input contains the following types, we only display the
       * approximate output. */
      input().recursivelyMatches(
        [](const Expression e, Context * c) {
          ExpressionNode::Type approximateOnlyTypes[] = {
            ExpressionNode::Type::Random,
            ExpressionNode::Type::Unit,
            ExpressionNode::Type::Round,
            ExpressionNode::Type::FracPart,
            ExpressionNode::Type::Integral,
            ExpressionNode::Type::Product,
            ExpressionNode::Type::Sum,
            ExpressionNode::Type::Derivative,
            ExpressionNode::Type::ConfidenceInterval,
            ExpressionNode::Type::PredictionInterval
          };
          return e.isOfType(approximateOnlyTypes, sizeof(approximateOnlyTypes)/sizeof(ExpressionNode::Type));
        }, context, true)
  )
  {
    m_displayOutput = DisplayOutput::ApproximateOnly;
  } else if (input().recursivelyMatches(Expression::IsApproximate, context)
      || exactOutput().recursivelyMatches(Expression::IsApproximate, context))
  {
    m_displayOutput = DisplayOutput::ExactAndApproximateToggle;
  } else {
    m_displayOutput = DisplayOutput::ExactAndApproximate;
  }
  return m_displayOutput;
}

void Calculation::forceDisplayOutput(DisplayOutput d) {
  m_displayOutput = d;
  // Reset heights memoization as it might have changed when we modify the display output
  m_height = -1;
  m_expandedHeight = -1;
}
bool Calculation::shouldOnlyDisplayExactOutput() {
  /* If the input is a "store in a function", do not display the approximate
   * result. This prevents x->f(x) from displaying x = undef. */
  Expression i = input();
  return i.type() == ExpressionNode::Type::Store
    && i.childAtIndex(1).type() == ExpressionNode::Type::Function;
}

Calculation::EqualSign Calculation::exactAndApproximateDisplayedOutputsAreEqual(Poincare::Context * context) {
  if (m_equalSign != EqualSign::Unknown) {
    return m_equalSign;
  }
  /* Displaying the right equal symbol is less important than displaying a
   * result, so we do not want exactAndApproximateDisplayedOutputsAreEqual to
   * create a pool failure that would prevent from displaying a result that we
   * managed to compute. We thus encapsulate the method in an exception
   * checkpoint: if there was not enough memory on the pool to compute the equal
   * sign, just return EqualSign::Approximation.
   * We can safely use an exception checkpoint here because we are sure of not
   * modifying any pre-existing node in the pool. We are sure there cannot be a
   * Store in the exactOutput. */
  Poincare::ExceptionCheckpoint ecp;
  if (ExceptionRun(ecp)) {
    Preferences * preferences = Preferences::sharedPreferences();
    Preferences::ComplexFormat complexFormat = Expression::UpdatedComplexFormatWithTextInput(preferences->complexFormat(), m_inputText);
    m_equalSign = Expression::ParsedExpressionsAreEqual(exactOutputText(), approximateOutputText(NumberOfSignificantDigits::UserDefined), context, complexFormat, preferences->angleUnit()) ? EqualSign::Equal : EqualSign::Approximation;
    return m_equalSign;
  } else {
    /* Do not override m_equalSign in case there is enough room in the pool
     * later to compute it. */
    return EqualSign::Approximation;
  }
}

Calculation::AdditionalInformationType Calculation::additionalInformationType(Context * context) {
  Preferences * preferences = Preferences::sharedPreferences();
  Preferences::ComplexFormat complexFormat = Expression::UpdatedComplexFormatWithTextInput(preferences->complexFormat(), m_inputText);
  Expression i = input();
  Expression o = exactOutput();
  /* Special case for Equal and Store:
   * Equal/Store nodes have to be at the root of the expression, which prevents
   * from creating new expressions with equal/store node as a child. We don't
   * return any additional outputs for them to avoid bothering with special
   * cases. */
  if (i.type() == ExpressionNode::Type::Equal || i.type() == ExpressionNode::Type::Store) {
    return AdditionalInformationType::None;
  }
  /* Trigonometry additional results are displayed if either input or output is a sin or a cos. Indeed, we want to capture both cases:
   * - > input: cos(60)
   *   > output: 1/2
   * - > input: 2cos(2) - cos(2)
   *   > output: cos(2)
   */
  if (input().isDefinedCosineOrSine(context, complexFormat, preferences->angleUnit()) || o.isDefinedCosineOrSine(context, complexFormat, preferences->angleUnit())) {
    return AdditionalInformationType::Trigonometry;
  }

  // TODO: return AdditionalInformationType::Unit
  if (o.isBasedIntegerCappedBy(k_maximalIntegerWithAdditionalInformation)) {
    return AdditionalInformationType::Integer;
  }
  // Find forms like [12]/[23] or -[12]/[23]
  if (o.isDivisionOfIntegers() || (o.type() == ExpressionNode::Type::Opposite && o.childAtIndex(0).isDivisionOfIntegers())) {
    return AdditionalInformationType::Rational;
  }
  if (o.hasDefinedComplexApproximation(context, complexFormat, preferences->angleUnit())) {
    return AdditionalInformationType::Complex;
  }
  return AdditionalInformationType::None;
}

}
