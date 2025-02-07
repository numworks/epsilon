#include "../calculation_store.h"

#include <apps/calculation/additional_results/additional_results_type.h>
#include <apps/shared/global_context.h>
#include <assert.h>
#include <poincare/cas.h>
#include <poincare/k_tree.h>
#include <poincare/old/context.h>
#include <poincare/preferences.h>
#include <poincare/test/helper.h>
#include <poincare/test/old/helper.h>
#include <quiz.h>
#include <string.h>

using AdditionalResultsType = Calculation::AdditionalResultsType;
using DisplayOutput = Calculation::Calculation::DisplayOutput;
using EqualSign = Calculation::Calculation::EqualSign;
using OutputLayouts = Calculation::Calculation::OutputLayouts;
using CalculationStore = Calculation::CalculationStore;

using namespace Poincare;

constexpr static size_t calculationBufferSize =
    10 * (sizeof(Calculation::Calculation) +
          Calculation::Calculation::k_numberOfExpressions *
              ::Constant::MaxSerializedExpressionSize +
          sizeof(Calculation::Calculation*));
char calculationBuffer[calculationBufferSize];

void assert_store_is(CalculationStore* store, const char** result) {
  for (int i = 0; i < store->numberOfCalculations(); i++) {
    assert_expression_serializes_to(store->calculationAtIndex(i)->input(),
                                    result[i]);
  }
}

struct CalculationResult {
  Shared::ExpiringPointer<Calculation::Calculation> lastCalculation;
  OutputLayouts layouts;
};

CalculationResult pushAndProcessCalculation(CalculationStore* store,
                                            const char* input,
                                            Context* context) {
  /* These two variables mirror the "font" and "maxVisibleWidth" variables in
   * HistoryViewCell::setNewCalculation */
  constexpr static KDFont::Size font = KDFont::Size::Large;
  constexpr static KDCoordinate maxVisibleWidth = 280;

  store->push(Layout::String(input), context);
  Shared::ExpiringPointer<Calculation::Calculation> lastCalculation =
      store->calculationAtIndex(0);

  OutputLayouts outputLayouts =
      lastCalculation->layoutCalculation(font, maxVisibleWidth, context, true);

  return {lastCalculation, std::move(outputLayouts)};
}

QUIZ_CASE(calculation_store) {
  Shared::GlobalContext globalContext;
  CalculationStore store(calculationBuffer, calculationBufferSize);
  // Store is now {9, 8, 7, 6, 5, 4, 3, 2, 1, 0}
  const char* result[] = {"9", "8", "7", "6", "5", "4", "3", "2", "1", "0"};
  for (int i = 0; i < 10; i++) {
    char text[2] = {(char)(i + '0'), 0};
    pushAndProcessCalculation(&store, text, &globalContext);
    quiz_assert(store.numberOfCalculations() == i + 1);
  }
  assert_store_is(&store, result);

  for (int i = 9; i > 0; i = i - 2) {
    store.deleteCalculationAtIndex(i);
  }
  // Store is now {9, 7, 5, 3, 1}
  const char* result2[] = {"9", "7", "5", "3", "1"};
  assert_store_is(&store, result2);

  store.deleteAll();

  /* Checking if the store handles correctly the delete of older calculations
   * when full. */
  constexpr size_t minimalSize = CalculationStore::k_calculationMinimalSize +
                                 sizeof(Calculation::Calculation*);
  constexpr const char* pattern = "1234567890123456789+";
  constexpr size_t patternSize = 20;
  assert(strlen(pattern) == patternSize);

  // Case 1 : Remaining space < minimalSize
  {
    constexpr size_t calculationSize = 200;
    assert(calculationSize < store.remainingBufferSize());
    static_assert(calculationSize % patternSize == 0);
    char text[calculationSize];
    for (size_t i = 0; i < calculationSize; i += patternSize) {
      memcpy(text + i, pattern, patternSize);
    }
    text[calculationSize - 1] = 0;

    while (store.remainingBufferSize() >= minimalSize) {
      pushAndProcessCalculation(&store, text, &globalContext);
    }
    int numberOfCalculations1 = store.numberOfCalculations();
    /* The buffer is now too full to push a new calculation.
     * Trying to push a new one should delete the oldest one. Alter new text to
     * distinguish it from previously pushed ones. */
    text[0] = '9';
    auto [pushedCalculation, _] =
        pushAndProcessCalculation(&store, text, &globalContext);
    // Assert pushed text is correct
    char buffer[4096];
    store.calculationAtIndex(0)->input().serialize(buffer, std::size(buffer));
    quiz_assert(strcmp(buffer, text) == 0);
    pushedCalculation->input().serialize(buffer, std::size(buffer));
    quiz_assert(strcmp(buffer, text) == 0);
    int numberOfCalculations2 = store.numberOfCalculations();
    // The numberOfCalculations should be the same
    quiz_assert(numberOfCalculations1 == numberOfCalculations2);
  }
  store.deleteAll();

  // Case 2 : Remaining space > minimalSize but pushed calculation doesn't fit
  {
    /* This value was chosen so that :
         - the calculationSize is larger than minimalSize (expression.treeSize)
         - the layout can be serialized (treeSize() < TreeStack/2)
         - the expression cannot have more than 256 children in the addition
    */
    constexpr size_t textSize = 3500;
    assert(textSize % patternSize == 0);
    assert(textSize < store.remainingBufferSize());
    char text[textSize];
    for (size_t i = 0; i < textSize; i += patternSize) {
      memcpy(text + i, pattern, patternSize);
    }
    text[textSize - 1] = 0;

    const size_t emptyStoreSize = store.remainingBufferSize();
    pushAndProcessCalculation(&store, text, &globalContext);
    assert(emptyStoreSize > store.remainingBufferSize());
    const size_t calculationSize = emptyStoreSize - store.remainingBufferSize();

    // Push big calculations until approaching the limit
    while (store.remainingBufferSize() > 2 * calculationSize) {
      pushAndProcessCalculation(&store, text, &globalContext);
    }
    /* Push small calculations so that remainingBufferSize remain bigger, but
     * gets closer to minimalSize */
    while (store.remainingBufferSize() > minimalSize + minimalSize / 4) {
      pushAndProcessCalculation(&store, "1", &globalContext);
    }
    assert(store.remainingBufferSize() < calculationSize);
    assert(store.remainingBufferSize() > minimalSize);
    int numberOfCalculations1 = store.numberOfCalculations();
    /* The buffer is now too full to push a new calculation.
     * Trying to push a new one should delete older ones. Alter new text to
     * distinguish it from previously pushed ones. */
    text[0] = '9';
    auto [pushedCalculation, _] =
        pushAndProcessCalculation(&store, text, &globalContext);
    char buffer[8192];
    store.calculationAtIndex(0)->input().serialize(buffer, std::size(buffer));
    quiz_assert(strcmp(buffer, text) == 0);
    pushedCalculation->input().serialize(buffer, std::size(buffer));
    quiz_assert(strcmp(buffer, text) == 0);
    int numberOfCalculations2 = store.numberOfCalculations();
    // The numberOfCalculations should be the equal or smaller
    quiz_assert(numberOfCalculations1 >= numberOfCalculations2);
  }
  store.deleteAll();
  quiz_assert(store.remainingBufferSize() == store.bufferSize());
}

