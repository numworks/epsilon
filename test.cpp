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
  Float smallFloat(0.2f);
  Float bigFloat(3.4f);

  Addition a(smallFloat, bigFloat);

  TreePool::sharedPool()->log();
  printf("EXITING\n");
  printf("smallFloat ref = %d\n", smallFloat.identifier());
  printf("bigFloat ref = %d\n", bigFloat.identifier());
  return a;
}

int main() {
  printf("Hello\n");
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
