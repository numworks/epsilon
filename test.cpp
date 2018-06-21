#include "float_node.h"
#include "addition_node.h"
#include <stdio.h>

void poolTest() {
  TreePool::sharedPool()->log();

  FloatNode * f = new FloatNode();
  f->setFloat(0.1f);

  FloatNode * g = new FloatNode();
  g->setFloat(1.1f);

  AdditionNode * h = new AdditionNode();

  TreePool::sharedPool()->log();

  delete g;

  TreePool::sharedPool()->log();
}

Expression buildAddition() {
  printf("\n\n-----------------------------\n");
  printf("CODE: Float smallFloat(0.2f);\n\n");
  Float smallFloat(0.2f);

  printf("\n\n-----------------------------\n");
  printf("CODE: Float bigFloat(3.4f);\n\n");
  Float bigFloat(3.4f);

  printf("\n\n-----------------------------\n");
  printf("CODE: Addition a(smallFloat, bigFloat);\n\n");
  Addition a(smallFloat, bigFloat);

  TreePool::sharedPool()->log();
  printf("\n\n-----------------------------\n");
  printf("CODE: return a;\n\n");
  return a;
}

int main() {
  printf("\n\n-----------------------------\n");
  printf("CODE: Expression a = buildAddition();\n\n");
  Expression a = buildAddition();
  printf("HAS RETURNED\n");
  TreePool::sharedPool()->log();



  /*
  Expression e = Parse("1+1");
  Expression j = e;
  Expression f = Addition(e, Parse("5"));
  f.removeChildAtIndex(0);
  */


  return 0;
}
