#ifndef POINCARE_TEST_SIMPLIFY_UTILS_H
#define POINCARE_TEST_SIMPLIFY_UTILS_H

/* Tests that the first expression simplifies to the second. */
bool simplifies_to(const char * input_string, const char * expected_string);

/* Tests that the first expression is identical to the second. */
bool identical_to(const char * input_string, const char * expected_string);

/* Tests that the first and the second expressions simplify to the same expression. */
bool equivalent_to(const char * input_string, const char * expected_string);

#endif // POINCARE_TEST_SIMPLIFY_UTILS_H
