#include <quiz.h>
#include <apps/global_preferences.h>
#include <apps/shared/global_context.h>
#include <poincare/test/helper.h>
#include <string.h>
#include <assert.h>
#include "../calculation_store.h"
#include "../../exam_mode_configuration.h"

typedef ::Calculation::Calculation::DisplayOutput DisplayOutput;
typedef ::Calculation::Calculation::EqualSign EqualSign ;
typedef ::Calculation::Calculation::NumberOfSignificantDigits NumberOfSignificantDigits;

using namespace Poincare;
using namespace Calculation;

static constexpr int calculationBufferSize = 10 * (sizeof(::Calculation::Calculation) + ::Calculation::Calculation::k_numberOfExpressions * ::Constant::MaxSerializedExpressionSize + sizeof(::Calculation::Calculation *));
char calculationBuffer[calculationBufferSize];


void assert_store_is(CalculationStore * store, const char * * result) {
  for (int i = 0; i < store->numberOfCalculations(); i++) {
    quiz_assert(strcmp(store->calculationAtIndex(i)->inputText(), result[i]) == 0);
  }
}

KDCoordinate dummyHeight(::Calculation::Calculation * c, bool expanded) { return 0; }

QUIZ_CASE(calculation_store) {
  Shared::GlobalContext globalContext;
  CalculationStore store(calculationBuffer,calculationBufferSize);
  // Store is now {9, 8, 7, 6, 5, 4, 3, 2, 1, 0}
  const char * result[] = {"9", "8", "7", "6", "5", "4", "3", "2", "1", "0"};
  for (int i = 0; i < 10; i++) {
    char text[2] = {(char)(i+'0'), 0};
    store.push(text, &globalContext, dummyHeight);
    quiz_assert(store.numberOfCalculations() == i+1);
  }
  assert_store_is(&store, result);

  for (int i = 9; i > 0; i = i-2) {
    store.deleteCalculationAtIndex(i);
  }
  // Store is now {9, 7, 5, 3, 1}
  const char * result2[] = {"9", "7", "5", "3", "1"};
  assert_store_is(&store, result2);

  store.deleteAll();

  // Checking if the store handles correctly the delete of the oldest calculation when full
  static int minSize = ::Calculation::Calculation::MinimalSize();
  char text[2] = {'0', 0};
  while (store.remainingBufferSize() > minSize) {
    store.push(text, &globalContext, dummyHeight);
  }
  int numberOfCalculations1 = store.numberOfCalculations();
  /* The buffer is now to  full to push a new calculation.
   * Trying to push a new one should delete the oldest one*/
  store.push(text, &globalContext, dummyHeight);
  int numberOfCalculations2 = store.numberOfCalculations();
  // The numberOfCalculations should be the same
  quiz_assert(numberOfCalculations1 == numberOfCalculations2);
  store.deleteAll();
  quiz_assert(store.remainingBufferSize() == store.bufferSize());
}

void assertAnsIs(const char * input, const char * expectedAnsInputText, Context * context, CalculationStore * store) {
  store->push(input, context, dummyHeight);
  store->push("ans", context, dummyHeight);
  Shared::ExpiringPointer<::Calculation::Calculation> lastCalculation = store->calculationAtIndex(0);
  quiz_assert(strcmp(lastCalculation->inputText(), expectedAnsInputText) == 0);
}

