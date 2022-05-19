#include <quiz.h>
#include <apps/shared/global_context.h>
#include <poincare/test/helper.h>
#include <poincare/preferences.h>
#include <poincare_expressions.h>
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

KDCoordinate dummyHeight(::Calculation::Calculation * c, Poincare::Context * context, bool expanded) { return 0; }

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
  /* Checking if the store handles correctly the delete of older calculations
   * when full. */
  constexpr int minimalSize = ::Calculation::Calculation::k_minimalSize + sizeof(::Calculation::Calculation *);
  constexpr const char * pattern = "123456789+";
  constexpr int patternSize = 10;
  assert(strlen(pattern) == patternSize);

  // Case 1 : Remaining space < minimalSize
  {
    constexpr int calculationSize = 200;
    assert(calculationSize < store.remainingBufferSize());
    assert(calculationSize % patternSize == 0);
    char text[calculationSize];
    for (int i = 0; i < calculationSize; i += patternSize) {
      memcpy(text + i, pattern, patternSize);
    }
    text[calculationSize - 1] = '\0';

    while (store.remainingBufferSize() > minimalSize) {
      store.push(text, &globalContext, dummyHeight);
    }
    int numberOfCalculations1 = store.numberOfCalculations();
    /* The buffer is now too full to push a new calculation.
     * Trying to push a new one should delete the oldest one. Alter new text to
     * distinguish it from previously pushed ones. */
    text[0] = '9';
    Shared::ExpiringPointer<::Calculation::Calculation> pushedCalculation = store.push(text, &globalContext, dummyHeight);
    // Assert pushed text is correct
    quiz_assert(strcmp(store.calculationAtIndex(0)->inputText(), text) == 0);
    quiz_assert(strcmp(pushedCalculation->inputText(), text) == 0);
    int numberOfCalculations2 = store.numberOfCalculations();
    // The numberOfCalculations should be the same
    quiz_assert(numberOfCalculations1 == numberOfCalculations2);
  }
  store.deleteAll();

  // Case 2 : Remaining space > minimalSize but pushed calculation doesn't fit
  {
    constexpr int calculationSize = 2*minimalSize - (2*minimalSize)%patternSize;
    assert(calculationSize % patternSize == 0);
    assert(calculationSize < store.remainingBufferSize());
    char text[calculationSize];
    for (int i = 0; i < calculationSize; i += patternSize) {
      memcpy(text + i, pattern, patternSize);
    }
    text[calculationSize - 1] = '\0';

    // Push big calculations until approaching the limit
    while (store.remainingBufferSize() > 2 * minimalSize) {
      store.push(text, &globalContext, dummyHeight);
    }
    /* Push small calculations so that remainingBufferSize remain bigger, but gets
     * closer to minimalSize */
    while (store.remainingBufferSize() > minimalSize + minimalSize/2) {
      store.push("1", &globalContext, dummyHeight);
    }
    assert(store.remainingBufferSize() > minimalSize);
    int numberOfCalculations1 = store.numberOfCalculations();
    /* The buffer is now too full to push a new calculation.
     * Trying to push a new one should delete older ones. Alter new text to
     * distinguish it from previously pushed ones. */
    text[0] = '9';
    Shared::ExpiringPointer<::Calculation::Calculation> pushedCalculation = store.push(text, &globalContext, dummyHeight);
    quiz_assert(strcmp(store.calculationAtIndex(0)->inputText(), text) == 0);
    quiz_assert(strcmp(pushedCalculation->inputText(), text) == 0);
    int numberOfCalculations2 = store.numberOfCalculations();
    // The numberOfCalculations should be the equal or smaller
    quiz_assert(numberOfCalculations1 >= numberOfCalculations2);
  }
  store.deleteAll();
  quiz_assert(store.remainingBufferSize() == store.bufferSize());
}

void assertAnsIs(const char * input, const char * expectedAnsInputText, Context * context, CalculationStore * store) {
  store->push(input, context, dummyHeight);
  store->push("Ans", context, dummyHeight);
  Shared::ExpiringPointer<::Calculation::Calculation> lastCalculation = store->calculationAtIndex(0);
  quiz_assert(strcmp(lastCalculation->inputText(), expectedAnsInputText) == 0);
}

