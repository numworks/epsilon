#include <poincare.h>
#include <assert.h>

#if POINCARE_TESTS_PRINT_EXPRESSIONS
#include "../src/expression_debug.h"
#include <iostream>
using namespace std;
#endif

bool simplifies_to(const char * input_string, const char * expected_string) {
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  cout << "---- Simplification Run ----"  << endl;
  cout << input_string << " -> " << expected_string << endl;
#endif
  Expression * input = Expression::parse(input_string);
  assert(input != nullptr);
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  cout << "Input = " << endl;
  print_expression(input);
#endif

  Expression * simplified = input->simplify();
  assert(simplified != nullptr);
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  cout << "Simplified = " << endl;
  print_expression(simplified);
#endif

  Expression * expected = Expression::parse(expected_string);
  assert(expected != nullptr);
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  cout << "Expected = " << endl;
  print_expression(expected);
#endif

  bool isIdentical = simplified->isIdenticalTo(expected);

  delete expected;
  delete simplified;
  delete input;

  return isIdentical;
}

bool identical_to(const char * input_string, const char * expected_string) {
  Expression * input = Expression::parse(input_string);
  assert(input != nullptr);
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  cout << "Input = " << endl;
  print_expression(input);
#endif

  Expression * expected = Expression::parse(expected_string);
  assert(expected != nullptr);
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  cout << "Expected = " << endl;
  print_expression(expected);
#endif

  bool isIdentical = input->isIdenticalTo(expected);

  delete expected;
  delete input;

  return isIdentical;
}

bool equivalent_to(const char * input_string, const char * expected_string) {
  Expression * input = Expression::parse(input_string);
  assert(input != nullptr);
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  cout << "Input = " << endl;
  print_expression(input);
#endif

  Expression * expected = Expression::parse(expected_string);
  assert(expected != nullptr);
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  cout << "Expected = " << endl;
  print_expression(expected);
#endif

  bool isEquivalent = input->isEquivalentTo(expected);

  delete expected;
  delete input;

  return isEquivalent;
}
