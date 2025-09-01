#include "../calculation_store.h"

#include <apps/calculation/additional_results/additional_results_type.h>
#include <apps/exam_mode_manager.h>
#include <apps/global_preferences.h>
#include <assert.h>
#include <poincare/cas.h>
#include <poincare/k_tree.h>
#include <poincare/test/helper.h>
#include <quiz.h>
#include <shared/global_context.h>
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

OutputLayouts pushAndProcessCalculation(CalculationStore* store,
                                        const char* input) {
  /* These two variables mirror the "font" and "maxVisibleWidth" variables in
   * HistoryViewCell::setNewCalculation */
  constexpr static KDFont::Size font = KDFont::Size::Large;
  constexpr static KDCoordinate maxVisibleWidth = 280;

  store->push(Layout::String(input));
  OMG::ExpiringPointer<Calculation::Calculation> lastCalculation =
      store->calculationAtIndex(0);

  return lastCalculation->layoutCalculation(font, maxVisibleWidth, true);
}

QUIZ_CASE(calculation_store) {
  CalculationStore store(calculationBuffer, calculationBufferSize);
  // Store is now {9, 8, 7, 6, 5, 4, 3, 2, 1, 0}
  const char* result[] = {"9", "8", "7", "6", "5", "4", "3", "2", "1", "0"};
  for (int i = 0; i < 10; i++) {
    char text[2] = {(char)(i + '0'), 0};
    pushAndProcessCalculation(&store, text);
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

  constexpr const char* pattern = "1234567890123456789+";
  constexpr size_t patternSize = 20;
  assert(strlen(pattern) == patternSize);

  constexpr size_t textSize = 200;
  static_assert(textSize % patternSize == 0);
  char text[textSize];
  for (size_t i = 0; i < textSize; i += patternSize) {
    memcpy(text + i, pattern, patternSize);
  }
  text[textSize - 1] = 0;

  const size_t emptyStoreSize = store.remainingSize();
  pushAndProcessCalculation(&store, text);
  assert(emptyStoreSize > store.remainingSize());
  const size_t calculationSize = emptyStoreSize - store.remainingSize();

  int numberOfCalculations = store.numberOfCalculations();
  while (store.remainingSize() > calculationSize) {
    pushAndProcessCalculation(&store, text);
    quiz_assert(++numberOfCalculations == store.numberOfCalculations());
  }
  int numberOfCalculations1 = store.numberOfCalculations();
  /* The buffer is now too full to push a new calculation.
   * Trying to push a new one should delete the oldest one. Alter new text to
   * distinguish it from previously pushed ones. */
  text[0] = '9';
  pushAndProcessCalculation(&store, text);
  // Assert pushed text is correct
  char buffer[4096];
  store.calculationAtIndex(0)->input().serialize(buffer);
  quiz_assert(strcmp(buffer, text) == 0);
  store.calculationAtIndex(0)->input().serialize(buffer);
  quiz_assert(strcmp(buffer, text) == 0);
  int numberOfCalculations2 = store.numberOfCalculations();
  // The numberOfCalculations should be the same
  quiz_assert(numberOfCalculations1 == numberOfCalculations2);

  store.deleteAll();
  quiz_assert(store.remainingSize() == store.maximumSize());
}

void assertAnsIs(
    const char* input, const char* expectedAnsInputText,
    CalculationStore* store,
    Poincare::Preferences::PrintFloatMode displayMode = ScientificMode) {
  pushAndProcessCalculation(store, input);
  OMG::ExpiringPointer<Calculation::Calculation> lastCalculation =
      store->calculationAtIndex(0);
  pushAndProcessCalculation(store, "Ans");
  lastCalculation = store->calculationAtIndex(0);
  assert_expression_serializes_to(lastCalculation->input(),
                                  expectedAnsInputText, displayMode);
}

QUIZ_CASE(calculation_ans) {
  CalculationStore store(calculationBuffer, calculationBufferSize);
  // Real + No exam mode
  pushAndProcessCalculation(&store, "1+3/4");
  pushAndProcessCalculation(&store, "ans+2/3");
  OMG::ExpiringPointer<Calculation::Calculation> lastCalculation =
      store.calculationAtIndex(0);
  quiz_assert(lastCalculation->displayOutput() ==
              DisplayOutput::ExactAndApproximate);
  assert_expression_serializes_to(lastCalculation->exactOutput(), "29/12");

  pushAndProcessCalculation(&store, "ans+0.22");
  lastCalculation = store.calculationAtIndex(0);
  quiz_assert(lastCalculation->displayOutput() ==
              DisplayOutput::ExactAndApproximateToggle);
  assert_expression_serializes_to(
      lastCalculation->approximateOutput(), "2.6366666666667", ScientificMode,
      Poincare::PrintFloat::k_maxNumberOfSignificantDigits);

  assertAnsIs("1+1→a", "2", &store);
  assertAnsIs("0^0→a", "0^0", &store);
  Shared::GlobalContextAccessor::Store().resetAll();

  assertAnsIs("1+1", "2", &store);
  assertAnsIs("13.3", "13.3", &store, DecimalMode);
  GlobalPreferences::SharedGlobalPreferences()->setComplexFormat(
      ComplexFormat::Cartesian);
  assertAnsIs("√(-1-1)", "√(2)×i", &store);
  GlobalPreferences::SharedGlobalPreferences()->setComplexFormat(
      ComplexFormat::Real);
  assertAnsIs("int(diff(x^2,x,x),x,0,1)", "int(diff(x^2,x,x),x,0,1)", &store);
  assertAnsIs("int(diff(x^2.1,x,x),x,0,1)", "int(diff(x^2.1,x,x),x,0,1)",
              &store);
  assertAnsIs("int(diff(x^2,x,x),x,0,1)→a", "1", &store);

  assertAnsIs("√(1+1)", "√(2)", &store);

  ExamModeManager::SetExamMode(ExamMode(ExamMode::Ruleset::Dutch));
  assert(CAS::ShouldOnlyDisplayApproximation(
      UserExpression::Builder(KSqrt(2_e)), UserExpression::Builder(KSqrt(2_e)),
      UserExpression(), Poincare::EmptyContext{}));

  assertAnsIs("√(1+1)", "√(1+1)", &store);

  ExamModeManager::SetExamMode(ExamMode(ExamMode::Ruleset::Off));

  pushAndProcessCalculation(&store, "_g0");
  pushAndProcessCalculation(&store, "ans→m*s^-2");
  lastCalculation = store.calculationAtIndex(0);
  assert_expression_serializes_to(lastCalculation->exactOutput(),
                                  "9.80665×_m×_s^(-2)");

  pushAndProcessCalculation(&store, "4546249×1.0071^9");
  pushAndProcessCalculation(&store, "Ans×1.0071^9");
  lastCalculation = store.calculationAtIndex(0);
  assert_expression_serializes_to(lastCalculation->input(),
                                  "(4546249×1.0071^9)×1.0071^9");

  store.deleteAll();
}

void assertCalculationIs(const char* input, DisplayOutput expectedDisplay,
                         EqualSign expectedSign,
                         const char* expectedExactOutput,
                         const char* expectedApproximateOutput,
                         CalculationStore* store,
                         const char* expectedStoredInput = nullptr,
                         bool skipApproximation = false) {
  OutputLayouts outputLayouts = pushAndProcessCalculation(store, input);
  OMG::ExpiringPointer<Calculation::Calculation> lastCalculation =
      store->calculationAtIndex(0);

  quiz_assert_print_if_failure(
      lastCalculation->displayOutput() == expectedDisplay, input,
      "correct displayOutput", "incorrect displayOutput");
  quiz_assert_print_if_failure(lastCalculation->equalSign() == expectedSign,
                               input, "correct equalSign",
                               "incorrect equalSign");

  if (expectedStoredInput) {
    assert_expression_serializes_to(lastCalculation->input(),
                                    expectedStoredInput);
  }
  /* To preserve the Poincare_update CI, we output a something whether or not
   * the layouts can be compared. */
  if (expectedExactOutput) {
    assert(Calculation::Calculation::CanDisplayExact(expectedDisplay));
    if (outputLayouts.exact.isUninitialized()) {
      quiz_assert_print_if_failure(false, input, expectedExactOutput,
                                   "Uninitialized");
    } else {
      assert_layout_serializes_to(outputLayouts.exact, expectedExactOutput);
    }
  } else {
    assert(!Calculation::Calculation::CanDisplayExact(expectedDisplay));
    quiz_assert_print_if_failure(true, input, "Uninitialized", "Uninitialized");
  }

  /* To preserve the Poincare_update CI, we output a something whether or not
   * the layouts can be compared. */
  if (expectedApproximateOutput && !skipApproximation) {
    assert(Calculation::Calculation::CanDisplayApproximate(expectedDisplay));
    if (outputLayouts.approximate.isUninitialized()) {
      quiz_assert_print_if_failure(false, input, expectedApproximateOutput,
                                   "Uninitialized");
    } else {
      assert_layout_serializes_to(outputLayouts.approximate,
                                  expectedApproximateOutput);
    }
  } else {
    /* When testing input containing random nodes, the approximation is skipped
     * as we can't compare it. */
    assert(skipApproximation ||
           !Calculation::Calculation::CanDisplayApproximate(expectedDisplay));
    quiz_assert_print_if_failure(true, input, "Uninitialized", "Uninitialized");
  }
  store->deleteAll();
}

QUIZ_CASE(calculation_significant_digits) {
  CalculationStore store(calculationBuffer, calculationBufferSize);

  assertCalculationIs("123456789123456789",
                      DisplayOutput::ExactAndApproximateToggle,
                      EqualSign::Approximation, "123456789123456789",
                      "1.234567891×10^17", &store);
  // FIXME: sign is wrong (returns Equal)
  assertCalculationIs("123123456789", DisplayOutput::ExactAndApproximateToggle,
                      EqualSign::Approximation, "123123456789",
                      "1.231234568×10^11", &store);
  // FIXME: sign is wrong (returns Equal)
  assertCalculationIs("11123456789", DisplayOutput::ExactAndApproximateToggle,
                      EqualSign::Approximation, "11123456789",
                      "1.112345679×10^10", &store);
  assertCalculationIs("1123456789",
                      DisplayOutput::ApproximateIsIdenticalToExact,
                      EqualSign::Hidden, "1123456789", nullptr, &store);
  assertCalculationIs("123456789", DisplayOutput::ApproximateIsIdenticalToExact,
                      EqualSign::Hidden, "123456789", nullptr, &store);
}

QUIZ_CASE(calculation_display_exact_approximate) {
  CalculationStore store(calculationBuffer, calculationBufferSize);

  AngleUnit previousAngleUnit =
      GlobalPreferences::SharedGlobalPreferences()->angleUnit();
  GlobalPreferences::SharedGlobalPreferences()->setAngleUnit(AngleUnit::Degree);

  uint8_t previousNumberOfSignificantDigits =
      GlobalPreferences::SharedGlobalPreferences()->numberOfSignificantDigits();
  GlobalPreferences::SharedGlobalPreferences()->setNumberOfSignificantDigits(
      PrintFloat::k_maxNumberOfSignificantDigits);

  assertCalculationIs("1/2", DisplayOutput::ExactAndApproximateToggle,
                      EqualSign::Equal, "1/2", "0.5", &store);
  assertCalculationIs("1/3", DisplayOutput::ExactAndApproximateToggle,
                      EqualSign::Approximation, "1/3", "0.33333333333333",
                      &store);

  assertCalculationIs("1/(2i)", DisplayOutput::ExactAndApproximate,
                      EqualSign::Equal, "-(1/2)i", "-0.5i", &store);
  assertCalculationIs("1/0", DisplayOutput::ExactOnly, EqualSign::Hidden,
                      "undef", nullptr, &store);
  assertCalculationIs("2x-x", DisplayOutput::ExactOnly, EqualSign::Hidden,
                      "undef", nullptr, &store);
  assertCalculationIs("[[1,2,3]]", DisplayOutput::ApproximateIsIdenticalToExact,
                      EqualSign::Hidden, "[[1,2,3]]", nullptr, &store);
  assertCalculationIs("[[1,x,3]]", DisplayOutput::ApproximateIsIdenticalToExact,
                      EqualSign::Hidden, "[[1,undef,3]]", nullptr, &store);
  assertCalculationIs("[[1/0,2/0]]", DisplayOutput::ApproximateOnly,
                      EqualSign::Hidden, nullptr, "undef", &store);
  assertCalculationIs("{1/0,2/0}", DisplayOutput::ApproximateIsIdenticalToExact,
                      EqualSign::Hidden, "{undef,undef}", nullptr, &store);
  assertCalculationIs("28^7", DisplayOutput::ApproximateIsIdenticalToExact,
                      EqualSign::Hidden, "13492928512", nullptr, &store);
  assertCalculationIs("3+√(2)→a", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "3+√(2)", "4.4142135623731",
                      &store);
  Shared::GlobalContextAccessor::Store().resetAll();
  assertCalculationIs("3+2→a", DisplayOutput::ApproximateIsIdenticalToExact,
                      EqualSign::Hidden, "5", nullptr, &store);
  Shared::GlobalContextAccessor::Store().resetAll();
  assertCalculationIs("3→a", DisplayOutput::ApproximateIsIdenticalToExact,
                      EqualSign::Hidden, "3", nullptr, &store);
  Shared::GlobalContextAccessor::Store().resetAll();
  assertCalculationIs("3/2→a", DisplayOutput::ExactAndApproximate,
                      EqualSign::Equal, "3/2", "1.5", &store);
  Shared::GlobalContextAccessor::Store().resetAll();
  assertCalculationIs("3+x→f(x)", DisplayOutput::ExactOnly, EqualSign::Hidden,
                      "3+x", nullptr, &store);
  Shared::GlobalContextAccessor::Store().resetAll();
  assertCalculationIs("1+1+random()", DisplayOutput::ApproximateOnly,
                      EqualSign::Hidden, nullptr, nullptr, &store, nullptr,
                      true);
  assertCalculationIs("1+1+round(1.343,2)", DisplayOutput::ApproximateOnly,
                      EqualSign::Hidden, nullptr, "3.34", &store);
  assertCalculationIs("randint(2,2)+3", DisplayOutput::ApproximateOnly,
                      EqualSign::Hidden, nullptr, "5", &store);
  assertCalculationIs("√(8)", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "2√(2)", "2.8284271247462",
                      &store);
  assertCalculationIs("cos(45×_°)", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "(√(2))/2", "0.70710678118655",
                      &store);
  assertCalculationIs("cos(π/4×_rad)", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "(√(2))/2", "0.70710678118655",
                      &store);
  assertCalculationIs("cos(50×_°)", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "cos(50)", "0.64278760968654",
                      &store);
  assertCalculationIs("binompdf(2,3,0.5)", DisplayOutput::ApproximateOnly,
                      EqualSign::Hidden, nullptr, "0.375", &store);
  assertCalculationIs("1+2%", DisplayOutput::ExactAndApproximateToggle,
                      EqualSign::Equal, "1(1+(2/100))", "1.02", &store);
  assertCalculationIs("1-(1/3)%", DisplayOutput::ExactAndApproximateToggle,
                      EqualSign::Approximation, "1×(1-((1/3)/100))",
                      "0.99666666666667", &store);
  assertCalculationIs("π-15i", DisplayOutput::ExactAndApproximateToggle,
                      EqualSign::Approximation, "π-15i", "3.1415926535898-15i",
                      &store);
  assertCalculationIs("1.234×10^22×i",
                      DisplayOutput::ApproximateIsIdenticalToExact,
                      EqualSign::Hidden, "1.234×10^22×i", nullptr, &store);

  // Big integers
  assertCalculationIs("12340000000000000000000",
                      DisplayOutput::ApproximateIsIdenticalToExact,
                      EqualSign::Hidden, "1.234×10^22", nullptr, &store);
  assertCalculationIs(
      "1234567890123456789012345678000",
      DisplayOutput::ExactAndApproximateToggle, EqualSign::Approximation,
      "1.234567890123456789012345678×10^30", "1.2345678901235×10^30", &store);
  assertCalculationIs("123456789012345678901234567800",
                      DisplayOutput::ApproximateOnly, EqualSign::Hidden,
                      nullptr, "1.2345678901235×10^29", &store);
  assertCalculationIs("1234567890123456789012345678901×10^15",
                      DisplayOutput::ApproximateOnly, EqualSign::Hidden,
                      nullptr, "1.2345678901235×10^45", &store);
  assertCalculationIs("10^45+i*10^45",
                      DisplayOutput::ApproximateIsIdenticalToExact,
                      EqualSign::Hidden, "10^45+10^45i", nullptr, &store);
  // TODO: first parentheses should be removed in approximation too.
  assertCalculationIs("-10^45-i*10^45", DisplayOutput::ExactAndApproximate,
                      EqualSign::Equal, "-(10^45)-10^45i", "-(10^45)-(10^45)i",
                      &store);

  // IntegerOverflow during reduction
  assertCalculationIs("0^(10^600)", DisplayOutput::ApproximateOnly,
                      EqualSign::Hidden, nullptr, "0", &store);

  // Exact output that have dependencies are not displayed
  assertCalculationIs("2→f(x)", DisplayOutput::ExactOnly, EqualSign::Hidden,
                      "2", nullptr, &store);
  Shared::GlobalContextAccessor::Store().resetAll();

  assertCalculationIs("(1/6)_g", DisplayOutput::ApproximateOnly,
                      EqualSign::Hidden, nullptr, "0.16666666666667g", &store);
  assertCalculationIs("(1/6)_L_kg", DisplayOutput::ApproximateOnly,
                      EqualSign::Hidden, nullptr,
                      "1.6666666666667×10^(-4)m^3·kg", &store);
  assertCalculationIs("(π/6)_rad", DisplayOutput::ApproximateOnly,
                      EqualSign::Hidden, nullptr, "30°", &store);
  assertCalculationIs("(1/11)_°", DisplayOutput::ApproximateOnly,
                      EqualSign::Hidden, nullptr, "0.090909090909091°", &store);
  assertCalculationIs("180→rad", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "πrad", "3.1415926535898rad",
                      &store);
  assertCalculationIs("45→gon", DisplayOutput::ApproximateOnly,
                      EqualSign::Hidden, nullptr, "50gon", &store);
  GlobalPreferences::SharedGlobalPreferences()->setAngleUnit(AngleUnit::Radian);
  assertCalculationIs("2+π→_rad", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "(2+π)rad",
                      "5.1415926535898rad", &store);
  assertCalculationIs("π/2→°", DisplayOutput::ApproximateOnly,
                      EqualSign::Hidden, nullptr, "90°", &store);
  assertCalculationIs("πrad/2→°", DisplayOutput::ApproximateOnly,
                      EqualSign::Hidden, nullptr, "90°", &store);
  assertCalculationIs("(1/6)_rad^(-1)", DisplayOutput::ApproximateOnly,
                      EqualSign::Hidden, nullptr, "0.16666666666667rad^(-1)",
                      &store);
  assertCalculationIs("diff(x^2,x,3)_rad", DisplayOutput::ApproximateOnly,
                      EqualSign::Hidden, nullptr, "6rad", &store);
  assertCalculationIs("(1/6)_rad→a", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "(1/6)rad",
                      "0.16666666666667rad", &store);
  assertCalculationIs("diff(x^2,x,3)_rad→a", DisplayOutput::ApproximateOnly,
                      EqualSign::Hidden, nullptr, "6rad", &store);
  // Add 1 because test fails with 0 on linux
  assertCalculationIs("int(6/x^2-8/x^3,x,1,2)+1",
                      DisplayOutput::ApproximateOnly, EqualSign::Hidden,
                      nullptr, "1", &store);
  assertCalculationIs("6i'log(14,10)", DisplayOutput::ExactAndApproximateToggle,
                      EqualSign::Approximation,
                      "6rad·(((π·log(2))/10800)i+((π·log(7))/10800)i)",
                      "0.0020003707872rad·i", &store);
  assertCalculationIs("i'log(14,10)", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation,
                      "((π·(log(2)i+log(7)i))/10800)rad",
                      "3.333951312×10^(-4)rad·i", &store);

  GlobalPreferences::SharedGlobalPreferences()->setAngleUnit(AngleUnit::Degree);
  Shared::GlobalContextAccessor::Store().resetAll();

  ExamModeManager::SetExamMode(ExamMode(ExamMode::Ruleset::Dutch));

  assertCalculationIs("1+1", DisplayOutput::ApproximateIsIdenticalToExact,
                      EqualSign::Hidden, "2", nullptr, &store);
  assertCalculationIs("1/2", DisplayOutput::ExactAndApproximateToggle,
                      EqualSign::Equal, "1/2", "0.5", &store);
  assertCalculationIs("0.5", DisplayOutput::ExactAndApproximateToggle,
                      EqualSign::Equal, "1/2", "0.5", &store);
  assertCalculationIs("√(8)", DisplayOutput::ApproximateOnly, EqualSign::Hidden,
                      nullptr, "2.8284271247462", &store);
  assertCalculationIs("cos(45×°)", DisplayOutput::ApproximateOnly,
                      EqualSign::Hidden, nullptr, "0.70710678118655", &store);
  assertCalculationIs("cos(π/4×_rad)", DisplayOutput::ApproximateOnly,
                      EqualSign::Hidden, nullptr, "0.70710678118655", &store);
  assertCalculationIs("_G", DisplayOutput::ExactOnly, EqualSign::Hidden,
                      "undef", nullptr, &store);
  assertCalculationIs("_g0", DisplayOutput::ExactOnly, EqualSign::Hidden,
                      "undef", nullptr, &store);

  using PTTFlags = ExamMode::PressToTestFlags::Flags;
  ExamModeManager::SetExamMode(ExamMode(
      ExamMode::Ruleset::PressToTest,
      ExamMode::PressToTestFlags().setFlag(PTTFlags::ForbidBasedLogarithm)));

  assertCalculationIs("ln(5)", DisplayOutput::ExactAndApproximateToggle,
                      EqualSign::Approximation, "ln(5)", "1.6094379124341",
                      &store);
  assertCalculationIs("log(5)", DisplayOutput::ExactAndApproximateToggle,
                      EqualSign::Approximation, "log(5)", "0.69897000433602",
                      &store);
  assertCalculationIs("log(5,10)", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "log(5)", "0.69897000433602",
                      &store);
  assertCalculationIs("log(5,e)", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "ln(5)", "1.6094379124341",
                      &store);
  assertCalculationIs("log(5,5)", DisplayOutput::ExactOnly, EqualSign::Hidden,
                      "undef", nullptr, &store);
  assertCalculationIs("ln(89)/ln(5)", DisplayOutput::ExactAndApproximateToggle,
                      EqualSign::Approximation, "(ln(89))/(ln(5))",
                      "2.7889465850494", &store);

  ExamModeManager::SetExamMode(ExamMode(ExamMode::Ruleset::Off));
  GlobalPreferences::SharedGlobalPreferences()->setAngleUnit(previousAngleUnit);
  GlobalPreferences::SharedGlobalPreferences()->setNumberOfSignificantDigits(
      previousNumberOfSignificantDigits);

  assertCalculationIs("(π+1)*log(7,3)", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "(1+π)log(7,3)", "7.335770099",
                      &store);
  I18n::Country previousCountry =
      GlobalPreferences::SharedGlobalPreferences()->country();
  GlobalPreferences::SharedGlobalPreferences()->setCountry(I18n::Country::NL);
  assertCalculationIs("(π+1)*log(7,3)", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "(1+π)·log(7,3)", "7.335770099",
                      &store);
  GlobalPreferences::SharedGlobalPreferences()->setCountry(previousCountry);
}

QUIZ_CASE(calculation_big_expressions) {
  CalculationStore store(calculationBuffer, calculationBufferSize);
  assertCalculationIs("identity(37)", DisplayOutput::ExactOnly,
                      EqualSign::Hidden, "undef", nullptr, &store);
}

void assertMainCalculationOutputIs(const char* input, const char* output,
                                   CalculationStore* store) {
  // For the next test, we only need to checkout input and output text.
  pushAndProcessCalculation(store, input);
  OMG::ExpiringPointer<Calculation::Calculation> lastCalculation =
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
  CalculationStore store(calculationBuffer, calculationBufferSize);

  // 0 - General cases
  assertMainCalculationOutputIs("x+x+1+3+√(π)", "undef", &store);
  assertMainCalculationOutputIs("f(x)", "undef", &store);
  assertMainCalculationOutputIs("1+x→f(x)", "1+x", &store);
  assertMainCalculationOutputIs("f(x)", "undef", &store);
  assertMainCalculationOutputIs("f(2)", "3", &store);
  assertMainCalculationOutputIs("2→x", "2", &store);
  assertMainCalculationOutputIs("f(x)", "3", &store);
  assertMainCalculationOutputIs("x+x+1+3+√(π)", "8+√(π)", &store);

  // Destroy records
  Shared::GlobalContextAccessor::Store().resetAll();

  // Derivatives
  assertCalculationIs("foo'(1)", DisplayOutput::ExactOnly, EqualSign::Hidden,
                      "undef", nullptr, &store, "f×o×o×_'×(1)");
  assertCalculationIs("foo\"(1)", DisplayOutput::ExactOnly, EqualSign::Hidden,
                      "undef", nullptr, &store, "f×o×o×_\"×(1)");
  assertCalculationIs("foo^(3)(1)", DisplayOutput::ExactOnly, EqualSign::Hidden,
                      "undef", nullptr, &store, "f×o×o^(3)×(1)");
  assertMainCalculationOutputIs("x^4→foo(x)", "x^4", &store);
  assertCalculationIs("foo'(1)", DisplayOutput::ApproximateOnly,
                      EqualSign::Hidden, nullptr, "4", &store, "foo'(1)");
  assertCalculationIs("foo^(1)(1)", DisplayOutput::ApproximateOnly,
                      EqualSign::Hidden, nullptr, "4", &store, "foo'(1)");
  assertCalculationIs("foo\"(1)", DisplayOutput::ApproximateOnly,
                      EqualSign::Hidden, nullptr, "12", &store, "foo\"(1)");
  assertCalculationIs("foo^(2)(1)", DisplayOutput::ApproximateOnly,
                      EqualSign::Hidden, nullptr, "12", &store, "foo\"(1)");
  assertCalculationIs("foo^(3)(1)", DisplayOutput::ApproximateOnly,
                      EqualSign::Hidden, nullptr, "24", &store, "foo^(3)(1)");
  Shared::GlobalContextAccessor::Store().resetAll();

  // 1 - Predefined variable in fraction in integral
  assertMainCalculationOutputIs("int(x+1/x,x,1,2)", "2.193147", &store);
  assertMainCalculationOutputIs("1→x", "1", &store);
  assertMainCalculationOutputIs("int(x+1/x,x,1,2)", "2.193147", &store);
  // Destroy records
  Shared::GlobalContextAccessor::Store().resetAll();

  /* 2 - Circularly defined functions
   *   A - f(x) = g(x) = f(x) */
  assertMainCalculationOutputIs("1→f(x)", "1", &store);
  assertMainCalculationOutputIs("f(x)→g(x)", "f(x)", &store);
  assertMainCalculationOutputIs("g(x)→f(x)", "g(x)", &store);
  // With x undefined
  assertMainCalculationOutputIs("f(x)", "undef", &store);
  assertMainCalculationOutputIs("diff(f(x),x,1)", "undef", &store);
  // With x  defined
  assertMainCalculationOutputIs("1→x", "1", &store);

  assertMainCalculationOutputIs("f(x)", "undef", &store);
  assertMainCalculationOutputIs("diff(f(x),x,1)", "undef", &store);
  Shared::GlobalContextAccessor::Store().resetAll();
  //   B - f(x) = g(x) = a = f(x)
  assertMainCalculationOutputIs("f(x)→a", "undef", &store);
  assertMainCalculationOutputIs("a→g(x)", "a", &store);
  // With x undefined
  assertMainCalculationOutputIs("f(x)", "undef", &store);
  assertMainCalculationOutputIs("diff(f(x),x,1)", "undef", &store);
  // With x defined
  assertMainCalculationOutputIs("1→x", "1", &store);

  assertMainCalculationOutputIs("f(x)", "undef", &store);
  assertMainCalculationOutputIs("diff(f(x),x,1)", "undef", &store);
  //   C - g(x) = f(f(..))
  assertMainCalculationOutputIs("x+5→f(x)", "x+5", &store);
  assertMainCalculationOutputIs("f(f(1-f(x)))→g(x)", "f(f(1-f(x)))", &store);
  assertMainCalculationOutputIs("g(4)", "2", &store);

  // Destroy records
  Shared::GlobalContextAccessor::Store().resetAll();

  // 3 - Differences between functions and variables
  assertMainCalculationOutputIs("x+1→f(x)", "x+1", &store);
  assertMainCalculationOutputIs("x+1→y", "undef", &store);
  // With x undefined
  assertMainCalculationOutputIs("y", "undef", &store);

  assertMainCalculationOutputIs("int(y,x,1,3)", "undef", &store);
  assertMainCalculationOutputIs("sum(y,x,0,1)", "undef", &store);
  assertMainCalculationOutputIs("product(y,x,0,1)", "undef", &store);
  assertMainCalculationOutputIs("diff(y,x,1)", "undef", &store);

  assertMainCalculationOutputIs("f(y)", "undef", &store);
  assertMainCalculationOutputIs("diff(f(y),x,1)", "undef", &store);
  assertMainCalculationOutputIs("diff(f(x)×y,x,1)", "undef", &store);
  assertMainCalculationOutputIs("diff(f(x×y),x,1)", "undef", &store);

  // With x defined
  assertMainCalculationOutputIs("1→x", "1", &store);
  assertMainCalculationOutputIs("y", "undef", &store);
  assertMainCalculationOutputIs("x+1→y", "2", &store);
  assertMainCalculationOutputIs("y", "2", &store);

  assertMainCalculationOutputIs("int(x+1,x,1,3)", "6", &store);
  assertMainCalculationOutputIs("int(f(x),x,1,3)", "6", &store);
  assertMainCalculationOutputIs("int(y,x,1,3)", "4", &store);

  assertMainCalculationOutputIs("sum(x+1,x,0,1)", "3", &store);
  assertMainCalculationOutputIs("sum(f(x),x,0,1)", "3", &store);
  assertMainCalculationOutputIs("sum(y,x,0,1)", "4", &store);

  assertMainCalculationOutputIs("product(x+1,x,0,1)", "2", &store);
  assertMainCalculationOutputIs("product(f(x),x,0,1)", "2", &store);
  assertMainCalculationOutputIs("product(y,x,0,1)", "4", &store);

  assertMainCalculationOutputIs("diff(x+1,x,1)", "1", &store);
  assertMainCalculationOutputIs("diff(f(x),x,1)", "1", &store);
  assertMainCalculationOutputIs("diff(y,x,1)", "0", &store);

  assertMainCalculationOutputIs("f(y)", "3", &store);
  assertMainCalculationOutputIs("diff(f(y),x,1)", "0", &store);
  assertMainCalculationOutputIs("diff(f(x)×y,x,1)", "2", &store);
  assertMainCalculationOutputIs("diff(f(x×y),x,1)", "2", &store);
  // Destroy records
  Shared::GlobalContextAccessor::Store().resetAll();

  // 4 - Nested local variables within variables
  assertMainCalculationOutputIs("int(x+1,x,1,3)→a", "6", &store);
  assertMainCalculationOutputIs("a", "6", &store);
  assertMainCalculationOutputIs("a+1→a", "7", &store);
  assertMainCalculationOutputIs("diff(y×a,y,1)", "7", &store);

  // Destroy records
  Shared::GlobalContextAccessor::Store().resetAll();

  assertMainCalculationOutputIs("2→x", "2", &store);
  assertMainCalculationOutputIs("diff(x,x,x)→a", "1", &store);
  assertMainCalculationOutputIs("diff(a,x,3)", "0", &store);
  // Destroy records
  Shared::GlobalContextAccessor::Store().resetAll();

  // 5 - Double nested local variables within variables
  assertMainCalculationOutputIs("1→x", "1", &store);
  assertMainCalculationOutputIs("x→a", "1", &store);
  assertMainCalculationOutputIs("diff(a,x,x)→b", "0", &store);
  assertMainCalculationOutputIs("b", "0", &store);
  // Destroy records
  Shared::GlobalContextAccessor::Store().resetAll();

#if 0  // TODO: Fix function definition with their own expression
  // 6 - Define function from their own expression
  assertMainCalculationOutputIs("x+1→f(x)", "x+1", &store);
  assertMainCalculationOutputIs("f(x^2)→f(x)", "x^2+1", &store);
  Shared::GlobalContextAccessor::Store().resetAll();
#endif

  // 7 - Circularly defined functions with exponential expression size growth
  assertMainCalculationOutputIs("x→f(x)", "x", &store);
  assertMainCalculationOutputIs("f(xx)→g(x)", "f(x×x)", &store);
  assertMainCalculationOutputIs("g(xx)→f(x)", "g(x×x)", &store);
  assertMainCalculationOutputIs("g(2)", "undef", &store);
  Shared::GlobalContextAccessor::Store().resetAll();

  // 8 - Circularly with symbols
  pushAndProcessCalculation(&store, "x→f(x)");
  assertMainCalculationOutputIs("f(Ans)→A", "undef", &store);
  Shared::GlobalContextAccessor::Store().resetAll();

#if 0  // TODO: Fix function definition with their own expression
  // Derivatives condensed form
  pushAndProcessCalculation(&store, "2→c(x)");
  assertMainCalculationOutputIs("c''(0)→c(x)", "diff(2,x,0,2)",
                                &store);
  Shared::GlobalContextAccessor::Store().resetAll();
#endif
}

QUIZ_CASE(calculation_symbolic_computation_and_parametered_expressions) {
  CalculationStore store(calculationBuffer, calculationBufferSize);

  // Tests a bug with symbolic computation
  assertCalculationIs("int((e^(-x))-x^(0.5), x, 0, 3)",
                      DisplayOutput::ApproximateOnly, EqualSign::Hidden,
                      nullptr, "-2.513888684", &store);
  assertCalculationIs("int(x,x,0,2)", DisplayOutput::ApproximateOnly,
                      EqualSign::Hidden, nullptr, "2", &store);
  assertCalculationIs("sum(x,x,0,2)", DisplayOutput::ApproximateOnly,
                      EqualSign::Hidden, nullptr, "3", &store);
  assertCalculationIs("product(x,x,1,2)", DisplayOutput::ApproximateOnly,
                      EqualSign::Hidden, nullptr, "2", &store);
  assertCalculationIs("diff(x^2,x,3)", DisplayOutput::ApproximateOnly,
                      EqualSign::Hidden, nullptr, "6", &store);
  assertCalculationIs("2→x", DisplayOutput::ApproximateIsIdenticalToExact,
                      EqualSign::Hidden, "2", nullptr, &store);
  assertCalculationIs("int(x,x,0,2)", DisplayOutput::ApproximateOnly,
                      EqualSign::Hidden, nullptr, "2", &store);
  assertCalculationIs("sum(x,x,0,2)", DisplayOutput::ApproximateOnly,
                      EqualSign::Hidden, nullptr, "3", &store);
  assertCalculationIs("product(x,x,1,2)", DisplayOutput::ApproximateOnly,
                      EqualSign::Hidden, nullptr, "2", &store);
  assertCalculationIs("diff(x^2,x,3)", DisplayOutput::ApproximateOnly,
                      EqualSign::Hidden, nullptr, "6", &store);

  Shared::GlobalContextAccessor::Store().resetAll();
}

QUIZ_CASE(calculation_complex_format) {
  CalculationStore store(calculationBuffer, calculationBufferSize);

  uint8_t previousNumberOfSignificantDigits =
      GlobalPreferences::SharedGlobalPreferences()->numberOfSignificantDigits();
  GlobalPreferences::SharedGlobalPreferences()->setNumberOfSignificantDigits(
      PrintFloat::k_maxNumberOfSignificantDigits);

  GlobalPreferences::SharedGlobalPreferences()->setComplexFormat(
      ComplexFormat::Real);
  assertCalculationIs("1+i", DisplayOutput::ApproximateIsIdenticalToExact,
                      EqualSign::Hidden, "1+i", nullptr, &store);
  assertCalculationIs("√(-1)", DisplayOutput::ExactOnly, EqualSign::Hidden,
                      "nonreal", nullptr, &store);
  assertCalculationIs("ln(-2)", DisplayOutput::ExactOnly, EqualSign::Hidden,
                      "nonreal", nullptr, &store);
  assertCalculationIs("√(-1)×√(-1)", DisplayOutput::ExactOnly,
                      EqualSign::Hidden, "nonreal", nullptr, &store);
  assertCalculationIs("(-8)^(1/3)",
                      DisplayOutput::ApproximateIsIdenticalToExact,
                      EqualSign::Hidden, "-2", nullptr, &store);
  assertCalculationIs("(-8)^(2/3)",
                      DisplayOutput::ApproximateIsIdenticalToExact,
                      EqualSign::Hidden, "4", nullptr, &store);
  assertCalculationIs("(-2)^(1/4)", DisplayOutput::ExactOnly, EqualSign::Hidden,
                      "nonreal", nullptr, &store);

  GlobalPreferences::SharedGlobalPreferences()->setComplexFormat(
      ComplexFormat::Cartesian);
  assertCalculationIs("1+i", DisplayOutput::ApproximateIsIdenticalToExact,
                      EqualSign::Hidden, "1+i", nullptr, &store);
  assertCalculationIs("√(-1)", DisplayOutput::ApproximateIsIdenticalToExact,
                      EqualSign::Hidden, "i", nullptr, &store);
  assertCalculationIs("ln(-2)", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "ln(2)+π·i",
                      "0.69314718055995+3.1415926535898i", &store);
  assertCalculationIs("√(-1)×√(-1)",
                      DisplayOutput::ApproximateIsIdenticalToExact,
                      EqualSign::Hidden, "-1", nullptr, &store);
  assertCalculationIs("(-8)^(1/3)", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "1+√(3)i", "1+1.7320508075689i",
                      &store);
  assertCalculationIs("(-8)^(2/3)", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "-2+2√(3)i",
                      "-2+3.4641016151378i", &store);
  assertCalculationIs("(-2)^(1/4)", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "((2^(3/4))/2)+((2^(3/4))/2)i",
                      "0.84089641525371+0.84089641525371i", &store);

  GlobalPreferences::SharedGlobalPreferences()->setComplexFormat(
      ComplexFormat::Polar);
  assertCalculationIs("1+i", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "√(2)e^((π/4)i)",
                      "1.4142135623731e^(0.78539816339745i)", &store);
  assertCalculationIs("√(-1)", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "e^((π/2)i)",
                      "e^(1.5707963267949i)", &store);
  assertCalculationIs("ln(-2)", DisplayOutput::ExactAndApproximateToggle,
                      EqualSign::Approximation, "ln(-2)",
                      "3.2171505117118e^(1.3536398454434i)", &store);
  assertCalculationIs("√(-1)×√(-1)", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "e^(π·i)",
                      "e^(3.1415926535898i)", &store);
  assertCalculationIs("(-8)^(1/3)", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "2e^((π/3)i)",
                      "2e^(1.0471975511966i)", &store);
  assertCalculationIs("(-8)^(2/3)", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "4e^(((2π)/3)i)",
                      "4e^(2.0943951023932i)", &store);
  assertCalculationIs("(-2)^(1/4)", DisplayOutput::ExactAndApproximate,
                      EqualSign::Approximation, "root(2,4)e^((π/4)i)",
                      "1.1892071150027e^(0.78539816339745i)", &store);

  GlobalPreferences::SharedGlobalPreferences()->setComplexFormat(
      ComplexFormat::Real);

  GlobalPreferences::SharedGlobalPreferences()->setNumberOfSignificantDigits(
      previousNumberOfSignificantDigits);
}

QUIZ_CASE(calculation_involving_sequence) {
  Shared::SequenceStore* seqStore = Shared::GlobalContext::s_sequenceStore;
  Ion::Storage::Record::ErrorStatus err = seqStore->addEmptyModel();
  assert(err == Ion::Storage::Record::ErrorStatus::None);
  Ion::Storage::Record record =
      seqStore->recordAtIndex(seqStore->numberOfModels() - 1);
  Shared::Sequence* u = seqStore->modelForRecord(record);
  u->setType(Shared::Sequence::Type::Explicit);
  err = u->setContent(Layout::String("i"),
                      Shared::GlobalContextAccessor::Context());
  assert(err == Ion::Storage::Record::ErrorStatus::None);
  (void)err;  // Silence compilation warning.

  CalculationStore calcStore(calculationBuffer, calculationBufferSize);

  assertMainCalculationOutputIs("√(i×u(0))×√(6)", "undef", &calcStore);
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
    CalculationStore* store) {
  pushAndProcessCalculation(store, input);
  OMG::ExpiringPointer<Calculation::Calculation> lastCalculation =
      store->calculationAtIndex(0);
  quiz_assert_print_if_failure(
      lastCalculation->additionalResultsType() == additionalResultsType, input,
      "correct additional results", "incorrect additional results");
  store->deleteAll();
}

QUIZ_CASE(calculation_additional_results) {
  CalculationStore store(calculationBuffer, calculationBufferSize);

  GlobalPreferences::SharedGlobalPreferences()->setComplexFormat(
      ComplexFormat::Real);
  assertCalculationAdditionalResultTypeHas("1+1", {.integer = true}, &store);
  assertCalculationAdditionalResultTypeHas("π-π", {.integer = true}, &store);
  assertCalculationAdditionalResultTypeHas(
      "2/24", {.rational = true, .scientificNotation = true}, &store);
  assertCalculationAdditionalResultTypeHas("1+i", {.complex = true}, &store);
  assertCalculationAdditionalResultTypeHas(
      "sin(π)", {.directTrigonometry = true}, &store);
  assertCalculationAdditionalResultTypeHas(
      "cos(45°)", {.directTrigonometry = true}, &store);
  assertCalculationAdditionalResultTypeHas(
      "acos(0.5)", {.inverseTrigonometry = true}, &store);
  assertCalculationAdditionalResultTypeHas(
      "acos(e/5)", {.inverseTrigonometry = true}, &store);
  assertCalculationAdditionalResultTypeHas("sin(iπ)", {.complex = true},
                                           &store);
  assertCalculationAdditionalResultTypeHas(
      "ln(2)", {.function = true, .scientificNotation = true}, &store);
  assertCalculationAdditionalResultTypeHas(
      "2^3", {.integer = true, .function = true}, &store);
  assertCalculationAdditionalResultTypeHas(
      ".5^2", {.rational = true, .function = true, .scientificNotation = true},
      &store);
  assertCalculationAdditionalResultTypeHas(
      "e^3", {.function = true, .scientificNotation = true}, &store);
  assertCalculationAdditionalResultTypeHas("tan(π/2)", {}, &store);
  assertCalculationAdditionalResultTypeHas("atan(i)", {}, &store);
  assertCalculationAdditionalResultTypeHas(
      "atan(∞)", {.inverseTrigonometry = true}, &store);
  assertCalculationAdditionalResultTypeHas(
      "atan(-∞)", {.inverseTrigonometry = true}, &store);
  assertCalculationAdditionalResultTypeHas("[[1]]", {.vector = true}, &store);
  assertCalculationAdditionalResultTypeHas("[[1,1]]", {.vector = true}, &store);
  assertCalculationAdditionalResultTypeHas("[[1][2][3]]", {.vector = true},
                                           &store);
  assertCalculationAdditionalResultTypeHas("transpose(identity(2))",
                                           {.matrix = true}, &store);
  assertCalculationAdditionalResultTypeHas(
      "[[cos(π/3),-sin(π/3)][sin(π/3),cos(π/3)]]", {.matrix = true}, &store);
  assertCalculationAdditionalResultTypeHas("[[mi0]]", {}, &store);
  assertCalculationAdditionalResultTypeHas("345nV", {.unit = true}, &store);
  assertCalculationAdditionalResultTypeHas("223m^3", {.unit = true}, &store);

  assertCalculationAdditionalResultTypeHas(
      "1/400", {.rational = true, .scientificNotation = true}, &store);
  assertCalculationAdditionalResultTypeHas(
      "400", {.integer = true, .scientificNotation = true}, &store);
  assertCalculationAdditionalResultTypeHas(
      "sum(k,k,0,5)", {.integer = true, .scientificNotation = true}, &store);
  assertCalculationAdditionalResultTypeHas("π+π", {}, &store);
  assertCalculationAdditionalResultTypeHas(
      "e^(2+3)", {.scientificNotation = true}, &store);
  assertCalculationAdditionalResultTypeHas("2i", {.complex = true}, &store);
  assertCalculationAdditionalResultTypeHas("1+cos(2_rad)", {}, &store);
  assertCalculationAdditionalResultTypeHas("-sin(\")", {}, &store);
  assertCalculationAdditionalResultTypeHas("i=0", {}, &store);
  assertCalculationAdditionalResultTypeHas("30°+2_rad", {.unit = true}, &store);
  assertCalculationAdditionalResultTypeHas("45_rad", {.unit = true}, &store);
  assertCalculationAdditionalResultTypeHas("gon", {.unit = true}, &store);
  assertCalculationAdditionalResultTypeHas(
      "3°/(4π_rad)", {.rational = true, .scientificNotation = true}, &store);
  assertCalculationAdditionalResultTypeHas(
      "3°/(4_rad)", {.scientificNotation = true}, &store);
  assertCalculationAdditionalResultTypeHas("180°/(π_rad)", {.integer = true},
                                           &store);
  assertCalculationAdditionalResultTypeHas("_L/(_L/3)", {}, &store);

  // IntegerOverflow during reduction
  assertCalculationAdditionalResultTypeHas("0^(10^600)", {}, &store);

  GlobalPreferences::SharedGlobalPreferences()->setDisplayMode(
      Preferences::PrintFloatMode::Scientific);
  assertCalculationAdditionalResultTypeHas("e^(2+3)", {}, &store);
  GlobalPreferences::SharedGlobalPreferences()->setDisplayMode(
      Preferences::PrintFloatMode::Decimal);

  assertCalculationAdditionalResultTypeHas("√(-1)", {}, &store);
  assertCalculationAdditionalResultTypeHas("{1}", {}, &store);
  assertCalculationAdditionalResultTypeHas("{i}", {}, &store);
  /* TODO: Not working on windows
   * assertCalculationAdditionalResultTypeHas("i^(2×e^(7i^(2×e^322)))", {},
   *                                         &store);*/

  assertCalculationAdditionalResultTypeHas("ln(3+4)", {}, &store);
  assertCalculationAdditionalResultTypeHas("cos(i)", {}, &store);
  assertCalculationAdditionalResultTypeHas("cos(i%)", {}, &store);
  assertMainCalculationOutputIs("i→z", "i", &store);
  assertCalculationAdditionalResultTypeHas("z+1", {.complex = true}, &store);
  Shared::GlobalContextAccessor::Store().resetAll();

#if 0  // TODO: Fix additional results for negative numbers in polar form
  GlobalPreferences::SharedGlobalPreferences()->setComplexFormat(
      ComplexFormat::Polar);
  assertCalculationAdditionalResultTypeHas("-10", {.complex = true},
                                           &store);
#endif

  GlobalPreferences::SharedGlobalPreferences()->setComplexFormat(
      ComplexFormat::Cartesian);
  assertCalculationAdditionalResultTypeHas("√(-1)", {.complex = true}, &store);
  assertCalculationAdditionalResultTypeHas("[[1+2i][3+i]]", {.matrix = true},
                                           &store);
  assertCalculationAdditionalResultTypeHas("-10", {.scientificNotation = true},
                                           &store);

  GlobalPreferences::SharedGlobalPreferences()->setComplexFormat(
      ComplexFormat::Real);
}