void assertAnsIs(const char* input, const char* expectedAnsInputText,
                 Context* context, CalculationStore* store) {
  pushAndProcessCalculation(store, input, context);
  Shared::ExpiringPointer<Calculation::Calculation> lastCalculation =
      store->calculationAtIndex(0);
  pushAndProcessCalculation(store, "Ans", context);
  lastCalculation = store->calculationAtIndex(0);
  assert_expression_serializes_to(lastCalculation->input(),
                                  expectedAnsInputText);
}

QUIZ_CASE(calculation_ans) {
  Shared::GlobalContext globalContext;
  CalculationStore store(calculationBuffer, calculationBufferSize);
  // Setup complex format and exam mode
  Preferences::ComplexFormat previousComplexFormat =
      Preferences::SharedPreferences()->complexFormat();
  ExamMode previousExamMode = Preferences::SharedPreferences()->examMode();
  Preferences::SharedPreferences()->setComplexFormat(
      Preferences::ComplexFormat::Real);
  Preferences::SharedPreferences()->setExamMode(
      ExamMode(ExamMode::Ruleset::Off));

  pushAndProcessCalculation(&store, "1+3/4", &globalContext);
  pushAndProcessCalculation(&store, "ans+2/3", &globalContext);
  Shared::ExpiringPointer<Calculation::Calculation> lastCalculation =
      store.calculationAtIndex(0);
  quiz_assert(lastCalculation->displayOutput() ==
              DisplayOutput::ExactAndApproximate);
  assert_expression_serializes_to(lastCalculation->exactOutput(), "29/12");

  pushAndProcessCalculation(&store, "ans+0.22", &globalContext);
  lastCalculation = store.calculationAtIndex(0);
  quiz_assert(lastCalculation->displayOutput() ==
              DisplayOutput::ExactAndApproximateToggle);
  assert_expression_serializes_to(lastCalculation->approximateOutput(),
                                  "2.6366666666667");

  assertAnsIs("1+1→a", "2", &globalContext, &store);
  assertAnsIs("0^0→a", "0^0", &globalContext, &store);
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("a.exp").destroy();

  assertAnsIs("1+1", "2", &globalContext, &store);
  assertAnsIs("13.3", "13.3", &globalContext, &store);
  assertAnsIs("√(-1-1)", "√(-1-1)", &globalContext, &store);
  assertAnsIs("int(diff(x^2,x,x),x,0,1)", "int(diff(x^2,x,x),x,0,1)",
              &globalContext, &store);
  assertAnsIs("int(diff(x^2.1,x,x),x,0,1)", "int(diff(x^2.1,x,x),x,0,1)",
              &globalContext, &store);
  assertAnsIs("int(diff(x^2,x,x),x,0,1)→a", "1", &globalContext, &store);

  assertAnsIs("√(1+1)", "√(2)", &globalContext, &store);

  Preferences::SharedPreferences()->setExamMode(
      ExamMode(ExamMode::Ruleset::Dutch));
  assert(CAS::ShouldOnlyDisplayApproximation(
      UserExpression::Builder(KSqrt(2_e)), UserExpression::Builder(KSqrt(2_e)),
      Expression(), nullptr));

  assertAnsIs("√(1+1)", "√(1+1)", &globalContext, &store);

  // Restore complex format and exam mode
  Preferences::SharedPreferences()->setExamMode(previousExamMode);
  Preferences::SharedPreferences()->setComplexFormat(previousComplexFormat);

  pushAndProcessCalculation(&store, "_g0", &globalContext);
  pushAndProcessCalculation(&store, "ans→m*s^-2", &globalContext);
  lastCalculation = store.calculationAtIndex(0);
  assert_expression_serializes_to(lastCalculation->exactOutput(),
                                  "9.80665×_m×_s^\U00000012-2\U00000013");

  store.deleteAll();
}

void assertCalculationIs(const char* input, DisplayOutput expectedDisplay,
                         EqualSign expectedSign,
                         const char* expectedExactOutput,
                         const char* expectedApproximateOutput,
                         Context* context, CalculationStore* store,
                         const char* expectedStoredInput = nullptr) {
  auto [lastCalculation, outputLayouts] =
      pushAndProcessCalculation(store, input, context);

#if POINCARE_STRICT_TESTS
  quiz_assert(displayOutput == expectedDisplay);
#else
  quiz_tolerate_print_if_failure(
      lastCalculation->displayOutput() == expectedDisplay, input,
      "correct displayOutput", "incorrect displayOutput");
#endif

#if POINCARE_STRICT_TESTS
  quiz_assert(equalSign == expectedSign);
#else
  quiz_tolerate_print_if_failure(lastCalculation->equalSign() == expectedSign,
                                 input, "correct equalSign",
                                 "incorrect equalSign");
#endif

  if (expectedStoredInput) {
    assert_expression_serializes_to(lastCalculation->input(),
                                    expectedStoredInput);
  }
  if (expectedExactOutput) {
    quiz_assert(Calculation::Calculation::CanDisplayExact(
        lastCalculation->displayOutput()));
    assert_layout_serializes_to(outputLayouts.exact, expectedExactOutput);
  }

  if (expectedApproximateOutput) {
    assert(Calculation::Calculation::CanDisplayApproximate(
        lastCalculation->displayOutput()));
    assert_layout_serializes_to(outputLayouts.approximate,
                                expectedApproximateOutput);
  }
  store->deleteAll();
}

