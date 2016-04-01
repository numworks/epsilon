#include <poincare/integer.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int __print_ast(Expression* expression, int offset) {
  // TODO print my current node.
  char txt[offset+1] = {' '};
  text[offset] = '\0';
  cout << txt;

  // Print my children.
  for (int i(0); i<expression->m_operands(); i++) {
    int tmp = __print_ast(expression->operand(i), offset);
    offset += tmp;
  }

  return 3;
}