QUIZ_CASE(calculation_ans) {
  Shared::GlobalContext globalContext;
  CalculationStore store(calculationBuffer,calculationBufferSize);
  // Setup complex format and exam mode
  Poincare::Preferences::ComplexFormat previousComplexFormat = Poincare::Preferences::sharedPreferences()->complexFormat();
  GlobalPreferences::ExamMode previousExamMode = GlobalPreferences::sharedGlobalPreferences()->examMode();
  Poincare::Preferences::sharedPreferences()->setComplexFormat(Poincare::Preferences::ComplexFormat::Real);
  GlobalPreferences::sharedGlobalPreferences()->setExamMode(GlobalPreferences::ExamMode::Off);

  store.push("1+3/4", &globalContext, dummyHeight);
  store.push("ans+2/3", &globalContext, dummyHeight);
  Shared::ExpiringPointer<::Calculation::Calculation> lastCalculation = store.calculationAtIndex(0);
  quiz_assert(lastCalculation->displayOutput(&globalContext) == DisplayOutput::ExactAndApproximate);
  quiz_assert(strcmp(lastCalculation->exactOutputText(),"29/12") == 0);

  store.push("ans+0.22", &globalContext, dummyHeight);
  lastCalculation = store.calculationAtIndex(0);
  quiz_assert(lastCalculation->displayOutput(&globalContext) == DisplayOutput::ExactAndApproximateToggle);
  quiz_assert(strcmp(lastCalculation->approximateOutputText(NumberOfSignificantDigits::Maximal),"2.6366666666667") == 0);

  assertAnsIs("1+1→a", "2", &globalContext, &store);
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();

  assertAnsIs("1+1", "2", &globalContext, &store);
  assertAnsIs("13.3", "13.3", &globalContext, &store);
  assertAnsIs("√(-1-1)", "√(-1-1)", &globalContext, &store);
  assertAnsIs("int(diff(x^2,x,x),x,0,1)", "int(diff(x^2,x,x),x,0,1)", &globalContext, &store);

  assertAnsIs("√(1+1)", "√(2)", &globalContext, &store);

  GlobalPreferences::sharedGlobalPreferences()->setExamMode(GlobalPreferences::ExamMode::Dutch);
  assert(ExamModeConfiguration::exactExpressionsAreForbidden(GlobalPreferences::ExamMode::Dutch));

  assertAnsIs("√(1+1)", "√(1+1)", &globalContext, &store);

  // Restore complex format and exam mode
  GlobalPreferences::sharedGlobalPreferences()->setExamMode(previousExamMode);
  Poincare::Preferences::sharedPreferences()->setComplexFormat(previousComplexFormat);
  store.deleteAll();
}

void assertCalculationIs(const char * input, DisplayOutput display, EqualSign sign, const char * exactOutput, const char * displayedApproximateOutput, const char * storedApproximateOutput, Context * context, CalculationStore * store) {
  store->push(input, context, dummyHeight);
  Shared::ExpiringPointer<::Calculation::Calculation> lastCalculation = store->calculationAtIndex(0);
  quiz_assert(lastCalculation->displayOutput(context) == display);
  if (sign != EqualSign::Unknown) {
    quiz_assert(lastCalculation->exactAndApproximateDisplayedOutputsAreEqual(context) == sign);
  }
  if (exactOutput) {
    quiz_assert_print_if_failure(strcmp(lastCalculation->exactOutputText(), exactOutput) == 0, input);
  }
  if (displayedApproximateOutput) {
    quiz_assert_print_if_failure(strcmp(lastCalculation->approximateOutputText(NumberOfSignificantDigits::UserDefined), displayedApproximateOutput) == 0, input);
  }
  if (storedApproximateOutput) {
    quiz_assert_print_if_failure(strcmp(lastCalculation->approximateOutputText(NumberOfSignificantDigits::Maximal), storedApproximateOutput) == 0, input);
  }
  store->deleteAll();
}

QUIZ_CASE(calculation_significant_digits) {
  Shared::GlobalContext globalContext;
  CalculationStore store(calculationBuffer,calculationBufferSize);

  assertCalculationIs("123456789", DisplayOutput::ExactAndApproximate, EqualSign::Approximation, "123456789", "1.234568ᴇ8", "123456789", &globalContext, &store);
  assertCalculationIs("1234567", DisplayOutput::ApproximateOnly, EqualSign::Equal, "1234567", "1234567", "1234567", &globalContext, &store);

}

