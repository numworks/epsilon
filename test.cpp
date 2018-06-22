#include "float_node.h"
#include "addition_node.h"
#include <stdio.h>

Addition buildAddition() {
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
  Addition a = buildAddition();
  float result = a.approximate();
  printf("a = %f \n", result);
  Float smallFloat(1.3f);
  a.replaceChildAtIndex(0, smallFloat);
  float result2 = a.approximate();
  printf("a = %f \n", result2);

  printf("HAS RETURNED\n");
  TreePool::sharedPool()->log();

  return 0;
}
