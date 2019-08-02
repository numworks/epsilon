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
  quiz_assert(strcmp(lastCalculation->approximateOutputText(),"2.6366666666667") == 0);

  store.deleteAll();
}

void assertCalculationDisplay(const char * input, ::Calculation::Calculation::DisplayOutput display, ::Calculation::Calculation::EqualSign sign, const char * exactOutput, const char * approximateOutput, Context * context, CalculationStore * store) {
  store->push(input, context);
  Shared::ExpiringPointer<::Calculation::Calculation> lastCalculation = store->calculationAtIndex(0);
  quiz_assert(lastCalculation->displayOutput(context) == display);
  if (sign != ::Calculation::Calculation::EqualSign::Unknown) {
    quiz_assert(lastCalculation->exactAndApproximateDisplayedOutputsAreEqual(context) == sign);
  }
  if (exactOutput) {
    quiz_assert_print_if_failure(strcmp(lastCalculation->exactOutputText(), exactOutput) == 0, input);
  }
  if (approximateOutput) {
    quiz_assert_print_if_failure(strcmp(lastCalculation->approximateOutputText(), approximateOutput) == 0, input);
  }
  store->deleteAll();
}

QUIZ_CASE(calculation_display_exact_approximate) {
  Shared::GlobalContext globalContext;
  CalculationStore store;

  assertCalculationDisplay("1/2", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Equal, nullptr, nullptr, &globalContext, &store);
  assertCalculationDisplay("1/3", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Approximation, nullptr, nullptr, &globalContext, &store);
  assertCalculationDisplay("1/0", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, "undef", "undef", &globalContext, &store);
  assertCalculationDisplay("2x-x", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, "undef", "undef", &globalContext, &store);
  assertCalculationDisplay("[[1,2,3]]", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, nullptr, &globalContext, &store);
  assertCalculationDisplay("[[1,x,3]]", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "undef", &globalContext, &store);
  assertCalculationDisplay("28^7", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Unknown, nullptr, nullptr, &globalContext, &store);
  assertCalculationDisplay("3+√(2)→a", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Approximation, "√(2)+3", nullptr, &globalContext, &store);
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
  assertCalculationDisplay("3+2→a", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Equal, "5", "5", &globalContext, &store);
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
  assertCalculationDisplay("3→a", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Equal, "3", "3", &globalContext, &store);
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
  assertCalculationDisplay("3+x→f(x)", ::Calculation::Calculation::DisplayOutput::ExactOnly, ::Calculation::Calculation::EqualSign::Unknown, "x+3", nullptr, &globalContext, &store);
  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();
  assertCalculationDisplay("1+1+random()", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, nullptr, &globalContext, &store);
  assertCalculationDisplay("1+1+round(1.343,2)", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "3.34", &globalContext, &store);
  assertCalculationDisplay("randint(2,2)+3", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, "5", "5", &globalContext, &store);
  assertCalculationDisplay("confidence(0.5,2)+3", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, nullptr, &globalContext, &store);
  assertCalculationDisplay("prediction(0.5,2)+3", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, nullptr, &globalContext, &store);
  assertCalculationDisplay("prediction95(0.5,2)+3", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, nullptr, &globalContext, &store);

}

QUIZ_CASE(calculation_symbolic_computation) {
  Shared::GlobalContext globalContext;
  CalculationStore store;

  assertCalculationDisplay("x+x+1+3+√(π)", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, "undef", "undef", &globalContext, &store);
  assertCalculationDisplay("f(x)", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, "undef", "undef", &globalContext, &store);
  assertCalculationDisplay("1+x→f(x)", ::Calculation::Calculation::DisplayOutput::ExactOnly, ::Calculation::Calculation::EqualSign::Unknown, "x+1", nullptr, &globalContext, &store);
  assertCalculationDisplay("f(x)", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, "undef", "undef", &globalContext, &store);
  assertCalculationDisplay("f(2)", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Equal, "3", "3", &globalContext, &store);
  assertCalculationDisplay("2→x", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Equal, "2", nullptr, &globalContext, &store);
  assertCalculationDisplay("f(x)", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Equal, "3", nullptr, &globalContext, &store);
  assertCalculationDisplay("x+x+1+3+√(π)", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Approximation, "√(π)+8", nullptr, &globalContext, &store);

  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();
  Ion::Storage::sharedStorage()->recordNamed("x.exp").destroy();
}

