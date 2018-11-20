#include <quiz.h>
#include <apps/shared/global_context.h>
#include <string.h>
#include <assert.h>
#include "../calculation_store.h"

using namespace Poincare;
using namespace Calculation;

void assert_store_is(CalculationStore * store, const char * result[10]) {
  for (int i = 0; i < store->numberOfCalculations(); i++) {
    quiz_assert(strcmp(store->calculationAtIndex(i)->inputText(), result[i]) == 0);
  }
}

QUIZ_CASE(calculation_store) {
  Shared::GlobalContext globalContext;
  CalculationStore store;
  quiz_assert(CalculationStore::k_maxNumberOfCalculations == 10);
  for (int i = 0; i < CalculationStore::k_maxNumberOfCalculations; i++) {
    char text[2] = {(char)(i+'0'), 0};
    store.push(text, &globalContext);
    quiz_assert(store.numberOfCalculations() == i+1);
  }
  /* Store is now {0, 1, 2, 3, 4, 5, 6, 7, 8, 9} */
  const char * result[10] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
  assert_store_is(&store, result);

  store.push("10", &globalContext);
  /* Store is now {1, 2, 3, 4, 5, 6, 7, 8, 9, 10} */
  const char * result1[10] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};
  assert_store_is(&store, result1);

  for (int i = 9; i > 0; i = i-2) {
   store.deleteCalculationAtIndex(i);
  }
  /* Store is now {1, 3, 5, 7, 9} */
  const char * result2[10] = {"1", "3", "5", "7", "9", "", "", "", "", ""};
  assert_store_is(&store, result2);

  for (int i = 5; i < CalculationStore::k_maxNumberOfCalculations; i++) {
    char text[3] = {(char)(i+'0'), 0};
    store.push(text, &globalContext);
    quiz_assert(store.numberOfCalculations() == i+1);
  }
  /* Store is now {0, 2, 4, 6, 8, 5, 6, 7, 8, 9} */
  const char * result3[10] = {"1", "3", "5", "7", "9", "5", "6", "7", "8", "9"};
  assert_store_is(&store, result3);

  store.deleteAll();
}

QUIZ_CASE(calculation_display_exact_approximate) {
  Shared::GlobalContext globalContext;
  CalculationStore store;

  store.push("1+3/4", &globalContext);
  store.push("ans+2/3", &globalContext);
  ::Calculation::Calculation * lastCalculation = store.calculationAtIndex(1);
  quiz_assert(lastCalculation->shouldOnlyDisplayApproximateOutput(&globalContext) == false);
  quiz_assert(strcmp(lastCalculation->exactOutputText(),"29/12") == 0);

  store.push("ans+0.22", &globalContext);
  lastCalculation = store.calculationAtIndex(2);
  quiz_assert(lastCalculation->shouldOnlyDisplayApproximateOutput(&globalContext) == true);
  quiz_assert(strcmp(lastCalculation->approximateOutputText(),"2.6366666666667") == 0);

  store.deleteAll();
  store.push("1/2", &globalContext);
  lastCalculation = store.calculationAtIndex(1);
  quiz_assert(lastCalculation->exactAndApproximateDisplayedOutputsAreEqual(&globalContext) == ::Calculation::Calculation::EqualSign::Equal);
  quiz_assert(lastCalculation->shouldOnlyDisplayExactOutput() == false);
  quiz_assert(lastCalculation->shouldOnlyDisplayApproximateOutput(&globalContext) == false);

  store.deleteAll();
  store.push("1/3", &globalContext);
  lastCalculation = store.calculationAtIndex(1);
  quiz_assert(lastCalculation->exactAndApproximateDisplayedOutputsAreEqual(&globalContext) == ::Calculation::Calculation::EqualSign::Approximation);
  quiz_assert(lastCalculation->shouldOnlyDisplayExactOutput() == false);
  quiz_assert(lastCalculation->shouldOnlyDisplayApproximateOutput(&globalContext) == false);

  store.deleteAll();
  store.push("1/0", &globalContext);
  lastCalculation = store.calculationAtIndex(1);
  quiz_assert(lastCalculation->shouldOnlyDisplayExactOutput() == true);
  quiz_assert(strcmp(lastCalculation->approximateOutputText(),"undef") == 0);

  store.deleteAll();
  store.push("2x-x", &globalContext);
  lastCalculation = store.calculationAtIndex(1);
  quiz_assert(lastCalculation->shouldOnlyDisplayExactOutput() == true);
  quiz_assert(strcmp(lastCalculation->exactOutputText(),"x") == 0);

  store.deleteAll();
  store.push("[[1,2,3]]", &globalContext);
  lastCalculation = store.calculationAtIndex(1);
  quiz_assert(lastCalculation->shouldOnlyDisplayExactOutput() == false);
  quiz_assert(lastCalculation->shouldOnlyDisplayApproximateOutput(&globalContext) == true);

  store.deleteAll();
  store.push("[[1,x,3]]", &globalContext);
  lastCalculation = store.calculationAtIndex(1);
  quiz_assert(lastCalculation->shouldOnlyDisplayExactOutput() == false);
  quiz_assert(lastCalculation->shouldOnlyDisplayApproximateOutput(&globalContext) == true);
  store.deleteAll();
}
