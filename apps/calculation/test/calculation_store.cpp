#include <quiz.h>
#include <apps/shared/global_context.h>
#include <poincare/test/helper.h>
#include <string.h>
#include <assert.h>
#include "../calculation_store.h"

using namespace Poincare;
using namespace Calculation;

void assert_store_is(CalculationStore * store, const char * * result) {
  for (int i = 0; i < store->numberOfCalculations(); i++) {
    quiz_assert(strcmp(store->calculationAtIndex(i)->inputText(), result[i]) == 0);
  }
}


QUIZ_CASE(calculation_store) {
  Shared::GlobalContext globalContext;
  CalculationStore store;
  // Store is now {9, 8, 7, 6, 5, 4, 3, 2, 1, 0}
  const char * result[] = {"9", "8", "7", "6", "5", "4", "3", "2", "1", "0"};
  for (int i = 0; i < 10; i++) {
    char text[2] = {(char)(i+'0'), 0};
    store.push(text, &globalContext);
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
}

QUIZ_CASE(calculation_ans) {
  Shared::GlobalContext globalContext;
  CalculationStore store;

  store.push("1+3/4", &globalContext);
  store.push("ans+2/3", &globalContext);
  Shared::ExpiringPointer<::Calculation::Calculation> lastCalculation = store.calculationAtIndex(0);
  quiz_assert(lastCalculation->displayOutput(&globalContext) == ::Calculation::Calculation::DisplayOutput::ExactAndApproximate);
  quiz_assert(strcmp(lastCalculation->exactOutputText(),"29/12") == 0);

  store.push("ans+0.22", &globalContext);
  lastCalculation = store.calculationAtIndex(0);
  quiz_assert(lastCalculation->displayOutput(&globalContext) == ::Calculation::Calculation::DisplayOutput::ExactAndApproximateToggle);
  quiz_assert(strcmp(lastCalculation->approximateOutputText(::Calculation::Calculation::NumberOfSignificantDigits::Maximal),"2.6366666666667") == 0);

  store.deleteAll();
}

void assertCalculationIs(const char * input, ::Calculation::Calculation::DisplayOutput display, ::Calculation::Calculation::EqualSign sign, const char * exactOutput, const char * displayedApproximateOutput, const char * storedApproximateOutput, Context * context, CalculationStore * store) {
  store->push(input, context);
  Shared::ExpiringPointer<::Calculation::Calculation> lastCalculation = store->calculationAtIndex(0);
  quiz_assert(lastCalculation->displayOutput(context) == display);
  if (sign != ::Calculation::Calculation::EqualSign::Unknown) {
    quiz_assert(lastCalculation->exactAndApproximateDisplayedOutputsAreEqual(context) == sign);
  }
  if (exactOutput) {
    quiz_assert_print_if_failure(strcmp(lastCalculation->exactOutputText(), exactOutput) == 0, input);
  }
  if (displayedApproximateOutput) {
    quiz_assert_print_if_failure(strcmp(lastCalculation->approximateOutputText(::Calculation::Calculation::NumberOfSignificantDigits::UserDefined), displayedApproximateOutput) == 0, input);
  }
  if (storedApproximateOutput) {
    quiz_assert_print_if_failure(strcmp(lastCalculation->approximateOutputText(::Calculation::Calculation::NumberOfSignificantDigits::Maximal), storedApproximateOutput) == 0, input);
  }
  store->deleteAll();
}

QUIZ_CASE(calculation_significant_digits) {
  Shared::GlobalContext globalContext;
  CalculationStore store;

  assertCalculationIs("123456789", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Approximation, "123456789", "1.234568ᴇ8", "123456789", &globalContext, &store);
  assertCalculationIs("1234567", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Equal, "1234567", "1234567", "1234567", &globalContext, &store);

}

QUIZ_CASE(calculation_display_exact_approximate) {
  Shared::GlobalContext globalContext;
  CalculationStore store;

  assertCalculationIs("1/2", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Equal, nullptr, nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("1/3", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Approximation, nullptr, nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("1/0", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, "undef", "undef", "undef", &globalContext, &store);
  assertCalculationIs("2x-x", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, "undef", "undef", "undef", &globalContext, &store);
  assertCalculationIs("[[1,2,3]]", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("[[1,x,3]]", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "undef", "undef", &globalContext, &store);
  assertCalculationIs("28^7", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Unknown, nullptr, nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("3+√(2)→a", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Approximation, "√(2)+3", nullptr, nullptr, &globalContext, &store);
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
  assertCalculationIs("3+2→a", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Equal, "5", "5", "5", &globalContext, &store);
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
  assertCalculationIs("3→a", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Equal, "3", "3", "3", &globalContext, &store);
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
  assertCalculationIs("3+x→f(x)", ::Calculation::Calculation::DisplayOutput::ExactOnly, ::Calculation::Calculation::EqualSign::Unknown, "x+3", nullptr, nullptr, &globalContext, &store);
  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();
  assertCalculationIs("1+1+random()", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("1+1+round(1.343,2)", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "3.34", "3.34", &globalContext, &store);
  assertCalculationIs("randint(2,2)+3", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, "5", "5", "5", &globalContext, &store);
  assertCalculationIs("confidence(0.5,2)+3", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("prediction(0.5,2)+3", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("prediction95(0.5,2)+3", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, nullptr, nullptr, &globalContext, &store);

}

QUIZ_CASE(calculation_symbolic_computation) {
  Shared::GlobalContext globalContext;
  CalculationStore store;

  assertCalculationIs("x+x+1+3+√(π)", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, "undef", "undef", "undef", &globalContext, &store);
  assertCalculationIs("f(x)", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, "undef", "undef", "undef", &globalContext, &store);
  assertCalculationIs("1+x→f(x)", ::Calculation::Calculation::DisplayOutput::ExactOnly, ::Calculation::Calculation::EqualSign::Unknown, "x+1", nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("f(x)", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, "undef", "undef", "undef", &globalContext, &store);
  assertCalculationIs("f(2)", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Equal, "3", "3", "3", &globalContext, &store);
  assertCalculationIs("2→x", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Equal, "2", nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("f(x)", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Equal, "3", nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("x+x+1+3+√(π)", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Approximation, "√(π)+8", nullptr, nullptr, &globalContext, &store);

  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();
  Ion::Storage::sharedStorage()->recordNamed("x.exp").destroy();
}

QUIZ_CASE(calculation_symbolic_computation_and_parametered_expressions) {
  Shared::GlobalContext globalContext;
  CalculationStore store;

  assertCalculationIs("int((ℯ^(-x))-x^(0.5), x, 0, 3)", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, nullptr, nullptr, &globalContext, &store); // Tests a bug with symbolic computation
  assertCalculationIs("int(x,x,0,2)", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "2", "2", &globalContext, &store);
  assertCalculationIs("sum(x,x,0,2)", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "3", "3", &globalContext, &store);
  assertCalculationIs("product(x,x,1,2)", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "2", "2", &globalContext, &store);
  assertCalculationIs("diff(x^2,x,3)", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "6", "6", &globalContext, &store);
  assertCalculationIs("2→x", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("int(x,x,0,2)", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "2", "2", &globalContext, &store);
  assertCalculationIs("sum(x,x,0,2)", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "3", "3", &globalContext, &store);
  assertCalculationIs("product(x,x,1,2)", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "2", "2", &globalContext, &store);
  assertCalculationIs("diff(x^2,x,3)", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "6", "6", &globalContext, &store);

  Ion::Storage::sharedStorage()->recordNamed("x.exp").destroy();
}


QUIZ_CASE(calculation_complex_format) {
  Shared::GlobalContext globalContext;
  CalculationStore store;

  Poincare::Preferences::sharedPreferences()->setComplexFormat(Poincare::Preferences::ComplexFormat::Real);
  assertCalculationIs("1+𝐢", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "1+𝐢", "1+𝐢", &globalContext, &store);
  assertCalculationIs("√(-1)", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, "unreal", nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("ln(-2)", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "unreal", "unreal", &globalContext, &store);
  assertCalculationIs("√(-1)×√(-1)", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "unreal", "unreal", &globalContext, &store);
  assertCalculationIs("(-8)^(1/3)", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "-2", "-2", &globalContext, &store);
  assertCalculationIs("(-8)^(2/3)", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "4", "4", &globalContext, &store);
  assertCalculationIs("(-2)^(1/4)", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "unreal", "unreal", &globalContext, &store);

  Poincare::Preferences::sharedPreferences()->setComplexFormat(Poincare::Preferences::ComplexFormat::Cartesian);
  assertCalculationIs("1+𝐢", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "1+𝐢", "1+𝐢", &globalContext, &store);
  assertCalculationIs("√(-1)", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "𝐢", "𝐢", &globalContext, &store);
  assertCalculationIs("ln(-2)", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Approximation, "ln(-2)", nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("√(-1)×√(-1)", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "-1", "-1", &globalContext, &store);
  assertCalculationIs("(-8)^(1/3)", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Approximation, "1+√(3)×𝐢", nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("(-8)^(2/3)", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Approximation, "-2+2×√(3)×𝐢", nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("(-2)^(1/4)", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Approximation, "root(8,4)/2+root(8,4)/2×𝐢", nullptr, nullptr, &globalContext, &store);

  Poincare::Preferences::sharedPreferences()->setComplexFormat(Poincare::Preferences::ComplexFormat::Polar);
  assertCalculationIs("1+𝐢", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Approximation, "√(2)×ℯ^\u0012π/4×𝐢\u0013", nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("√(-1)", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Approximation, "ℯ^\u0012π/2×𝐢\u0013", nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("ln(-2)", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Approximation, "ln(-2)", nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("√(-1)×√(-1)", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "ℯ^\u00123.141593×𝐢\u0013", "ℯ^\u00123.1415926535898×𝐢\u0013", &globalContext, &store);
  assertCalculationIs("(-8)^(1/3)", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Approximation, "2×ℯ^\u0012π/3×𝐢\u0013", nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("(-8)^(2/3)", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Approximation, "4×ℯ^\u0012\u00122×π\u0013/3×𝐢\u0013", nullptr, nullptr, &globalContext, &store);
  assertCalculationIs("(-2)^(1/4)", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Approximation, "root(2,4)×ℯ^\u0012π/4×𝐢\u0013", nullptr, nullptr, &globalContext, &store);

  Poincare::Preferences::sharedPreferences()->setComplexFormat(Poincare::Preferences::ComplexFormat::Cartesian);
}
