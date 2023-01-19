#include <quiz.h>
#include <apps/shared/global_context.h>
#include <poincare/test/helper.h>
#include <poincare/preferences.h>
#include <poincare_expressions.h>
#include <string.h>
#include <assert.h>
#include "../calculation_store.h"
#include "../../exam_mode_configuration.h"

typedef ::Calculation::Calculation::AdditionalInformations AdditionalInformations;
typedef ::Calculation::Calculation::DisplayOutput DisplayOutput;
typedef ::Calculation::Calculation::EqualSign EqualSign ;
typedef ::Calculation::Calculation::NumberOfSignificantDigits NumberOfSignificantDigits;

using namespace Poincare;
using namespace Calculation;

constexpr static int calculationBufferSize = 10 * (sizeof(::Calculation::Calculation) + ::Calculation::Calculation::k_numberOfExpressions * ::Constant::MaxSerializedExpressionSize + sizeof(::Calculation::Calculation *));
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
  if (sign != EqualSign::Unknown && display != DisplayOutput::ApproximateOnly && display != DisplayOutput::ExactOnly) {
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
  assertCalculationIs("1123456789", DisplayOutput::ApproximateOnly, EqualSign::Unknown, "1123456789", "1123456789", "1123456789", &globalContext, &store);

}

QUIZ_CASE(calculation_display_exact_approximate) {
  Shared::GlobalContext globalContext;
  CalculationStore store(calculationBuffer,calculationBufferSize);

  Preferences::AngleUnit previousAngleUnit = Preferences::sharedPreferences()->angleUnit();
  Preferences::sharedPreferences()->setAngleUnit(Preferences::AngleUnit::Degree);

  assertCalculationIs("1/2", DisplayOutput::ExactAndApproximate, EqualSign::Equal, "1/2", "0.5", "0.5", &globalContext, &store);
  assertCalculationIs("1/3", DisplayOutput::ExactAndApproximate, EqualSign::Approximation, "1/3", "0.3333333333", "0.33333333333333", &globalContext, &store);
  assertCalculationIs("1/(2i)", DisplayOutput::ExactAndApproximate, EqualSign::Equal, "-1/2×i", "-0.5×i", "-0.5×i", &globalContext, &store);
  assertCalculationIs("1/0", DisplayOutput::ApproximateOnly, EqualSign::Unknown, "undef", "undef", "undef", &globalContext, &store);
  assertCalculationIs("2x-x", DisplayOutput::ApproximateOnly, EqualSign::Unknown, "undef", "undef", "undef", &globalContext, &store);
  assertCalculationIs("[[1,2,3]]", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "[[1,2,3]]", "[[1,2,3]]", &globalContext, &store);
  assertCalculationIs("[[1,x,3]]", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "[[1,undef,3]]", "[[1,undef,3]]", &globalContext, &store);
  assertCalculationIs("[[1/0,2/0]]", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "[[undef,undef]]", "[[undef,undef]]", &globalContext, &store);
  assertCalculationIs("{1/0,2/0}", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "{undef,undef}", "{undef,undef}", &globalContext, &store);
  assertCalculationIs("28^7", DisplayOutput::ExactAndApproximate, EqualSign::Unknown, "13492928512", "1.349292851ᴇ10", "13492928512", &globalContext, &store);
  assertCalculationIs("3+√(2)→a", DisplayOutput::ExactAndApproximate, EqualSign::Approximation, "3+√(2)", "4.414213562", "4.4142135623731", &globalContext, &store);
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("a.exp").destroy();
  assertCalculationIs("3+2→a", DisplayOutput::ApproximateOnly, EqualSign::Unknown, "5", "5", "5", &globalContext, &store);
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("a.exp").destroy();
  assertCalculationIs("3→a", DisplayOutput::ApproximateOnly, EqualSign::Unknown, "3", "3", "3", &globalContext, &store);
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("a.exp").destroy();
  assertCalculationIs("3/2→a", DisplayOutput::ExactAndApproximate, EqualSign::Unknown, "3/2", "1.5", "1.5", &globalContext, &store);
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("a.exp").destroy();
  assertCalculationIs("3+x→f(x)", DisplayOutput::ExactOnly, EqualSign::Unknown, "3+x", nullptr, nullptr, &globalContext, &store);
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("f.func").destroy();
  assertCalculationIs("1+1+random()", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("1+1+round(1.343,2)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "3.34", "3.34", &globalContext, &store);
  assertCalculationIs("randint(2,2)+3", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "5", "5", &globalContext, &store);
  assertCalculationIs("√(8)", DisplayOutput::ExactAndApproximate, EqualSign::Unknown, "2×√(2)", "2.828427125", "2.8284271247462", &globalContext, &store);
  assertCalculationIs("cos(45×_°)", DisplayOutput::ExactAndApproximate, EqualSign::Unknown, "√(2)/2", "0.7071067812", "0.70710678118655", &globalContext, &store);
  assertCalculationIs("cos(π/4×_rad)", DisplayOutput::ExactAndApproximate, EqualSign::Unknown, "√(2)/2", "0.7071067812", "0.70710678118655", &globalContext, &store);
  assertCalculationIs("cos(50×_°)", DisplayOutput::ExactAndApproximate, EqualSign::Unknown, "cos(50)", "0.6427876097", "0.64278760968654", &globalContext, &store);
  assertCalculationIs("binompdf(2,3,0.5)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "0.375", "0.375", &globalContext, &store);
  assertCalculationIs("1+2%", DisplayOutput::ExactAndApproximateToggle, EqualSign::Equal, "1×(1+2/100)", "1.02", "1.02", &globalContext, &store);
  assertCalculationIs("1-(1/3)%", DisplayOutput::ExactAndApproximateToggle, EqualSign::Approximation, "1×(1-\u00121/3\u0013/100)", "0.9966666667", "0.99666666666667", &globalContext, &store);

  // Exact output that have dependencies are not displayed
  assertCalculationIs("2→f(x)", DisplayOutput::ExactOnly, EqualSign::Unknown, "2", "2", "2", &globalContext, &store);
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("a.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("f.func").destroy();

  assertCalculationIs("(1/6)_g", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "1.666666667×_dg", "1.6666666666667×_dg", &globalContext, &store);
  assertCalculationIs("(1/6)_L_kg", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "1.666666667×_dg×_m^3", "1.6666666666667×_dg×_m^3", &globalContext, &store);
  assertCalculationIs("(π/6)_rad", DisplayOutput::ApproximateOnly, EqualSign::Approximation, nullptr, "30×_°", "30×_°", &globalContext, &store);
  assertCalculationIs("(1/11)_°", DisplayOutput::ApproximateOnly, EqualSign::Approximation, nullptr, "5.454545455×_'", "5.4545454545455×_'", &globalContext, &store);
  assertCalculationIs("180→rad", DisplayOutput::ExactAndApproximate, EqualSign::Approximation, "1×π×_rad", "3.141592654×_rad", "3.1415926535898×_rad", &globalContext, &store);
  assertCalculationIs("45→gon", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "50×_gon", "50×_gon", &globalContext, &store);
  Preferences::sharedPreferences()->setAngleUnit(Preferences::AngleUnit::Radian);
  assertCalculationIs("π/2→°", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "90×_°", "90×_°", &globalContext, &store);
  assertCalculationIs("πrad/2→°", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "90×_°", "90×_°", &globalContext, &store);
  assertCalculationIs("(1/6)_rad^(-1)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "0.1666666667×_rad^\u0012-1\u0013", "0.16666666666667×_rad^\u0012-1\u0013", &globalContext, &store);
  assertCalculationIs("diff(x^2,x,3)_rad", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "6×_rad", "6×_rad", &globalContext, &store);
  assertCalculationIs("(1/6)_rad→a", DisplayOutput::ExactAndApproximate, EqualSign::Approximation, "1/6×_rad", "0.1666666667×_rad", "0.16666666666667×_rad", &globalContext, &store);
  assertCalculationIs("diff(x^2,x,3)_rad→a", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "6×_rad", "6×_rad", &globalContext, &store);
  Preferences::sharedPreferences()->setAngleUnit(Preferences::AngleUnit::Degree);
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("a.exp").destroy();

  Poincare::Preferences::ExamMode previousExamMode = Poincare::Preferences::sharedPreferences()->examMode();
  Poincare::Preferences::sharedPreferences()->setExamMode(Poincare::Preferences::ExamMode::Dutch);

  assertCalculationIs("1+1", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "2", "2", &globalContext, &store);
  assertCalculationIs("1/2", DisplayOutput::ExactAndApproximate, EqualSign::Equal, "1/2", "0.5", nullptr, &globalContext, &store);
  assertCalculationIs("0.5", DisplayOutput::ExactAndApproximateToggle, EqualSign::Equal, "1/2", "0.5", nullptr, &globalContext, &store);
  assertCalculationIs("√(8)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr,"2.828427125", "2.8284271247462", &globalContext, &store);
  assertCalculationIs("cos(45×°)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "0.7071067812", "0.70710678118655", &globalContext, &store);
  assertCalculationIs("cos(π/4×_rad)", DisplayOutput::ApproximateOnly, EqualSign::Unknown, nullptr, "0.7071067812", "0.70710678118655", &globalContext, &store);
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

  // 6 - Define function from their own expression
  assertMainCalculationOutputIs("x+1→f(x)",            "x+1",   &globalContext, &store);
  assertMainCalculationOutputIs("f(x^2)→f(x)",         "x^2+1", &globalContext, &store);
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("f.func").destroy();

  // 7 - Circularly defined functions with exponential expression size growth
  assertMainCalculationOutputIs("x→f(x)",         "x", &globalContext, &store);
  assertMainCalculationOutputIs("f(xx)→g(x)", "f(x×x)", &globalContext, &store);
  assertMainCalculationOutputIs("g(xx)→f(x)", "g(x×x)", &globalContext, &store);
  assertMainCalculationOutputIs("g(2)",       "undef", &globalContext, &store);
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("f.func").destroy();
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("g.func").destroy();
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

bool operator ==(const AdditionalInformations &a, const AdditionalInformations &b) {
  // TODO C++20 Use a default comparison operator
  return a.integer == b.integer && a.rational == b.rational && a.directTrigonometry == b.directTrigonometry && a.inverseTrigonometry == b.inverseTrigonometry && a.unit == b.unit && a.matrix == b.matrix && a.vector == b.vector && a.complex == b.complex && a.function == b.function && a.scientificNotation == b.scientificNotation;
}

void assertCalculationAdditionalResultTypeHas(const char * input, const AdditionalInformations additionalInformationType, Context * context, CalculationStore * store) {
  store->push(input, context, dummyHeight);
  Shared::ExpiringPointer<::Calculation::Calculation> lastCalculation = store->calculationAtIndex(0);
  quiz_assert_print_if_failure(lastCalculation->additionalInformations() == additionalInformationType, input);
  store->deleteAll();
}

QUIZ_CASE(calculation_additional_results) {
  Shared::GlobalContext globalContext;
  CalculationStore store(calculationBuffer,calculationBufferSize);

  Poincare::Preferences::sharedPreferences()->setComplexFormat(Poincare::Preferences::ComplexFormat::Real);
  assertCalculationAdditionalResultTypeHas("1+1", {.integer = true}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("π-π", {.integer = true}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("2/24", {.rational = true, .scientificNotation = true}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("1+i", {.complex = true}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("sin(π)", {.directTrigonometry = true}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("acos(0.5)", {.inverseTrigonometry = true}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("sin(iπ)", {.complex = true}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("ln(2)", {.function = true, .scientificNotation = true}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("2^3", {.integer = true, .function = true}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas(".5^2", {.rational = true, .function = true, .scientificNotation = true}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("e^3", {.function = true, .scientificNotation = true}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("tan(π/2)", {}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("[[1]]", {.vector = true}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("[[1,1]]", {.vector = true}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("[[1][2][3]]", {.vector = true}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("transpose(identity(2))", {.matrix = true}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("[[cos(π/3),-sin(π/3)][sin(π/3),cos(π/3)]]", {.matrix = true}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("345nV", {.unit = true}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("223m^3", {.unit = true}, &globalContext, &store);

  assertCalculationAdditionalResultTypeHas("1/400", {.rational = true, .scientificNotation = true}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("400", {.integer = true, .scientificNotation = true}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("π+π", {}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("e^(2+3)", {.scientificNotation = true}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("2i", {.complex = true}, &globalContext, &store);
  Poincare::Preferences::sharedPreferences()->setDisplayMode(Poincare::Preferences::PrintFloatMode::Scientific);
  assertCalculationAdditionalResultTypeHas("e^(2+3)", {}, &globalContext, &store);
  Poincare::Preferences::sharedPreferences()->setDisplayMode(Poincare::Preferences::PrintFloatMode::Decimal);

  assertCalculationAdditionalResultTypeHas("√(-1)", {}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("{1}", {}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("{i}", {}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("ln(3+4)", {}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("cos(i)", {}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("cos(i%)", {}, &globalContext, &store);
  assertMainCalculationOutputIs("i→z", "i", &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("z+1", {.complex = true}, &globalContext, &store);
  Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("z.exp").destroy();

  Poincare::Preferences::sharedPreferences()->setComplexFormat(Poincare::Preferences::ComplexFormat::Cartesian);
  assertCalculationAdditionalResultTypeHas("√(-1)", {.complex = true}, &globalContext, &store);
}
