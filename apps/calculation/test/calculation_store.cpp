#include <quiz.h>
#include <string.h>
#include <assert.h>
#include "../calculation_store.h"

using namespace Poincare;
using namespace Calculation;

QUIZ_CASE(calculation_store) {
  GlobalContext globalContext;
  CalculationStore store;
  assert(CalculationStore::k_maxNumberOfCalculations == 10);
  for (int i = 0; i < CalculationStore::k_maxNumberOfCalculations; i++) {
    char text[2] = {(char)(i+'0'), 0};
    store.push(text, &globalContext);
    assert(store.numberOfCalculations() == i+1);
  }
  /* Store is now {0, 1, 2, 3, 4, 5, 6, 7, 8, 9} */
  for (int i = 0; i < CalculationStore::k_maxNumberOfCalculations; i++) {
    assert(store.calculationAtIndex(i)->input()->approximate(globalContext) == i);
  }
  store.push("10", &globalContext);
  /* Store is now {1, 2, 3, 4, 5, 6, 7, 8, 9, 10} */
  assert(store.calculationAtIndex(9)->input()->approximate(globalContext) == 10);
  assert(store.calculationAtIndex(0)->input()->approximate(globalContext) == 1);
  for (int i = 0; i < CalculationStore::k_maxNumberOfCalculations; i++) {
    char text[2] = {(char)(i+'0'), 0};
    store.push(text, &globalContext);
    assert(store.numberOfCalculations() == 10);
  }
  /* Store is now {0, 1, 2, 3, 4, 5, 6, 7, 8, 9} */
  for (int i = 9; i > 0; i = i-2) {
   store.deleteCalculationAtIndex(i);
  }
  /* Store is now {0, 2, 4, 6, 8} */
  for (int i = 0; i < 5; i++) {
    assert(store.calculationAtIndex(i)->input()->approximate(globalContext) == 2.0f*i);
  }
  for (int i = 5; i < CalculationStore::k_maxNumberOfCalculations; i++) {
    char text[3] = {(char)(i+'0'), 0};
    store.push(text, &globalContext);
    assert(store.numberOfCalculations() == i+1);
  }
  /* Store is now {0, 2, 4, 6, 8, 5, 6, 7, 8, 9} */
}
