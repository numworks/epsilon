#include <quiz.h>
#include <poincare.h>
#include <assert.h>

#if POINCARE_TESTS_PRINT_EXPRESSIONS
#include "../src/expression_debug.h"
#include <iostream>
using namespace std;
#endif

void assert_simplifies_to(const char * input_string, const char * expected_string) {
#if DUMP_EXPRESSIONS_TO_STD_OUT
  cout << "---- Simplification Run ----"  << endl;
  cout << input_string << " -> " << expected_string << endl;
#endif
  //Expression* tab[3] = {new Integer(1), new Integer(2), new Integer(3)};
  //Expression* input = new Addition(tab, 3, false);
  Expression * input = Expression::parse(input_string);
  assert(input != nullptr);
#if DUMP_EXPRESSIONS_TO_STD_OUT
  cout << "Input = " << endl;
  print_expression(input);
#endif

  Expression * simplified = input->simplify();
  assert(simplified != nullptr);
#if DUMP_EXPRESSIONS_TO_STD_OUT
  cout << "Simplified = " << endl;
  print_expression(simplified);
#endif

  Expression * expected = Expression::parse(expected_string);
  assert(expected != nullptr);
#if DUMP_EXPRESSIONS_TO_STD_OUT
  cout << "Expected = " << endl;
  print_expression(expected);
#endif

  assert(simplified->isIdenticalTo(expected));
  delete expected;
  delete simplified;
  delete input;
}

QUIZ_CASE(poincare_simplify_addition_integer) {
  //assert_simplifies_to("2+2", "4");
  assert_simplifies_to("2*2*2*2*2*2", "6");
}
