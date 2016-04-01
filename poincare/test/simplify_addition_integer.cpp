#include <quiz.h>
#include <poincare.h>
#include <assert.h>

#ifdef DEBUG
#include <iostream>
using namespace std;
#endif

void assert_simplifies_to(const char * input_string, const char * expected_string) {
  //Expression* tab[3] = {new Integer(1), new Integer(2), new Integer(3)};
  //Expression* input = new Addition(tab, 3, false);
  Expression * input = Expression::parse(input_string);
  assert(input != nullptr);
#ifdef DEBUG
  cout << "Input = " << endl;
  PRINT_AST(input);
#endif
  Expression * simplified = input->simplify();
  assert(simplified != nullptr);
#ifdef DEBUG
  cout << "Simplified = " << endl;
  PRINT_AST(simplified);
#endif
  Expression * expected = Expression::parse(expected_string);
  assert(expected != nullptr);
#ifdef DEBUG
  cout << "Expected = " << endl;
  PRINT_AST(expected);
#endif
  assert(simplified->isIdenticalTo(expected));
  delete expected;
  delete simplified;
  delete input;
#ifdef DEBUG
  cout << "----" << endl;
#endif
}

QUIZ_CASE(poincare_simplify_addition_integer) {
  //assert_simplifies_to("2+2", "4");
  assert_simplifies_to("2*2*2*2*2*2", "6");
}
