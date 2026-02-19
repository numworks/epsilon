#include <omg/assertions.h>
#include <quiz.h>

/* Sets conditionHasBeenEvaluated to true and returns the condition. */
bool evaluateCondition(bool condition, bool* conditionHasBeenEvaluated) {
  quiz_assert(!*conditionHasBeenEvaluated);
  *conditionHasBeenEvaluated = true;
  return condition;
}

#define TEST_ASSERTION(Assertion, Condition, AssertionShouldEvaluateToTrue,    \
                       ConditionShouldBeEvaluated)                             \
  {                                                                            \
    bool conditionHasBeenEvaluated = false;                                    \
    quiz_assert(                                                               \
        Assertion(evaluateCondition(Condition, &conditionHasBeenEvaluated)) == \
        AssertionShouldEvaluateToTrue);                                        \
    quiz_assert(conditionHasBeenEvaluated == ConditionShouldBeEvaluated);      \
  }

QUIZ_CASE(omg_assertions) {
  TEST_ASSERTION(ASSERT, true, true, ASSERTIONS);
  TEST_ASSERTION(DANGER_ASSERT, true, true, ASSERTIONS);
  TEST_ASSERTION(DEBUG_ASSERT, true, true, (ASSERTIONS && DEBUG));
  TEST_ASSERTION(NON_DEBUG_ASSERT, true, true, (ASSERTIONS && !DEBUG));
  TEST_ASSERTION(ASSERT_AND_RETURN, true, true, true);

#if !ASSERTIONS
  TEST_ASSERTION(ASSERT, false, true, false);
  TEST_ASSERTION(DANGER_ASSERT, false, true, false);
  TEST_ASSERTION(DEBUG_ASSERT, false, true, false);
  TEST_ASSERTION(NON_DEBUG_ASSERT, false, true, false);
  TEST_ASSERTION(ASSERT_AND_RETURN, false, false, true);
#endif

#if !DEBUG
  TEST_ASSERTION(DEBUG_ASSERT, false, true, false);
#else
  TEST_ASSERTION(NON_DEBUG_ASSERT, false, true, false);
#endif

  // The following assertions should raise if uncommented and executed.
#if ASSERTIONS
  // ASSERT(false);
  // ASSERT_AND_RETURN(false);
  // DANGER_ASSERT(false);
#if DEBUG
  // DEBUG_ASSERT(false);
#else
  // NON_DEBUG_ASSERT(false);
#endif
#endif
}
