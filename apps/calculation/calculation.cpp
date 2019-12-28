#include "calculation.h"
#include "../shared/poincare_helpers.h"
#include "../global_preferences.h"
#include <poincare/exception_checkpoint.h>
#include <poincare/undefined.h>
#include <poincare/unreal.h>
#include <string.h>
#include <cmath>

using namespace Poincare;
using namespace Shared;

namespace Calculation {

static inline KDCoordinate maxCoordinate(KDCoordinate x, KDCoordinate y) { return x > y ? x : y; }

bool Calculation::operator==(const Calculation& c) {
  return strcmp(inputText(), c.inputText()) == 0
      && strcmp(approximateOutputText(), c.approximateOutputText()) == 0
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
  for (int i = 0; i < 3; i++) {
    result = result + strlen(result) + 1; // Pass inputText, exactOutputText, ApproximateOutputText
  }
  return reinterpret_cast<Calculation *>(const_cast<char *>(result));
}

void Calculation::tidy() {
  /* Reset height memoization (the complex format could have changed when
   * re-entering Calculation app which would impact the heights). */
  m_height = -1;
  m_expandedHeight = -1;
}

const char * Calculation::approximateOutputText() const {
  const char * exactOutput = exactOutputText();
  return exactOutput + strlen(exactOutput) + 1;
}

Expression Calculation::input() {
  return Expression::Parse(m_inputText);
}

Expression Calculation::exactOutput() {
  /* Because the angle unit might have changed, we do not simplify again. We
   * thereby avoid turning cos(Pi/4) into sqrt(2)/2 and displaying
   * 'sqrt(2)/2 = 0.999906' (which is totally wrong) instead of
   * 'cos(pi/4) = 0.999906' (which is true in degree). */
  Expression exactOutput = Expression::Parse(exactOutputText());
  assert(!exactOutput.isUninitialized());
  return exactOutput;
}

Expression Calculation::approximateOutput(Context * context) {
  /* To ensure that the expression 'm_output' is a matrix or a complex, we
   * call 'evaluate'. */
  Expression exp = Expression::Parse(approximateOutputText());
  assert(!exp.isUninitialized());
  return PoincareHelpers::Approximate<double>(exp, context);
}

Layout Calculation::createInputLayout() {
  return input().createLayout(Preferences::PrintFloatMode::Decimal, PrintFloat::k_numberOfStoredSignificantDigits);
}

Layout Calculation::createExactOutputLayout() {
  return PoincareHelpers::CreateLayout(exactOutput());
}

Layout Calculation::createApproximateOutputLayout(Context * context) {
  return PoincareHelpers::CreateLayout(approximateOutput(context));
}

KDCoordinate Calculation::height(Context * context, bool expanded) {
  KDCoordinate result =  expanded ? m_expandedHeight : m_height;
  if (result < 0) {
    DisplayOutput display = displayOutput(context);
    Layout inputLayout = createInputLayout();
    KDCoordinate inputHeight = inputLayout.layoutSize().height();
    if (display == DisplayOutput::ExactOnly) {
      KDCoordinate exactOutputHeight = createExactOutputLayout().layoutSize().height();
      result = inputHeight+exactOutputHeight;
    } else if (display == DisplayOutput::ApproximateOnly || (!expanded && display == DisplayOutput::ExactAndApproximateToggle)) {
      KDCoordinate approximateOutputHeight = createApproximateOutputLayout(context).layoutSize().height();
      result = inputHeight+approximateOutputHeight;
    } else {
      assert(display == DisplayOutput::ExactAndApproximate || (display == DisplayOutput::ExactAndApproximateToggle && expanded));
      Layout approximateLayout = createApproximateOutputLayout(context);
      Layout exactLayout = createExactOutputLayout();
      KDCoordinate approximateOutputHeight = approximateLayout.layoutSize().height();
      KDCoordinate exactOutputHeight = exactLayout.layoutSize().height();
      KDCoordinate outputHeight = maxCoordinate(exactLayout.baseline(), approximateLayout.baseline()) + maxCoordinate(exactOutputHeight-exactLayout.baseline(), approximateOutputHeight-approximateLayout.baseline());
      result = inputHeight + outputHeight;
    }
    /* For all display output except ExactAndApproximateToggle, the selected
     * height and the usual height are identical. We update both heights in
     * theses cases. */
    if (display != DisplayOutput::ExactAndApproximateToggle) {
      m_height = result;
      m_expandedHeight = result;
    } else {
      if (expanded) {
        m_expandedHeight = result;
      } else {
        m_height = result;
      }
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
  // Force all results to be ApproximateOnly in Dutch exam mode
  } else if (GlobalPreferences::sharedGlobalPreferences()->examMode() == GlobalPreferences::ExamMode::Dutch ||
      input().recursivelyMatches(
        [](const Expression e, Context * c) {
          constexpr int approximateOnlyTypesCount = 9;
          /* If the input contains the following types, we only display the
           * approximate output. */
          ExpressionNode::Type approximateOnlyTypes[approximateOnlyTypesCount] = {
            ExpressionNode::Type::Random,
            ExpressionNode::Type::Round,
            ExpressionNode::Type::FracPart,
            ExpressionNode::Type::Integral,
            ExpressionNode::Type::Product,
            ExpressionNode::Type::Sum,
            ExpressionNode::Type::Derivative,
            ExpressionNode::Type::ConfidenceInterval,
            ExpressionNode::Type::PredictionInterval
          };
          return e.isOfType(approximateOnlyTypes, approximateOnlyTypesCount);
        }, context, true))
  {
    m_displayOutput = DisplayOutput::ApproximateOnly;
  } else if (strcmp(exactOutputText(), approximateOutputText()) == 0) {
    /* If the exact and approximate results' texts are equal and their layouts
     * too, do not display the exact result. If the two layouts are not equal
     * because of the number of significant digits, we display both. */
    m_displayOutput = exactAndApproximateDisplayedOutputsAreEqual(context) == Calculation::EqualSign::Equal ? DisplayOutput::ApproximateOnly : DisplayOutput::ExactAndApproximate;
  } else if (strcmp(exactOutputText(), Undefined::Name()) == 0
      || strcmp(approximateOutputText(), Unreal::Name()) == 0
      || exactOutput().type() == ExpressionNode::Type::Undefined)
  {
    // If the approximate result is 'unreal' or the exact result is 'undef'
    m_displayOutput = DisplayOutput::ApproximateOnly;
  } else if (strcmp(approximateOutputText(), Undefined::Name()) == 0
      && strcmp(inputText(), exactOutputText()) == 0)
  {
    /* If the approximate result is 'undef' and the input and exactOutput are
     * equal */
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
    constexpr int bufferSize = Constant::MaxSerializedExpressionSize + 30;
    char buffer[bufferSize];
    Preferences * preferences = Preferences::sharedPreferences();
    Expression exactOutputExpression = PoincareHelpers::ParseAndSimplify(exactOutputText(), context, false);
    if (exactOutputExpression.isUninitialized()) {
      exactOutputExpression = Undefined::Builder();
    }
    Preferences::ComplexFormat complexFormat = Expression::UpdatedComplexFormatWithTextInput(preferences->complexFormat(), m_inputText);
    m_equalSign = exactOutputExpression.isEqualToItsApproximationLayout(approximateOutput(context), buffer, bufferSize, complexFormat, preferences->angleUnit(), preferences->displayMode(), preferences->numberOfSignificantDigits(), context) ? EqualSign::Equal : EqualSign::Approximation;
    return m_equalSign;
  } else {
    /* Do not override m_equalSign in case there is enough room in the pool
     * later to compute it. */
    return EqualSign::Approximation;
  }
}

}