QUIZ_CASE(calculation_symbolic_computation_and_parametered_expressions) {
  Shared::GlobalContext globalContext;
  CalculationStore store;

  assertCalculationDisplay("int(x,x,0,2)", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "2", &globalContext, &store);
  assertCalculationDisplay("sum(x,x,0,2)", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "3", &globalContext, &store);
  assertCalculationDisplay("product(x,x,1,2)", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "2", &globalContext, &store);
  assertCalculationDisplay("diff(x^2,x,3)", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "6", &globalContext, &store);
  assertCalculationDisplay("2→x", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, nullptr, &globalContext, &store);
  assertCalculationDisplay("int(x,x,0,2)", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "2", &globalContext, &store);
  assertCalculationDisplay("sum(x,x,0,2)", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "3", &globalContext, &store);
  assertCalculationDisplay("product(x,x,1,2)", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "2", &globalContext, &store);
  assertCalculationDisplay("diff(x^2,x,3)", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "6", &globalContext, &store);

  Ion::Storage::sharedStorage()->recordNamed("x.exp").destroy();
}


QUIZ_CASE(calculation_complex_format) {
  Shared::GlobalContext globalContext;
  CalculationStore store;

  Poincare::Preferences::sharedPreferences()->setComplexFormat(Poincare::Preferences::ComplexFormat::Real);
  assertCalculationDisplay("1+𝐢", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "1+𝐢", &globalContext, &store);
  assertCalculationDisplay("√(-1)", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, "unreal", nullptr, &globalContext, &store);
  assertCalculationDisplay("ln(-2)", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "unreal", &globalContext, &store);
  assertCalculationDisplay("√(-1)×√(-1)", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "unreal", &globalContext, &store);
  assertCalculationDisplay("(-8)^(1/3)", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "-2", &globalContext, &store);
  assertCalculationDisplay("(-8)^(2/3)", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "4", &globalContext, &store);
  assertCalculationDisplay("(-2)^(1/4)", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "unreal", &globalContext, &store);

  Poincare::Preferences::sharedPreferences()->setComplexFormat(Poincare::Preferences::ComplexFormat::Cartesian);
  assertCalculationDisplay("1+𝐢", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "1+𝐢", &globalContext, &store);
  assertCalculationDisplay("√(-1)", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "𝐢", &globalContext, &store);
  assertCalculationDisplay("ln(-2)", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Approximation, "ln(-2)", nullptr, &globalContext, &store);
  assertCalculationDisplay("√(-1)×√(-1)", ::Calculation::Calculation::DisplayOutput::ApproximateOnly, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "-1", &globalContext, &store);
  assertCalculationDisplay("(-8)^(1/3)", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Approximation, "1+√(3)𝐢", nullptr, &globalContext, &store);
  assertCalculationDisplay("(-8)^(2/3)", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Approximation, "-2+2√(3)𝐢", nullptr, &globalContext, &store);
  assertCalculationDisplay("(-2)^(1/4)", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Approximation, "root(8,4)/2+\u0012root(8,4)/2\u0013𝐢", nullptr, &globalContext, &store);

  Poincare::Preferences::sharedPreferences()->setComplexFormat(Poincare::Preferences::ComplexFormat::Polar);
  assertCalculationDisplay("1+𝐢", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Approximation, "√(2)ℯ^\u0012\u0012π/4\u0013𝐢\u0013", nullptr, &globalContext, &store);
  assertCalculationDisplay("√(-1)", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Approximation, "ℯ^\u0012\u0012π/2\u0013𝐢\u0013", nullptr, &globalContext, &store);
  assertCalculationDisplay("ln(-2)", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Approximation, "ln(-2)", nullptr, &globalContext, &store);
  assertCalculationDisplay("√(-1)×√(-1)", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "ℯ^\u00123.1415926535898𝐢\u0013", &globalContext, &store);
  assertCalculationDisplay("(-8)^(1/3)", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Approximation, "2ℯ^\u0012\u0012π/3\u0013𝐢\u0013", nullptr, &globalContext, &store);
  assertCalculationDisplay("(-8)^(2/3)", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Approximation, "4ℯ^\u0012\u00122π/3\u0013𝐢\u0013", nullptr, &globalContext, &store);
  assertCalculationDisplay("(-2)^(1/4)", ::Calculation::Calculation::DisplayOutput::ExactAndApproximate, ::Calculation::Calculation::EqualSign::Approximation, "root(2,4)ℯ^\u0012\u0012π/4\u0013𝐢\u0013", nullptr, &globalContext, &store);

  Poincare::Preferences::sharedPreferences()->setComplexFormat(Poincare::Preferences::ComplexFormat::Cartesian);
}