QUIZ_CASE(calculation_significant_digits) {
  Shared::GlobalContext globalContext;
  CalculationStore store(calculationBuffer, calculationBufferSize);

  assertCalculationIs("123456789123456789",
                      DisplayOutput::ExactAndApproximateToggle,
                      EqualSign::Approximation, "123456789123456789",
                      "1.234567891ᴇ17", &globalContext, &store);
  // FIXME: sign is wrong (returns Equal)
  assertCalculationIs("123123456789", DisplayOutput::ExactAndApproximateToggle,
                      EqualSign::Approximation, "123123456789",
                      "1.231234568ᴇ11", &globalContext, &store);
  // FIXME: sign is wrong (returns Equal)
  assertCalculationIs("11123456789", DisplayOutput::ExactAndApproximateToggle,
                      EqualSign::Approximation, "11123456789", "1.112345679ᴇ10",
                      &globalContext, &store);
  assertCalculationIs(
      "1123456789", DisplayOutput::ApproximateIsIdenticalToExact,
      EqualSign::Undefined, nullptr, "1123456789", &globalContext, &store);
  assertCalculationIs("123456789", DisplayOutput::ApproximateIsIdenticalToExact,
                      EqualSign::Undefined, nullptr, "123456789",
                      &globalContext, &store);
}