QUIZ_CASE(calculation_ans) {
  Shared::GlobalContext globalContext;
  CalculationStore store(calculationBuffer,calculationBufferSize);
  // Setup complex format and exam mode
  Poincare::Preferences::ComplexFormat previousComplexFormat = Poincare::Preferences::sharedPreferences()->complexFormat();
  Poincare::Preferences::ExamMode previousExamMode = Poincare::Preferences::sharedPreferences()->examMode();
  Poincare::Preferences::sharedPreferences()->setComplexFormat(Poincare::Preferences::ComplexFormat::Real);
  Poincare::Preferences::sharedPreferences()->setExamMode(Poincare::Preferences::ExamMode::Off);

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
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("a.exp").destroy();

  assertAnsIs("1+1", "2", &globalContext, &store);
  assertAnsIs("13.3", "13.3", &globalContext, &store);
  assertAnsIs("√(-1-1)", "√(-1-1)", &globalContext, &store);
  assertAnsIs("int(diff(x^2,x,x),x,0,1)", "int(diff(x^2,x,x),x,0,1)", &globalContext, &store);

  assertAnsIs("√(1+1)", "√(2)", &globalContext, &store);

  Poincare::Preferences::sharedPreferences()->setExamMode(Poincare::Preferences::ExamMode::Dutch);
  assert(ExamModeConfiguration::exactExpressionIsForbidden(SquareRoot::Builder(Rational::Builder(2))));

  assertAnsIs("√(1+1)", "√(1+1)", &globalContext, &store);

  // Restore complex format and exam mode
  Poincare::Preferences::sharedPreferences()->setExamMode(previousExamMode);
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

  assertCalculationIs("11123456789", DisplayOutput::ExactAndApproximate, EqualSign::Approximation, "11123456789", "1.112345679ᴇ10", "11123456789", &globalContext, &store);
  assertCalculationIs("1123456789", DisplayOutput::ApproximateOnly, EqualSign::Equal, "1123456789", "1123456789", "1123456789", &globalContext, &store);

}

