#include <quiz.h>
#include <string.h>
#include <assert.h>
#include "../calculation_store.h"

using namespace Poincare;
using namespace Calculation;

void assert_store_is(CalculationStore * store, const char * result[10]) {
  for (int i = 0; i < store->numberOfCalculations(); i++) {
    assert(strcmp(store->calculationAtIndex(i)->inputText(), result[i]) == 0);
  }
}

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
    assert(store.numberOfCalculations() == i+1);
  }
  /* Store is now {0, 2, 4, 6, 8, 5, 6, 7, 8, 9} */
  const char * result3[10] = {"1", "3", "5", "7", "9", "5", "6", "7", "8", "9"};
  assert_store_is(&store, result3);
}