QUIZ_CASE(calculation_display_exact_approximate) {
  Shared::GlobalContext globalContext;
  CalculationStore store(calculationBuffer, calculationBufferSize);

  Preferences::AngleUnit previousAngleUnit =
      Preferences::SharedPreferences()->angleUnit();
  Preferences::SharedPreferences()->setAngleUnit(
      Preferences::AngleUnit::Degree);

  uint8_t previousNumberOfSignificantDigits =
      Preferences::SharedPreferences()->numberOfSignificantDigits();
  Preferences::SharedPreferences()->setNumberOfSignificantDigits(
      PrintFloat::k_maxNumberOfSignificantDigits);

  assertCalculationIs("1/2", DisplayOutput::ExactAndApproximateToggle,
                      EqualSign::Equal, "1/2", "0.5", &globalContext, &store);
  assertCalculationIs("1/3", DisplayOutput::ExactAndApproximateToggle,
                      EqualSign::Approximation, "1/3", "0.33333333333333",
                      &globalContext, &store);

  assertCalculationIs("1/(2i)", DisplayOutput::ExactAndApproximate,
                      EqualSign::Equal, "-(1/2)i", "-0.5i", &globalContext,
                      &store);
  assertCalculationIs("1/0", DisplayOutput::ExactOnly, EqualSign::Undefined,
                      "undef", nullptr, &globalContext, &store);
  assertCalculationIs("2x-x", DisplayOutput::ExactOnly, EqualSign::Undefined,
                      "undef", nullptr, &globalContext, &store);
  assertCalculationIs("[[1,2,3]]", DisplayOutput::ApproximateIsIdenticalToExact,
                      EqualSign::Undefined, nullptr, "[[1,2,3]]",
                      &globalContext, &store);
  assertCalculationIs("[[1,x,3]]", DisplayOutput::ApproximateIsIdenticalToExact,
                      EqualSign::Undefined, nullptr, "[[1,undef,3]]",
                      &globalContext, &store);
  assertCalculationIs(
      "[[1/0,2/0]]", DisplayOutput::ApproximateIsIdenticalToExact,
      EqualSign::Undefined, nullptr, "[[undef,undef]]", &globalContext, &store);
  assertCalculationIs("{1/0,2/0}", DisplayOutput::ApproximateIsIdenticalToExact,
                      EqualSign::Undefined, nullptr, "{undef,undef}",
                      &globalContext, &store);
  assertCalculationIs("28^7", DisplayOutput::ApproximateIsIdenticalToExact,
                      EqualSign::Undefined, nullptr, "13492928512",
                      &globalContext, &store);
  assertCalculationIs("3+√(2)→a", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "3+√(2)", "4.4142135623731",
                      &globalContext, &store);
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("a.exp").destroy();
  assertCalculationIs("3+2→a", DisplayOutput::ApproximateIsIdenticalToExact,
                      EqualSign::Undefined, nullptr, "5", &globalContext,
                      &store);
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("a.exp").destroy();
  assertCalculationIs("3→a", DisplayOutput::ApproximateIsIdenticalToExact,
                      EqualSign::Undefined, nullptr, "3", &globalContext,
                      &store);
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("a.exp").destroy();
  assertCalculationIs("3/2→a", DisplayOutput::ExactAndApproximate,
                      EqualSign::Equal, "3/2", "1.5", &globalContext, &store);
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("a.exp").destroy();
  assertCalculationIs("3+x→f(x)", DisplayOutput::ExactOnly,
                      EqualSign::Undefined, "3+x", nullptr, &globalContext,
                      &store);
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("f.func").destroy();
  assertCalculationIs("1+1+random()", DisplayOutput::ApproximateOnly,
                      EqualSign::Undefined, nullptr, nullptr, &globalContext,
                      &store);
  assertCalculationIs("1+1+round(1.343,2)", DisplayOutput::ApproximateOnly,
                      EqualSign::Undefined, nullptr, "3.34", &globalContext,
                      &store);
  assertCalculationIs("randint(2,2)+3", DisplayOutput::ApproximateOnly,
                      EqualSign::Undefined, nullptr, "5", &globalContext,
                      &store);
  assertCalculationIs("√(8)", DisplayOutput::ExactAndApproximateToggle,
                      EqualSign::Approximation, "√(8)", "2.8284271247462",
                      &globalContext, &store);
  assertCalculationIs("cos(45×_°)", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "(√(2))/2", "0.70710678118655",
                      &globalContext, &store);
  assertCalculationIs("cos(π/4×_rad)", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "(√(2))/2", "0.70710678118655",
                      &globalContext, &store);
  assertCalculationIs("cos(50×_°)", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "cos(50)", "0.64278760968654",
                      &globalContext, &store);
  assertCalculationIs("binompdf(2,3,0.5)", DisplayOutput::ApproximateOnly,
                      EqualSign::Undefined, nullptr, "0.375", &globalContext,
                      &store);
  assertCalculationIs("1+2%", DisplayOutput::ExactAndApproximateToggle,
                      EqualSign::Equal, "1(1+(2/100))", "1.02", &globalContext,
                      &store);
  assertCalculationIs("1-(1/3)%", DisplayOutput::ExactAndApproximateToggle,
                      EqualSign::Approximation, "1×(1-((1/3)/100))",
                      "0.99666666666667", &globalContext, &store);

  // Exact output that have dependencies are not displayed
  assertCalculationIs("2→f(x)", DisplayOutput::ExactOnly, EqualSign::Undefined,
                      "2", nullptr, &globalContext, &store);
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("a.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("f.func").destroy();

  assertCalculationIs("(1/6)_g", DisplayOutput::ApproximateOnly,
                      EqualSign::Undefined, nullptr, "0.16666666666667g",
                      &globalContext, &store);
  assertCalculationIs("(1/6)_L_kg", DisplayOutput::ApproximateOnly,
                      EqualSign::Undefined, nullptr, "1.6666666666667ᴇ-4m^3·kg",
                      &globalContext, &store);
  assertCalculationIs("(π/6)_rad", DisplayOutput::ApproximateOnly,
                      EqualSign::Undefined, nullptr, "30°", &globalContext,
                      &store);
  assertCalculationIs("(1/11)_°", DisplayOutput::ApproximateOnly,
                      EqualSign::Undefined, nullptr, "0.090909090909091°",
                      &globalContext, &store);
  assertCalculationIs("180→rad", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "πrad", "3.1415926535898rad",
                      &globalContext, &store);
  assertCalculationIs("45→gon", DisplayOutput::ApproximateOnly,
                      EqualSign::Undefined, nullptr, "50gon", &globalContext,
                      &store);
  Preferences::SharedPreferences()->setAngleUnit(
      Preferences::AngleUnit::Radian);
  assertCalculationIs("2+π→_rad", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "(2+π)rad",
                      "5.1415926535898rad", &globalContext, &store);
  assertCalculationIs("π/2→°", DisplayOutput::ApproximateOnly,
                      EqualSign::Undefined, nullptr, "90°", &globalContext,
                      &store);
  assertCalculationIs("πrad/2→°", DisplayOutput::ApproximateOnly,
                      EqualSign::Undefined, nullptr, "90°", &globalContext,
                      &store);
  assertCalculationIs("(1/6)_rad^(-1)", DisplayOutput::ApproximateOnly,
                      EqualSign::Undefined, nullptr, "0.16666666666667rad^(-1)",
                      &globalContext, &store);
  assertCalculationIs("diff(x^2,x,3)_rad", DisplayOutput::ApproximateOnly,
                      EqualSign::Undefined, nullptr, "6rad", &globalContext,
                      &store);
  assertCalculationIs("(1/6)_rad→a", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "(1/6)rad",
                      "0.16666666666667rad", &globalContext, &store);
  assertCalculationIs("diff(x^2,x,3)_rad→a", DisplayOutput::ApproximateOnly,
                      EqualSign::Undefined, nullptr, "6rad", &globalContext,
                      &store);
  Preferences::SharedPreferences()->setAngleUnit(
      Preferences::AngleUnit::Degree);
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("a.exp").destroy();

  ExamMode previousExamMode = Preferences::SharedPreferences()->examMode();
  Preferences::SharedPreferences()->setExamMode(
      ExamMode(ExamMode::Ruleset::Dutch));

  assertCalculationIs("1+1", DisplayOutput::ApproximateIsIdenticalToExact,
                      EqualSign::Undefined, nullptr, "2", &globalContext,
                      &store);
  assertCalculationIs("1/2", DisplayOutput::ExactAndApproximateToggle,
                      EqualSign::Equal, "1/2", "0.5", &globalContext, &store);
  assertCalculationIs("0.5", DisplayOutput::ExactAndApproximateToggle,
                      EqualSign::Equal, "1/2", "0.5", &globalContext, &store);
  assertCalculationIs("√(8)", DisplayOutput::ApproximateOnly,
                      EqualSign::Undefined, nullptr, "2.8284271247462",
                      &globalContext, &store);
  assertCalculationIs("cos(45×°)", DisplayOutput::ApproximateOnly,
                      EqualSign::Undefined, nullptr, "0.70710678118655",
                      &globalContext, &store);
  assertCalculationIs("cos(π/4×_rad)", DisplayOutput::ApproximateOnly,
                      EqualSign::Undefined, nullptr, "0.70710678118655",
                      &globalContext, &store);
  assertCalculationIs("_G", DisplayOutput::ExactOnly, EqualSign::Undefined,
                      nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("_g0", DisplayOutput::ExactOnly, EqualSign::Undefined,
                      nullptr, nullptr, &globalContext, &store);

  Preferences::SharedPreferences()->setExamMode(previousExamMode);
  Preferences::SharedPreferences()->setAngleUnit(previousAngleUnit);

  Preferences::SharedPreferences()->setNumberOfSignificantDigits(
      previousNumberOfSignificantDigits);
}

void assertMainCalculationOutputIs(const char* input, const char* output,
                                   Context* context, CalculationStore* store) {
  // For the next test, we only need to checkout input and output text.
  pushAndProcessCalculation(store, input, context);
  Shared::ExpiringPointer<Calculation::Calculation> lastCalculation =
      store->calculationAtIndex(0);
  switch (lastCalculation->displayOutput()) {
    case DisplayOutput::ApproximateOnly:
      assert_expression_serializes_to(lastCalculation->approximateOutput(),
                                      output);
      break;
    default:
      assert_expression_serializes_to(lastCalculation->exactOutput(), output);
      break;
  }
  store->deleteAll();
}

QUIZ_CASE(calculation_symbolic_computation) {
  Shared::GlobalContext globalContext;
  CalculationStore store(calculationBuffer, calculationBufferSize);

  // 0 - General cases
  assertMainCalculationOutputIs("x+x+1+3+√(π)", "undef", &globalContext,
                                &store);
  assertMainCalculationOutputIs("f(x)", "undef", &globalContext, &store);
  assertMainCalculationOutputIs("1+x→f(x)", "1+x", &globalContext, &store);
  assertMainCalculationOutputIs("f(x)", "undef", &globalContext, &store);
  assertMainCalculationOutputIs("f(2)", "3", &globalContext, &store);
  assertMainCalculationOutputIs("2→x", "2", &globalContext, &store);
  assertMainCalculationOutputIs("f(x)", "3", &globalContext, &store);
  assertMainCalculationOutputIs("x+x+1+3+√(π)", "8+√(π)", &globalContext,
                                &store);

  // Destroy records
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("f.func").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("x.exp").destroy();

  // Derivatives
  assertCalculationIs("foo'(1)", DisplayOutput::ExactOnly, EqualSign::Undefined,
                      "undef", nullptr, &globalContext, &store, "f×o×o×_'×(1)");
  assertCalculationIs("foo\"(1)", DisplayOutput::ExactOnly,
                      EqualSign::Undefined, "undef", nullptr, &globalContext,
                      &store, "f×o×o×_\"×(1)");
  assertCalculationIs("foo^(3)(1)", DisplayOutput::ExactOnly,
                      EqualSign::Undefined, "undef", nullptr, &globalContext,
                      &store, "f×o×o^(3)×(1)");
  assertMainCalculationOutputIs("x^4→foo(x)", "x^4", &globalContext, &store);
  assertCalculationIs("foo'(1)", DisplayOutput::ApproximateOnly,
                      EqualSign::Undefined, nullptr, "4", &globalContext,
                      &store, "foo'(1)");
  assertCalculationIs("foo^(1)(1)", DisplayOutput::ApproximateOnly,
                      EqualSign::Undefined, nullptr, "4", &globalContext,
                      &store, "foo'(1)");
  assertCalculationIs("foo\"(1)", DisplayOutput::ApproximateOnly,
                      EqualSign::Undefined, nullptr, "12", &globalContext,
                      &store, "foo\"(1)");
  assertCalculationIs("foo^(2)(1)", DisplayOutput::ApproximateOnly,
                      EqualSign::Undefined, nullptr, "12", &globalContext,
                      &store, "foo\"(1)");
  assertCalculationIs("foo^(3)(1)", DisplayOutput::ApproximateOnly,
                      EqualSign::Undefined, nullptr, "24", &globalContext,
                      &store, "foo^(3)(1)");
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("foo.func").destroy();

  // 1 - Predefined variable in fraction in integral
  assertMainCalculationOutputIs("int(x+1/x,x,1,2)", "2.193147181",
                                &globalContext, &store);
  assertMainCalculationOutputIs("1→x", "1", &globalContext, &store);
  assertMainCalculationOutputIs("int(x+1/x,x,1,2)", "2.193147181",
                                &globalContext, &store);
  // Destroy records
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("x.exp").destroy();

  /* 2 - Circularly defined functions
   *   A - f(x) = g(x) = f(x) */
  assertMainCalculationOutputIs("1→f(x)", "1", &globalContext, &store);
  assertMainCalculationOutputIs("f(x)→g(x)", "f(x)", &globalContext, &store);
  assertMainCalculationOutputIs("g(x)→f(x)", "g(x)", &globalContext, &store);
  // With x undefined
  assertMainCalculationOutputIs("f(x)", "undef", &globalContext, &store);
  assertMainCalculationOutputIs("diff(f(x),x,1)", "undef", &globalContext,
                                &store);
  // With x  defined
  assertMainCalculationOutputIs("1→x", "1", &globalContext, &store);

  assertMainCalculationOutputIs("f(x)", "undef", &globalContext, &store);
  assertMainCalculationOutputIs("diff(f(x),x,1)", "undef", &globalContext,
                                &store);
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("x.exp").destroy();
  //   B - f(x) = g(x) = a = f(x)
  assertMainCalculationOutputIs("f(x)→a", "undef", &globalContext, &store);
  assertMainCalculationOutputIs("a→g(x)", "a", &globalContext, &store);
  // With x undefined
  assertMainCalculationOutputIs("f(x)", "undef", &globalContext, &store);
  assertMainCalculationOutputIs("diff(f(x),x,1)", "undef", &globalContext,
                                &store);
  // With x defined
  assertMainCalculationOutputIs("1→x", "1", &globalContext, &store);

  assertMainCalculationOutputIs("f(x)", "undef", &globalContext, &store);
  assertMainCalculationOutputIs("diff(f(x),x,1)", "undef", &globalContext,
                                &store);
  //   C - g(x) = f(f(..))
  assertMainCalculationOutputIs("x+5→f(x)", "x+5", &globalContext, &store);
  assertMainCalculationOutputIs("f(f(1-f(x)))→g(x)", "f(f(1-f(x)))",
                                &globalContext, &store);
  assertMainCalculationOutputIs("g(4)", "2", &globalContext, &store);

  // Destroy records
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("x.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("a.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("f.func").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("g.func").destroy();

  // 3 - Differences between functions and variables
  assertMainCalculationOutputIs("x+1→f(x)", "x+1", &globalContext, &store);
  assertMainCalculationOutputIs("x+1→y", "undef", &globalContext, &store);
  // With x undefined
  assertMainCalculationOutputIs("y", "undef", &globalContext, &store);

  assertMainCalculationOutputIs("int(y,x,1,3)", "undef", &globalContext,
                                &store);
  assertMainCalculationOutputIs("sum(y,x,0,1)", "undef", &globalContext,
                                &store);
  assertMainCalculationOutputIs("product(y,x,0,1)", "undef", &globalContext,
                                &store);
  assertMainCalculationOutputIs("diff(y,x,1)", "undef", &globalContext, &store);

  assertMainCalculationOutputIs("f(y)", "undef", &globalContext, &store);
  assertMainCalculationOutputIs("diff(f(y),x,1)", "undef", &globalContext,
                                &store);
  assertMainCalculationOutputIs("diff(f(x)×y,x,1)", "undef", &globalContext,
                                &store);
  assertMainCalculationOutputIs("diff(f(x×y),x,1)", "undef", &globalContext,
                                &store);

  // With x defined
  assertMainCalculationOutputIs("1→x", "1", &globalContext, &store);
  assertMainCalculationOutputIs("y", "undef", &globalContext, &store);
  assertMainCalculationOutputIs("x+1→y", "2", &globalContext, &store);
  assertMainCalculationOutputIs("y", "2", &globalContext, &store);

  assertMainCalculationOutputIs("int(x+1,x,1,3)", "6", &globalContext, &store);
  assertMainCalculationOutputIs("int(f(x),x,1,3)", "6", &globalContext, &store);
  assertMainCalculationOutputIs("int(y,x,1,3)", "4", &globalContext, &store);

  assertMainCalculationOutputIs("sum(x+1,x,0,1)", "3", &globalContext, &store);
  assertMainCalculationOutputIs("sum(f(x),x,0,1)", "3", &globalContext, &store);
  assertMainCalculationOutputIs("sum(y,x,0,1)", "4", &globalContext, &store);

  assertMainCalculationOutputIs("product(x+1,x,0,1)", "2", &globalContext,
                                &store);
  assertMainCalculationOutputIs("product(f(x),x,0,1)", "2", &globalContext,
                                &store);
  assertMainCalculationOutputIs("product(y,x,0,1)", "4", &globalContext,
                                &store);

  assertMainCalculationOutputIs("diff(x+1,x,1)", "1", &globalContext, &store);
  assertMainCalculationOutputIs("diff(f(x),x,1)", "1", &globalContext, &store);
  assertMainCalculationOutputIs("diff(y,x,1)", "0", &globalContext, &store);

  assertMainCalculationOutputIs("f(y)", "3", &globalContext, &store);
  assertMainCalculationOutputIs("diff(f(y),x,1)", "0", &globalContext, &store);
  assertMainCalculationOutputIs("diff(f(x)×y,x,1)", "2", &globalContext,
                                &store);
  assertMainCalculationOutputIs("diff(f(x×y),x,1)", "2", &globalContext,
                                &store);
  // Destroy records
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("x.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("y.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("f.func").destroy();

  // 4 - Nested local variables within variables
  assertMainCalculationOutputIs("int(x+1,x,1,3)→a", "6", &globalContext,
                                &store);
  assertMainCalculationOutputIs("a", "6", &globalContext, &store);
  assertMainCalculationOutputIs("a+1→a", "7", &globalContext, &store);
  assertMainCalculationOutputIs("diff(y×a,y,1)", "7", &globalContext, &store);

  // Destroy records
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("y.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("a.exp").destroy();

  assertMainCalculationOutputIs("2→x", "2", &globalContext, &store);
  assertMainCalculationOutputIs("diff(x,x,x)→a", "1", &globalContext, &store);
  assertMainCalculationOutputIs("diff(a,x,3)", "0", &globalContext, &store);
  // Destroy records
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("a.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("x.exp").destroy();

  // 5 - Double nested local variables within variables
  assertMainCalculationOutputIs("1→x", "1", &globalContext, &store);
  assertMainCalculationOutputIs("x→a", "1", &globalContext, &store);
  assertMainCalculationOutputIs("diff(a,x,x)→b", "0", &globalContext, &store);
  assertMainCalculationOutputIs("b", "0", &globalContext, &store);
  // Destroy records
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("b.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("a.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("x.exp").destroy();

  // 6 - Define function from their own expression
  assertMainCalculationOutputIs("x+1→f(x)", "x+1", &globalContext, &store);
  assertMainCalculationOutputIs("f(x^2)→f(x)", "x^2+1", &globalContext, &store);
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("f.func").destroy();

  // 7 - Circularly defined functions with exponential expression size growth
  assertMainCalculationOutputIs("x→f(x)", "x", &globalContext, &store);
  assertMainCalculationOutputIs("f(xx)→g(x)", "f(x×x)", &globalContext, &store);
  assertMainCalculationOutputIs("g(xx)→f(x)", "g(x×x)", &globalContext, &store);
  assertMainCalculationOutputIs("g(2)", "undef", &globalContext, &store);
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("f.func").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("g.func").destroy();

  // 8 - Circularly with symbols
  pushAndProcessCalculation(&store, "x→f(x)", &globalContext);
  assertMainCalculationOutputIs("f(Ans)→A", "undef", &globalContext, &store);
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("f.func").destroy();

  // Derivatives condensed form
  pushAndProcessCalculation(&store, "2→c(x)", &globalContext);
  assertMainCalculationOutputIs("c''(0)→c(x)", "diff(2,x,0,2)", &globalContext,
                                &store);
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("c.func").destroy();
}

QUIZ_CASE(calculation_symbolic_computation_and_parametered_expressions) {
  Shared::GlobalContext globalContext;
  CalculationStore store(calculationBuffer, calculationBufferSize);

  // Tests a bug with symbolic computation
  assertCalculationIs("int((e^(-x))-x^(0.5), x, 0, 3)",
                      DisplayOutput::ApproximateOnly, EqualSign::Undefined,
                      nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("int(x,x,0,2)", DisplayOutput::ApproximateOnly,
                      EqualSign::Undefined, nullptr, "2", &globalContext,
                      &store);
  assertCalculationIs("sum(x,x,0,2)", DisplayOutput::ApproximateOnly,
                      EqualSign::Undefined, nullptr, "3", &globalContext,
                      &store);
  assertCalculationIs("product(x,x,1,2)", DisplayOutput::ApproximateOnly,
                      EqualSign::Undefined, nullptr, "2", &globalContext,
                      &store);
  assertCalculationIs("diff(x^2,x,3)", DisplayOutput::ApproximateOnly,
                      EqualSign::Undefined, nullptr, "6", &globalContext,
                      &store);
  assertCalculationIs("2→x", DisplayOutput::ApproximateIsIdenticalToExact,
                      EqualSign::Undefined, nullptr, nullptr, &globalContext,
                      &store);
  assertCalculationIs("int(x,x,0,2)", DisplayOutput::ApproximateOnly,
                      EqualSign::Undefined, nullptr, "2", &globalContext,
                      &store);
  assertCalculationIs("sum(x,x,0,2)", DisplayOutput::ApproximateOnly,
                      EqualSign::Undefined, nullptr, "3", &globalContext,
                      &store);
  assertCalculationIs("product(x,x,1,2)", DisplayOutput::ApproximateOnly,
                      EqualSign::Undefined, nullptr, "2", &globalContext,
                      &store);
  assertCalculationIs("diff(x^2,x,3)", DisplayOutput::ApproximateOnly,
                      EqualSign::Undefined, nullptr, "6", &globalContext,
                      &store);

  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("x.exp").destroy();
}

QUIZ_CASE(calculation_complex_format) {
  Shared::GlobalContext globalContext;
  CalculationStore store(calculationBuffer, calculationBufferSize);

  uint8_t previousNumberOfSignificantDigits =
      Preferences::SharedPreferences()->numberOfSignificantDigits();
  Preferences::SharedPreferences()->setNumberOfSignificantDigits(
      PrintFloat::k_maxNumberOfSignificantDigits);

  Preferences::SharedPreferences()->setComplexFormat(
      Preferences::ComplexFormat::Real);
  assertCalculationIs("1+i", DisplayOutput::ApproximateIsIdenticalToExact,
                      EqualSign::Undefined, nullptr, "1+i", &globalContext,
                      &store);
  assertCalculationIs("√(-1)", DisplayOutput::ExactOnly, EqualSign::Undefined,
                      "nonreal", nullptr, &globalContext, &store);
  assertCalculationIs("ln(-2)", DisplayOutput::ExactOnly, EqualSign::Undefined,
                      "nonreal", nullptr, &globalContext, &store);
  assertCalculationIs("√(-1)×√(-1)", DisplayOutput::ExactOnly,
                      EqualSign::Undefined, "nonreal", nullptr, &globalContext,
                      &store);
  assertCalculationIs(
      "(-8)^(1/3)", DisplayOutput::ApproximateIsIdenticalToExact,
      EqualSign::Undefined, nullptr, "-2", &globalContext, &store);
  assertCalculationIs(
      "(-8)^(2/3)", DisplayOutput::ApproximateIsIdenticalToExact,
      EqualSign::Undefined, nullptr, "4", &globalContext, &store);
  assertCalculationIs("(-2)^(1/4)", DisplayOutput::ExactOnly,
                      EqualSign::Undefined, "nonreal", nullptr, &globalContext,
                      &store);

  Preferences::SharedPreferences()->setComplexFormat(
      Preferences::ComplexFormat::Cartesian);
  assertCalculationIs("1+i", DisplayOutput::ApproximateIsIdenticalToExact,
                      EqualSign::Undefined, nullptr, "1+i", &globalContext,
                      &store);
  assertCalculationIs("√(-1)", DisplayOutput::ApproximateIsIdenticalToExact,
                      EqualSign::Undefined, nullptr, "i", &globalContext,
                      &store);
  assertCalculationIs(
      "ln(-2)", DisplayOutput::ExactAndApproximate, EqualSign::Approximation,
      "ln(2)+π·i", "0.69314718055995+3.1415926535898i", &globalContext, &store);
  assertCalculationIs(
      "√(-1)×√(-1)", DisplayOutput::ApproximateIsIdenticalToExact,
      EqualSign::Undefined, nullptr, "-1", &globalContext, &store);
  assertCalculationIs("(-8)^(1/3)", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "1+√(3)i", "1+1.7320508075689i",
                      &globalContext, &store);
  assertCalculationIs("(-8)^(2/3)", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "-2+2×√(3)×i",
                      "-2+3.4641016151378i", &globalContext, &store);
  assertCalculationIs("(-2)^(1/4)", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "root(8,4)/2+root(8,4)/2×i",
                      "0.84089641525371+0.84089641525371i", &globalContext,
                      &store);

  Preferences::SharedPreferences()->setComplexFormat(
      Preferences::ComplexFormat::Polar);
  assertCalculationIs("1+i", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "√(2)e^((π/4)i)",
                      "1.4142135623731e^(0.78539816339745i)", &globalContext,
                      &store);
  assertCalculationIs("√(-1)", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "e^((π/2)i)",
                      "e^(1.5707963267949i)", &globalContext, &store);
  assertCalculationIs("ln(-2)", DisplayOutput::ExactAndApproximateToggle,
                      EqualSign::Approximation, "ln(-2)",
                      "3.2171505117118e^(1.3536398454434i)", &globalContext,
                      &store);
  assertCalculationIs("√(-1)×√(-1)", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "e^(π·i)",
                      "e^(3.1415926535898i)", &globalContext, &store);
  assertCalculationIs("(-8)^(1/3)", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "2e^((π/3)i)",
                      "2e^(1.0471975511966i)", &globalContext, &store);
  assertCalculationIs("(-8)^(2/3)", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "4e^(((2π)/3)i)",
                      "4e^(2.0943951023932i)", &globalContext, &store);
  assertCalculationIs("(-2)^(1/4)", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "root(2,4)e^((π/4)i)",
                      "1.1892071150027e^(0.78539816339745i)", &globalContext,
                      &store);

  Preferences::SharedPreferences()->setComplexFormat(
      Preferences::ComplexFormat::Cartesian);

  Preferences::SharedPreferences()->setNumberOfSignificantDigits(
      previousNumberOfSignificantDigits);
}

QUIZ_CASE(calculation_involving_sequence) {
  Shared::GlobalContext globalContext;

  Shared::SequenceStore* seqStore = globalContext.s_sequenceStore;
  Ion::Storage::Record::ErrorStatus err = seqStore->addEmptyModel();
  assert(err == Ion::Storage::Record::ErrorStatus::None);
  Ion::Storage::Record record =
      seqStore->recordAtIndex(seqStore->numberOfModels() - 1);
  Shared::Sequence* u = seqStore->modelForRecord(record);
  u->setType(Shared::Sequence::Type::Explicit);
  err = u->setContent(Layout::String("i"), &globalContext);
  assert(err == Ion::Storage::Record::ErrorStatus::None);
  (void)err;  // Silence compilation warning.

  CalculationStore calcStore(calculationBuffer, calculationBufferSize);

  assertMainCalculationOutputIs("√(i×u(0))×√(6)", "undef", &globalContext,
                                &calcStore);
  seqStore->removeAll();
  seqStore->tidyDownstreamPoolFrom();
}

bool operator==(const AdditionalResultsType& a,
                const AdditionalResultsType& b) {
  // TODO C++20 Use a default comparison operator
  return a.integer == b.integer && a.rational == b.rational &&
         a.directTrigonometry == b.directTrigonometry &&
         a.inverseTrigonometry == b.inverseTrigonometry && a.unit == b.unit &&
         a.matrix == b.matrix && a.vector == b.vector &&
         a.complex == b.complex && a.function == b.function &&
         a.scientificNotation == b.scientificNotation;
}

void assertCalculationAdditionalResultTypeHas(
    const char* input, const AdditionalResultsType additionalResultsType,
    Context* context, CalculationStore* store) {
  pushAndProcessCalculation(store, input, context);
  Shared::ExpiringPointer<Calculation::Calculation> lastCalculation =
      store->calculationAtIndex(0);
#if POINCARE_STRICT_TESTS
  quiz_assert(lastCalculation->additionalResultsType(context) ==
              additionalResultsType);
#else
  quiz_tolerate_print_if_failure(
      lastCalculation->additionalResultsType(context) == additionalResultsType,
      input, "correct additional results", "incorrect additional results");
#endif
  store->deleteAll();
}

QUIZ_CASE(calculation_additional_results) {
  Shared::GlobalContext globalContext;
  CalculationStore store(calculationBuffer, calculationBufferSize);

  Preferences::SharedPreferences()->setComplexFormat(
      Preferences::ComplexFormat::Real);
  assertCalculationAdditionalResultTypeHas("1+1", {.integer = true},
                                           &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("π-π", {.integer = true},
                                           &globalContext, &store);
  assertCalculationAdditionalResultTypeHas(
      "2/24", {.rational = true, .scientificNotation = true}, &globalContext,
      &store);
  assertCalculationAdditionalResultTypeHas("1+i", {.complex = true},
                                           &globalContext, &store);
  assertCalculationAdditionalResultTypeHas(
      "sin(π)", {.directTrigonometry = true}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas(
      "cos(45°)", {.directTrigonometry = true}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas(
      "acos(0.5)", {.inverseTrigonometry = true}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("sin(iπ)", {.complex = true},
                                           &globalContext, &store);
  assertCalculationAdditionalResultTypeHas(
      "ln(2)", {.function = true, .scientificNotation = true}, &globalContext,
      &store);
  assertCalculationAdditionalResultTypeHas(
      "2^3", {.integer = true, .function = true}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas(
      ".5^2", {.rational = true, .function = true, .scientificNotation = true},
      &globalContext, &store);
  assertCalculationAdditionalResultTypeHas(
      "e^3", {.function = true, .scientificNotation = true}, &globalContext,
      &store);
  assertCalculationAdditionalResultTypeHas("tan(π/2)", {}, &globalContext,
                                           &store);
  assertCalculationAdditionalResultTypeHas(
      "atan(∞)", {.inverseTrigonometry = true}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas(
      "atan(-∞)", {.inverseTrigonometry = true}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("[[1]]", {.vector = true},
                                           &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("[[1,1]]", {.vector = true},
                                           &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("[[1][2][3]]", {.vector = true},
                                           &globalContext, &store);
  assertCalculationAdditionalResultTypeHas(
      "transpose(identity(2))", {.matrix = true}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas(
      "[[cos(π/3),-sin(π/3)][sin(π/3),cos(π/3)]]", {.matrix = true},
      &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("[[mi0]]", {}, &globalContext,
                                           &store);
  assertCalculationAdditionalResultTypeHas("345nV", {.unit = true},
                                           &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("223m^3", {.unit = true},
                                           &globalContext, &store);

  assertCalculationAdditionalResultTypeHas(
      "1/400", {.rational = true, .scientificNotation = true}, &globalContext,
      &store);
  assertCalculationAdditionalResultTypeHas(
      "400", {.integer = true, .scientificNotation = true}, &globalContext,
      &store);
  assertCalculationAdditionalResultTypeHas("π+π", {}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas(
      "e^(2+3)", {.scientificNotation = true}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("2i", {.complex = true},
                                           &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("1+cos(2_rad)", {}, &globalContext,
                                           &store);
  assertCalculationAdditionalResultTypeHas("-sin(\")", {}, &globalContext,
                                           &store);
  assertCalculationAdditionalResultTypeHas("30°+2_rad", {.unit = true},
                                           &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("45_rad", {.unit = true},
                                           &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("gon", {.unit = true},
                                           &globalContext, &store);
  assertCalculationAdditionalResultTypeHas(
      "3°/(4π_rad)", {.rational = true, .scientificNotation = true},
      &globalContext, &store);
  assertCalculationAdditionalResultTypeHas(
      "3°/(4_rad)", {.scientificNotation = true}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("180°/(π_rad)", {.integer = true},
                                           &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("_L/(_L/3)", {}, &globalContext,
                                           &store);

  Preferences::SharedPreferences()->setDisplayMode(
      Preferences::PrintFloatMode::Scientific);
  assertCalculationAdditionalResultTypeHas("e^(2+3)", {}, &globalContext,
                                           &store);
  Preferences::SharedPreferences()->setDisplayMode(
      Preferences::PrintFloatMode::Decimal);

  assertCalculationAdditionalResultTypeHas("√(-1)", {}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("{1}", {}, &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("{i}", {}, &globalContext, &store);
  /* TODO: Not working on windows
   * assertCalculationAdditionalResultTypeHas("i^(2×e^(7i^(2×e^322)))", {},
   *                                         &globalContext, &store);*/

  assertCalculationAdditionalResultTypeHas("ln(3+4)", {}, &globalContext,
                                           &store);
  assertCalculationAdditionalResultTypeHas("cos(i)", {}, &globalContext,
                                           &store);
  assertCalculationAdditionalResultTypeHas("cos(i%)", {}, &globalContext,
                                           &store);
  assertMainCalculationOutputIs("i→z", "i", &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("z+1", {.complex = true},
                                           &globalContext, &store);
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("z.exp").destroy();

  Preferences::SharedPreferences()->setComplexFormat(
      Preferences::ComplexFormat::Polar);
  assertCalculationAdditionalResultTypeHas("-10", {.complex = true},
                                           &globalContext, &store);

  Preferences::SharedPreferences()->setComplexFormat(
      Preferences::ComplexFormat::Cartesian);
  assertCalculationAdditionalResultTypeHas("√(-1)", {.complex = true},
                                           &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("[[1+2i][3+i]]", {.matrix = true},
                                           &globalContext, &store);
  assertCalculationAdditionalResultTypeHas("-10", {.scientificNotation = true},
                                           &globalContext, &store);
}