QUIZ_CASE(calculation_display_exact_approximate) {
  Shared::GlobalContext globalContext;
  CalculationStore store(calculationBuffer,calculationBufferSize);

  Preferences::AngleUnit previousAngleUnit = Preferences::sharedPreferences()->angleUnit();
  Preferences::sharedPreferences()->setAngleUnit(Preferences::AngleUnit::Degree);

  assertCalculationIs("1/2", DisplayOutput::ExactAndApproximate, EqualSign::Equal, nullptr, nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("1/3", DisplayOutput::ExactAndApproximate, EqualSign::Approximation, nullptr, nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("1/0", DisplayOutput::ApproximateOnly, EqualSign::Unknown, "undef", "undef", "undef", &globalContext, &store);
  assertCalculationIs("2x-x", DisplayOutput::ApproximateOnly, EqualSign::Unknown, "undef", "undef", "undef", &globalContext, &store);
  assertCalculationIs("[[1,2,3]]", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("[[1,x,3]]", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "[[1,undef,3]]", "[[1,undef,3]]", &globalContext, &store);
  assertCalculationIs("[[1/0,2/0]]", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "[[undef,undef]]", "[[undef,undef]]", &globalContext, &store);
  assertCalculationIs("{1/0,2/0}", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "{undef,undef}", "{undef,undef}", &globalContext, &store);
  assertCalculationIs("28^7", DisplayOutput::ExactAndApproximate, EqualSign::Unknown, nullptr, nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("3+√(2)→a", DisplayOutput::ExactAndApproximate, EqualSign::Approximation, "3+√(2)", nullptr, nullptr, &globalContext, &store);
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("a.exp").destroy();
  assertCalculationIs("3+2→a", DisplayOutput::ApproximateOnly, EqualSign::Equal, "5", "5", "5", &globalContext, &store);
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("a.exp").destroy();
  assertCalculationIs("3→a", DisplayOutput::ApproximateOnly, EqualSign::Equal, "3", "3", "3", &globalContext, &store);
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("a.exp").destroy();
  assertCalculationIs("3+x→f(x)", DisplayOutput::ExactOnly, EqualSign::Unknown, "3+x", nullptr, nullptr, &globalContext, &store);
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("f.func").destroy();
  assertCalculationIs("1+1+random()", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("1+1+round(1.343,2)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "3.34", "3.34", &globalContext, &store);
  assertCalculationIs("randint(2,2)+3", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "5", "5", &globalContext, &store);
  assertCalculationIs("√(8)", DisplayOutput::ExactAndApproximate, EqualSign::Unknown, nullptr, nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("cos(45)", DisplayOutput::ExactAndApproximate, EqualSign::Unknown, nullptr, nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("cos(π/2)", DisplayOutput::ExactAndApproximate, EqualSign::Unknown, nullptr, nullptr, nullptr, &globalContext, &store);

  // Exact output that have dependencies are not displayed
  assertCalculationIs("sum(1/k,k,0,5)→a", DisplayOutput::ApproximateOnly, EqualSign::Approximation, "sum(1/k,k,0,5)", "undef", "undef", &globalContext, &store);
  assertCalculationIs("2→f(x)", DisplayOutput::ExactOnly, EqualSign::Equal, "2", "2", "2", &globalContext, &store);
  assertCalculationIs("f(a)", DisplayOutput::ApproximateOnly, EqualSign::Approximation, "dep\U00000014(2,{sum(1/k,k,0,5)})", "undef", "undef", &globalContext, &store);
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("a.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("f.func").destroy();

  Poincare::Preferences::ExamMode previousExamMode = Poincare::Preferences::sharedPreferences()->examMode();
  Poincare::Preferences::sharedPreferences()->setExamMode(Poincare::Preferences::ExamMode::Dutch);

  assertCalculationIs("1+1", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("1/2", DisplayOutput::ExactAndApproximate, EqualSign::Equal, "1/2", "0.5", nullptr, &globalContext, &store);
  assertCalculationIs("0.5", DisplayOutput::ExactAndApproximateToggle, EqualSign::Equal, "1/2", "0.5", nullptr, &globalContext, &store);
  assertCalculationIs("√(8)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("cos(45)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("cos(π/2)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("cos(π/2)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("_G", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("_g0", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, nullptr, nullptr, &globalContext, &store);

  Poincare::Preferences::sharedPreferences()->setExamMode(previousExamMode);
  Preferences::sharedPreferences()->setAngleUnit(previousAngleUnit);
}

void assertMainCalculationOutputIs(const char * input, const char * output, Context * context, CalculationStore * store) {
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
  assertMainCalculationOutputIs("x+x+1+3+√(π)",        "undef", &globalContext, &store);
  assertMainCalculationOutputIs("f(x)",                "undef", &globalContext, &store);
  assertMainCalculationOutputIs("1+x→f(x)",            "1+x",   &globalContext, &store);
  assertMainCalculationOutputIs("f(x)",                "undef", &globalContext, &store);
  assertMainCalculationOutputIs("f(2)",                "3",     &globalContext, &store);
  assertMainCalculationOutputIs("2→x",                 "2",     &globalContext, &store);
  assertMainCalculationOutputIs("f(x)",                "3",     &globalContext, &store);
  assertMainCalculationOutputIs("x+x+1+3+√(π)",        "8+√(π)",&globalContext, &store);
  // Destroy records
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("f.func").destroy();
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("x.exp").destroy();

  // 1 - Predefined variable in fraction in integral
  assertMainCalculationOutputIs("int(x+1/x,x,1,2)", "2.193147181", &globalContext, &store);
  assertMainCalculationOutputIs("1→x",                 "1"    , &globalContext, &store);
  assertMainCalculationOutputIs("int(x+1/x,x,1,2)", "2.193147181", &globalContext, &store);
  // Destroy records
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("x.exp").destroy();

  // 2 - Circularly defined functions
  //   A - f(x) = g(x) = f(x)
  assertMainCalculationOutputIs("1→f(x)",              "1",     &globalContext, &store);
  assertMainCalculationOutputIs("f(x)→g(x)",           "f(x)",  &globalContext, &store);
  assertMainCalculationOutputIs("g(x)→f(x)",           "g(x)",  &globalContext, &store);
  // With x undefined
  assertMainCalculationOutputIs("f(x)",                "undef", &globalContext, &store);
  assertMainCalculationOutputIs("diff(f(x),x,1)",      "undef", &globalContext, &store);
  // With x  defined
  assertMainCalculationOutputIs("1→x",                 "1",     &globalContext, &store);

  assertMainCalculationOutputIs("f(x)",                "undef", &globalContext, &store);
  assertMainCalculationOutputIs("diff(f(x),x,1)",      "undef", &globalContext, &store);
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("x.exp").destroy();
  //   B - f(x) = g(x) = a = f(x)
  assertMainCalculationOutputIs("f(x)→a",              "undef", &globalContext, &store);
  assertMainCalculationOutputIs("a→g(x)",              "a",     &globalContext, &store);
  // With x undefined
  assertMainCalculationOutputIs("f(x)",                "undef", &globalContext, &store);
  assertMainCalculationOutputIs("diff(f(x),x,1)",      "undef", &globalContext, &store);
  // With x defined
  assertMainCalculationOutputIs("1→x",                 "1",     &globalContext, &store);

  assertMainCalculationOutputIs("f(x)",                "undef", &globalContext, &store);
  assertMainCalculationOutputIs("diff(f(x),x,1)",      "undef", &globalContext, &store);
  // Destroy records
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("x.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("a.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("f.func").destroy();
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("g.func").destroy();

  // 3 - Differences between functions and variables
  assertMainCalculationOutputIs("x+1→f(x)",            "x+1",   &globalContext, &store);
  assertMainCalculationOutputIs("x+1→y",               "undef", &globalContext, &store);
  // With x undefined
  assertMainCalculationOutputIs("y",                   "undef", &globalContext, &store);

  assertMainCalculationOutputIs("int(y,x,1,3)",        "undef", &globalContext, &store);
  assertMainCalculationOutputIs("sum(y,x,0,1)",        "undef", &globalContext, &store);
  assertMainCalculationOutputIs("product(y,x,0,1)",    "undef", &globalContext, &store);
  assertMainCalculationOutputIs("diff(y,x,1)",         "undef", &globalContext, &store);

  assertMainCalculationOutputIs("f(y)",                "undef", &globalContext, &store);
  assertMainCalculationOutputIs("diff(f(y),x,1)",      "undef", &globalContext, &store);
  assertMainCalculationOutputIs("diff(f(x)×y,x,1)",    "undef", &globalContext, &store);
  assertMainCalculationOutputIs("diff(f(x×y),x,1)",    "undef", &globalContext, &store);

  // With x defined
  assertMainCalculationOutputIs("1→x",                 "1",     &globalContext, &store);
  assertMainCalculationOutputIs("y",                   "undef", &globalContext, &store);
  assertMainCalculationOutputIs("x+1→y",               "2",     &globalContext, &store);
  assertMainCalculationOutputIs("y",                   "2",     &globalContext, &store);

  assertMainCalculationOutputIs("int(x+1,x,1,3)",      "6",     &globalContext, &store);
  assertMainCalculationOutputIs("int(f(x),x,1,3)",     "6",     &globalContext, &store);
  assertMainCalculationOutputIs("int(y,x,1,3)",        "4",     &globalContext, &store);

  assertMainCalculationOutputIs("sum(x+1,x,0,1)",      "3",     &globalContext, &store);
  assertMainCalculationOutputIs("sum(f(x),x,0,1)",     "3",     &globalContext, &store);
  assertMainCalculationOutputIs("sum(y,x,0,1)",        "4",     &globalContext, &store);

  assertMainCalculationOutputIs("product(x+1,x,0,1)",  "2",     &globalContext, &store);
  assertMainCalculationOutputIs("product(f(x),x,0,1)", "2",     &globalContext, &store);
  assertMainCalculationOutputIs("product(y,x,0,1)",    "4",     &globalContext, &store);

  assertMainCalculationOutputIs("diff(x+1,x,1)",       "1",     &globalContext, &store);
  assertMainCalculationOutputIs("diff(f(x),x,1)",      "1",     &globalContext, &store);
  assertMainCalculationOutputIs("diff(y,x,1)",         "0",     &globalContext, &store);

  assertMainCalculationOutputIs("f(y)",                "3",     &globalContext, &store);
  assertMainCalculationOutputIs("diff(f(y),x,1)",      "0",     &globalContext, &store);
  assertMainCalculationOutputIs("diff(f(x)×y,x,1)",    "2",     &globalContext, &store);
  assertMainCalculationOutputIs("diff(f(x×y),x,1)",    "2",     &globalContext, &store);
  // Destroy records
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("x.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("y.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("f.func").destroy();

  // 4 - Nested local variables within variables
  assertMainCalculationOutputIs("int(x+1,x,1,3)→a",    "6",     &globalContext, &store);
  assertMainCalculationOutputIs("a",                   "6",     &globalContext, &store);
  assertMainCalculationOutputIs("a+1→a",               "7",     &globalContext, &store);
  assertMainCalculationOutputIs("diff(y×a,y,1)",       "7",     &globalContext, &store);

  // Destroy records
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("y.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("a.exp").destroy();

  assertMainCalculationOutputIs("2→x",                 "2",     &globalContext, &store);
  assertMainCalculationOutputIs("diff(x,x,x)→a",       "1",     &globalContext, &store);
  assertMainCalculationOutputIs("diff(a,x,3)",         "0",     &globalContext, &store);
  // Destroy records
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("a.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("x.exp").destroy();

  // 5 - Double nested local variables within variables
  assertMainCalculationOutputIs("1→x",                 "1",     &globalContext, &store);
  assertMainCalculationOutputIs("x→a",                 "1",     &globalContext, &store);
  assertMainCalculationOutputIs("diff(a,x,x)→b",       "0",     &globalContext, &store);
  assertMainCalculationOutputIs("b",                   "0",     &globalContext, &store);
  // Destroy records
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("b.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("a.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("x.exp").destroy();
}

QUIZ_CASE(calculation_symbolic_computation_and_parametered_expressions) {
  Shared::GlobalContext globalContext;
  CalculationStore store(calculationBuffer,calculationBufferSize);

  assertCalculationIs("int((e^(-x))-x^(0.5), x, 0, 3)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, nullptr, nullptr, &globalContext, &store); // Tests a bug with symbolic computation
  assertCalculationIs("int(x,x,0,2)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "2", "2", &globalContext, &store);
  assertCalculationIs("sum(x,x,0,2)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "3", "3", &globalContext, &store);
  assertCalculationIs("product(x,x,1,2)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "2", "2", &globalContext, &store);
  assertCalculationIs("diff(x^2,x,3)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "6", "6", &globalContext, &store);
  assertCalculationIs("2→x", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("int(x,x,0,2)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "2", "2", &globalContext, &store);
  assertCalculationIs("sum(x,x,0,2)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "3", "3", &globalContext, &store);
  assertCalculationIs("product(x,x,1,2)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "2", "2", &globalContext, &store);
  assertCalculationIs("diff(x^2,x,3)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "6", "6", &globalContext, &store);

  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("x.exp").destroy();
}


QUIZ_CASE(calculation_complex_format) {
  Shared::GlobalContext globalContext;
  CalculationStore store(calculationBuffer,calculationBufferSize);

  Poincare::Preferences::sharedPreferences()->setComplexFormat(Poincare::Preferences::ComplexFormat::Real);
  assertCalculationIs("1+i", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "1+i", "1+i", &globalContext, &store);
  assertCalculationIs("√(-1)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, "nonreal", nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("ln(-2)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "nonreal", "nonreal", &globalContext, &store);
  assertCalculationIs("√(-1)×√(-1)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "nonreal", "nonreal", &globalContext, &store);
  assertCalculationIs("(-8)^(1/3)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "-2", "-2", &globalContext, &store);
  assertCalculationIs("(-8)^(2/3)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "4", "4", &globalContext, &store);
  assertCalculationIs("(-2)^(1/4)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "nonreal", "nonreal", &globalContext, &store);

  Poincare::Preferences::sharedPreferences()->setComplexFormat(Poincare::Preferences::ComplexFormat::Cartesian);
  assertCalculationIs("1+i", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "1+i", "1+i", &globalContext, &store);
  assertCalculationIs("√(-1)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "i", "i", &globalContext, &store);
  assertCalculationIs("ln(-2)", DisplayOutput::ExactAndApproximate, EqualSign::Approximation, "ln(-2)", nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("√(-1)×√(-1)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "-1", "-1", &globalContext, &store);
  assertCalculationIs("(-8)^(1/3)", DisplayOutput::ExactAndApproximate, EqualSign::Approximation, "1+√(3)×i", nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("(-8)^(2/3)", DisplayOutput::ExactAndApproximate, EqualSign::Approximation, "-2+2×√(3)×i", nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("(-2)^(1/4)", DisplayOutput::ExactAndApproximate, EqualSign::Approximation, "root(8,4)/2+root(8,4)/2×i", nullptr, nullptr, &globalContext, &store);

  Poincare::Preferences::sharedPreferences()->setComplexFormat(Poincare::Preferences::ComplexFormat::Polar);
  assertCalculationIs("1+i", DisplayOutput::ExactAndApproximate, EqualSign::Approximation, "√(2)×e^\u0012π/4×i\u0013", nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("√(-1)", DisplayOutput::ExactAndApproximate, EqualSign::Approximation, "e^\u0012π/2×i\u0013", nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("ln(-2)", DisplayOutput::ExactAndApproximate, EqualSign::Approximation, "ln(-2)", nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("√(-1)×√(-1)", DisplayOutput::ExactAndApproximate, EqualSign::Unknown, nullptr, "e^\u00123.141592654×i\u0013", "e^\u00123.1415926535898×i\u0013", &globalContext, &store);
  assertCalculationIs("(-8)^(1/3)", DisplayOutput::ExactAndApproximate, EqualSign::Approximation, "2×e^\u0012π/3×i\u0013", nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("(-8)^(2/3)", DisplayOutput::ExactAndApproximate, EqualSign::Approximation, "4×e^\u0012\u00122×π\u0013/3×i\u0013", nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("(-2)^(1/4)", DisplayOutput::ExactAndApproximate, EqualSign::Approximation, "root(2,4)×e^\u0012π/4×i\u0013", nullptr, nullptr, &globalContext, &store);

  Poincare::Preferences::sharedPreferences()->setComplexFormat(Poincare::Preferences::ComplexFormat::Cartesian);
}

QUIZ_CASE(calculation_involving_sequence) {
  Shared::GlobalContext globalContext;

  Shared::SequenceStore * seqStore = globalContext.sequenceStore();
  Ion::Storage::Record::ErrorStatus err = seqStore->addEmptyModel();
  assert(err == Ion::Storage::Record::ErrorStatus::None);
  Ion::Storage::Record record = seqStore->recordAtIndex(seqStore->numberOfModels()-1);
  Shared::Sequence * u = seqStore->modelForRecord(record);
  u->setType(Shared::Sequence::Type::Explicit);
  err = u->setContent("i", &globalContext);
  assert(err == Ion::Storage::Record::ErrorStatus::None);
  (void) err; // Silence compilation warning.

  CalculationStore calcStore(calculationBuffer,calculationBufferSize);

  assertMainCalculationOutputIs("√(i×u(0))×√(6)", "undef", &globalContext, &calcStore);
  seqStore->removeAll();
  seqStore->tidyDownstreamPoolFrom();
}
