#include <omg/assertions.h>
#include <quiz.h>

/* Sets conditionHasBeenEvaluated to true and returns the condition. */
bool evaluateCondition(bool condition, bool* conditionHasBeenEvaluated) {
  quiz_assert(!*conditionHasBeenEvaluated);
  *conditionHasBeenEvaluated = true;
  return condition;
}

#define TEST_ASSERTION(Assertion, Condition, ConditionShouldBeEvaluated)  \
  {                                                                       \
    bool conditionHasBeenEvaluated = false;                               \
    Assertion(evaluateCondition(Condition, &conditionHasBeenEvaluated));  \
    quiz_assert(conditionHasBeenEvaluated == ConditionShouldBeEvaluated); \
  }

QUIZ_CASE(omg_assertions) {
  TEST_ASSERTION(assert, true, ASSERTIONS);
  TEST_ASSERTION(DANGER_ASSERT, true, ASSERTIONS);
  TEST_ASSERTION(DEBUG_ASSERT, true, (ASSERTIONS && DEBUG));
  TEST_ASSERTION(NON_DEBUG_ASSERT, true, (ASSERTIONS && !DEBUG));
  TEST_ASSERTION(EVALUATE_AND_ASSERT, true, true);

#if !ASSERTIONS
  TEST_ASSERTION(assert, false, false);
  TEST_ASSERTION(DANGER_ASSERT, false, false);
  TEST_ASSERTION(DEBUG_ASSERT, false, false);
  TEST_ASSERTION(NON_DEBUG_ASSERT, false, false);
  TEST_ASSERTION(EVALUATE_AND_ASSERT, false, true);
#endif

#if !DEBUG
  TEST_ASSERTION(DEBUG_ASSERT, false, false);
#else
  TEST_ASSERTION(NON_DEBUG_ASSERT, false, false);
#endif

  // The following lines should raise if uncommented.
#if ASSERTIONS
  // assert(false);
  // EVALUATE_AND_ASSERT(false);
  // DANGER_ASSERT(false);
#if DEBUG
  // DEBUG_ASSERT(false);
#else
  // NON_DEBUG_ASSERT(false);
#endif
#endif
}
