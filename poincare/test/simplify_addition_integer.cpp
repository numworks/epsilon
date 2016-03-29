#include <quiz.h>
#include <poincare.h>
#include <assert.h>
#if ULGY_DEBUG_HACK
#include <stdio.h>
#endif

void dump_ast(Expression * e, int level);

void assert_simplifies_to(const char * input_string, const char * expected_string) {
  Expression * input = Expression::parse(input_string);
  assert(input != nullptr);
#if ULGY_DEBUG_HACK
  printf("Input = \n");
  dump_ast(input,0);
#endif
  Expression * simplified = input->simplify();
  assert(simplified != nullptr);
#if ULGY_DEBUG_HACK
  printf("Simplified = \n");
  dump_ast(simplified,0);
#endif
  Expression * expected = Expression::parse(expected_string);
  assert(expected != nullptr);
#if ULGY_DEBUG_HACK
  printf("Expected = \n");
  dump_ast(expected,0);
#endif
  assert(simplified->isIdenticalTo(expected));
  delete expected;
  delete simplified;
  delete input;
#if ULGY_DEBUG_HACK
  printf("----\n");
#endif
}

#if ULGY_DEBUG_HACK
void dump_ast(Expression * e, int level) {
  char expression_symbol;
  switch(e->type()) {
    case Expression::Type::Integer:
      expression_symbol = 'i';
      break;
    case Expression::Type::Addition:
      expression_symbol = '+';
      break;
    case Expression::Type::Power:
      expression_symbol = '^';
      break;
    case Expression::Type::Float:
      expression_symbol = 'f';
      break;
    case Expression::Type::Fraction:
      expression_symbol = '/';
      break;
    case Expression::Type::Product:
      expression_symbol = '*';
      break;
    case Expression::Type::Symbol:
      expression_symbol = 's';
      break;
    case Expression::Type::Subtraction:
      expression_symbol = '-';
      break;
    default:
      expression_symbol = '?';
      break;
  }
  char text[255];
  int cursor = 0;
  for (cursor=0; cursor<level; cursor++) {
    text[cursor] = ' ';
  }
  text[cursor++] = expression_symbol;
  text[cursor] = 0;
  printf("%s\n", text);
  for (int i=0; i<e->numberOfOperands(); i++) {
    dump_ast(e->operand(i), level+1);
  }
}
#endif

QUIZ_CASE(poincare_simplify_addition_integer) {
  assert_simplifies_to("3+0", "3");
  assert_simplifies_to("3+foo+2", "foo+5");
}