QUIZ_CASE(calculation_display_exact_approximate) {
  Shared::GlobalContext globalContext;
  CalculationStore store(calculationBuffer,calculationBufferSize);

  assertCalculationIs("1/2", DisplayOutput::ExactAndApproximate, EqualSign::Equal, nullptr, nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("1/3", DisplayOutput::ExactAndApproximate, EqualSign::Approximation, nullptr, nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("1/0", DisplayOutput::ApproximateOnly, EqualSign::Unknown, "undef", "undef", "undef", &globalContext, &store);
  assertCalculationIs("2x-x", DisplayOutput::ApproximateOnly, EqualSign::Unknown, "undef", "undef", "undef", &globalContext, &store);
  assertCalculationIs("[[1,2,3]]", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("[[1,x,3]]", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "undef", "undef", &globalContext, &store);
  assertCalculationIs("28^7", DisplayOutput::ExactAndApproximate, EqualSign::Unknown, nullptr, nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("3+√(2)→a", DisplayOutput::ExactAndApproximate, EqualSign::Approximation, "√(2)+3", nullptr, nullptr, &globalContext, &store);
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
  assertCalculationIs("3+2→a", DisplayOutput::ApproximateOnly, EqualSign::Equal, "5", "5", "5", &globalContext, &store);
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
  assertCalculationIs("3→a", DisplayOutput::ApproximateOnly, EqualSign::Equal, "3", "3", "3", &globalContext, &store);
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
  assertCalculationIs("3+x→f(x)", DisplayOutput::ExactOnly, EqualSign::Unknown, "x+3", nullptr, nullptr, &globalContext, &store);
  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();
  assertCalculationIs("1+1+random()", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("1+1+round(1.343,2)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "3.34", "3.34", &globalContext, &store);
  assertCalculationIs("randint(2,2)+3", DisplayOutput::ApproximateOnly, EqualSign::Unknown, "5", "5", "5", &globalContext, &store);
  assertCalculationIs("confidence(0.5,2)+3", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("prediction(0.5,2)+3", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("prediction95(0.5,2)+3", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, nullptr, nullptr, &globalContext, &store);

}

void simpleAssertCalculationIs(const char * input, const char * output, Context * context, CalculationStore * store) {
  // For the next test, we only need to checkout input and output text.
  store->push(input, context, dummyHeight);
  Shared::ExpiringPointer<::Calculation::Calculation> lastCalculation = store->calculationAtIndex(0);
  switch (lastCalculation->displayOutput(context)) {
    case DisplayOutput::ApproximateOnly:
      quiz_assert_print_if_failure(strcmp(lastCalculation->approximateOutputText(NumberOfSignificantDigits::UserDefined), output) == 0, input);
      break;
    default:
      quiz_assert_print_if_failure(strcmp(lastCalculation->exactOutputText(), output) == 0, input);
      break;
    }
  store->deleteAll();
}

QUIZ_CASE(calculation_symbolic_computation) {
  Shared::GlobalContext globalContext;
  CalculationStore store(calculationBuffer,calculationBufferSize);

  // 0 - General cases
  simpleAssertCalculationIs("x+x+1+3+√(π)",        "undef", &globalContext, &store);
  simpleAssertCalculationIs("f(x)",                "undef", &globalContext, &store);
  simpleAssertCalculationIs("1+x→f(x)",            "x+1",   &globalContext, &store);
  simpleAssertCalculationIs("f(x)",                "undef", &globalContext, &store);
  simpleAssertCalculationIs("f(2)",                "3",     &globalContext, &store);
  simpleAssertCalculationIs("2→x",                 "2",     &globalContext, &store);
  simpleAssertCalculationIs("f(x)",                "3",     &globalContext, &store);
  simpleAssertCalculationIs("x+x+1+3+√(π)",        "√(π)+8",&globalContext, &store);
  // Destroy records
  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();
  Ion::Storage::sharedStorage()->recordNamed("x.exp").destroy();

  // 1 - Predefined variable in fraction in integral
  simpleAssertCalculationIs("int(x+1/x,x,1,2)", "2.193147", &globalContext, &store);
  simpleAssertCalculationIs("1→x",                     "1", &globalContext, &store);
  simpleAssertCalculationIs("int(x+1/x,x,1,2)", "2.193147", &globalContext, &store);
  // Destroy records
  Ion::Storage::sharedStorage()->recordNamed("x.exp").destroy();

  // 2 - Circularly defined functions
  //   A - f(x) = g(x) = f(x)
  simpleAssertCalculationIs("1→f(x)",              "1",     &globalContext, &store);
  simpleAssertCalculationIs("f(x)→g(x)",           "f(x)",  &globalContext, &store);
  simpleAssertCalculationIs("g(x)→f(x)",           "g(x)",  &globalContext, &store);
  // With x undefined
  simpleAssertCalculationIs("f(x)",                "undef", &globalContext, &store);
  simpleAssertCalculationIs("diff(f(x),x,1)",      "undef", &globalContext, &store);
  // With x  defined
  simpleAssertCalculationIs("1→x",                 "1",     &globalContext, &store);

  simpleAssertCalculationIs("f(x)",                "undef", &globalContext, &store);
  simpleAssertCalculationIs("diff(f(x),x,1)",      "undef", &globalContext, &store);
  Ion::Storage::sharedStorage()->recordNamed("x.exp").destroy();
  //   B - f(x) = g(x) = a = f(x)
  simpleAssertCalculationIs("f(x)→a",              "undef", &globalContext, &store);
  simpleAssertCalculationIs("a→g(x)",              "a",     &globalContext, &store);
  // With x undefined
  simpleAssertCalculationIs("f(x)",                "undef", &globalContext, &store);
  simpleAssertCalculationIs("diff(f(x),x,1)",      "undef", &globalContext, &store);
  // With x defined
  simpleAssertCalculationIs("1→x",                 "1",     &globalContext, &store);

  simpleAssertCalculationIs("f(x)",                "undef", &globalContext, &store);
  simpleAssertCalculationIs("diff(f(x),x,1)",      "undef", &globalContext, &store);
  // Destroy records
  Ion::Storage::sharedStorage()->recordNamed("x.exp").destroy();
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();
  Ion::Storage::sharedStorage()->recordNamed("g.func").destroy();

  // 3 - Differences between functions and variables
  simpleAssertCalculationIs("x+1→f(x)",            "x+1",   &globalContext, &store);
  simpleAssertCalculationIs("x+1→y",               "undef", &globalContext, &store);
  // With x undefined
  simpleAssertCalculationIs("y",                   "undef", &globalContext, &store);

  simpleAssertCalculationIs("int(y,x,1,3)",        "undef", &globalContext, &store);
  simpleAssertCalculationIs("sum(y,x,0,1)",        "undef", &globalContext, &store);
  simpleAssertCalculationIs("product(y,x,0,1)",    "undef", &globalContext, &store);
  simpleAssertCalculationIs("diff(y,x,1)",         "undef", &globalContext, &store);

  simpleAssertCalculationIs("f(y)",                "undef", &globalContext, &store);
  simpleAssertCalculationIs("diff(f(y),x,1)",      "undef", &globalContext, &store);
  simpleAssertCalculationIs("diff(f(x)×y,x,1)",    "undef", &globalContext, &store);
  simpleAssertCalculationIs("diff(f(x×y),x,1)",    "undef", &globalContext, &store);

  // With x defined
  simpleAssertCalculationIs("1→x",                 "1",     &globalContext, &store);

  simpleAssertCalculationIs("y",                   "2",     &globalContext, &store);

  simpleAssertCalculationIs("int(x+1,x,1,3)",      "6",     &globalContext, &store);
  simpleAssertCalculationIs("int(f(x),x,1,3)",     "4",     &globalContext, &store);
  simpleAssertCalculationIs("int(y,x,1,3)",        "4",     &globalContext, &store);

  simpleAssertCalculationIs("sum(x+1,x,0,1)",      "3",     &globalContext, &store);
  simpleAssertCalculationIs("sum(f(x),x,0,1)",     "4",     &globalContext, &store);
  simpleAssertCalculationIs("sum(y,x,0,1)",        "4",     &globalContext, &store);

  simpleAssertCalculationIs("product(x+1,x,0,1)",  "2",     &globalContext, &store);
  simpleAssertCalculationIs("product(f(x),x,0,1)", "4",     &globalContext, &store);
  simpleAssertCalculationIs("product(y,x,0,1)",    "4",     &globalContext, &store);

  simpleAssertCalculationIs("diff(x+1,x,1)",       "1",     &globalContext, &store);
  simpleAssertCalculationIs("diff(f(x),x,1)",      "0",     &globalContext, &store);
  simpleAssertCalculationIs("diff(y,x,1)",         "0",     &globalContext, &store);

  simpleAssertCalculationIs("f(y)",                "3",     &globalContext, &store);
  simpleAssertCalculationIs("diff(f(y),x,1)",      "0",     &globalContext, &store);
  simpleAssertCalculationIs("diff(f(x)×y,x,1)",    "0",     &globalContext, &store);
  simpleAssertCalculationIs("diff(f(x×y),x,1)",    "0",     &globalContext, &store);
  // Destroy records
  Ion::Storage::sharedStorage()->recordNamed("x.exp").destroy();
  Ion::Storage::sharedStorage()->recordNamed("y.exp").destroy();
  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();
}

QUIZ_CASE(calculation_symbolic_computation_and_parametered_expressions) {
  Shared::GlobalContext globalContext;
  CalculationStore store(calculationBuffer,calculationBufferSize);

  assertCalculationIs("int((ℯ^(-x))-x^(0.5), x, 0, 3)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, nullptr, nullptr, &globalContext, &store); // Tests a bug with symbolic computation
  assertCalculationIs("int(x,x,0,2)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "2", "2", &globalContext, &store);
  assertCalculationIs("sum(x,x,0,2)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "3", "3", &globalContext, &store);
  assertCalculationIs("product(x,x,1,2)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "2", "2", &globalContext, &store);
  assertCalculationIs("diff(x^2,x,3)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "6", "6", &globalContext, &store);
  assertCalculationIs("2→x", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("int(x,x,0,2)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "2", "2", &globalContext, &store);
  assertCalculationIs("sum(x,x,0,2)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "3", "3", &globalContext, &store);
  assertCalculationIs("product(x,x,1,2)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "2", "2", &globalContext, &store);
  assertCalculationIs("diff(x^2,x,3)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "6", "6", &globalContext, &store);

  Ion::Storage::sharedStorage()->recordNamed("x.exp").destroy();
}


QUIZ_CASE(calculation_complex_format) {
  Shared::GlobalContext globalContext;
  CalculationStore store(calculationBuffer,calculationBufferSize);

  Poincare::Preferences::sharedPreferences()->setComplexFormat(Poincare::Preferences::ComplexFormat::Real);
  assertCalculationIs("1+𝐢", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "1+𝐢", "1+𝐢", &globalContext, &store);
  assertCalculationIs("√(-1)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, "unreal", nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("ln(-2)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "unreal", "unreal", &globalContext, &store);
  assertCalculationIs("√(-1)×√(-1)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "unreal", "unreal", &globalContext, &store);
  assertCalculationIs("(-8)^(1/3)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "-2", "-2", &globalContext, &store);
  assertCalculationIs("(-8)^(2/3)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "4", "4", &globalContext, &store);
  assertCalculationIs("(-2)^(1/4)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "unreal", "unreal", &globalContext, &store);

  Poincare::Preferences::sharedPreferences()->setComplexFormat(Poincare::Preferences::ComplexFormat::Cartesian);
  assertCalculationIs("1+𝐢", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "1+𝐢", "1+𝐢", &globalContext, &store);
  assertCalculationIs("√(-1)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "𝐢", "𝐢", &globalContext, &store);
  assertCalculationIs("ln(-2)", DisplayOutput::ExactAndApproximate, EqualSign::Approximation, "ln(-2)", nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("√(-1)×√(-1)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "-1", "-1", &globalContext, &store);
  assertCalculationIs("(-8)^(1/3)", DisplayOutput::ExactAndApproximate, EqualSign::Approximation, "1+√(3)×𝐢", nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("(-8)^(2/3)", DisplayOutput::ExactAndApproximate, EqualSign::Approximation, "-2+2×√(3)×𝐢", nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("(-2)^(1/4)", DisplayOutput::ExactAndApproximate, EqualSign::Approximation, "root(8,4)/2+root(8,4)/2×𝐢", nullptr, nullptr, &globalContext, &store);

  Poincare::Preferences::sharedPreferences()->setComplexFormat(Poincare::Preferences::ComplexFormat::Polar);
  assertCalculationIs("1+𝐢", DisplayOutput::ExactAndApproximate, EqualSign::Approximation, "√(2)×ℯ^\u0012π/4×𝐢\u0013", nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("√(-1)", DisplayOutput::ExactAndApproximate, EqualSign::Approximation, "ℯ^\u0012π/2×𝐢\u0013", nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("ln(-2)", DisplayOutput::ExactAndApproximate, EqualSign::Approximation, "ln(-2)", nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("√(-1)×√(-1)", DisplayOutput::ExactAndApproximate, EqualSign::Unknown, nullptr, "ℯ^\u00123.141593×𝐢\u0013", "ℯ^\u00123.1415926535898×𝐢\u0013", &globalContext, &store);
  assertCalculationIs("(-8)^(1/3)", DisplayOutput::ExactAndApproximate, EqualSign::Approximation, "2×ℯ^\u0012π/3×𝐢\u0013", nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("(-8)^(2/3)", DisplayOutput::ExactAndApproximate, EqualSign::Approximation, "4×ℯ^\u0012\u00122×π\u0013/3×𝐢\u0013", nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("(-2)^(1/4)", DisplayOutput::ExactAndApproximate, EqualSign::Approximation, "root(2,4)×ℯ^\u0012π/4×𝐢\u0013", nullptr, nullptr, &globalContext, &store);

  Poincare::Preferences::sharedPreferences()->setComplexFormat(Poincare::Preferences::ComplexFormat::Cartesian);
}
