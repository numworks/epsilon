#include "float_node.h"
#include "addition_node.h"
#include <stdio.h>

Addition buildAddition() {
  Float smallFloat(0.2f);
  Float bigFloat(3.4f);
  Addition a(smallFloat, bigFloat);
  TreePool::sharedPool()->log();
  return a;
}

int main() {
  Addition a = buildAddition();
  float result = a.approximate();
  Float smallFloat(1.3f);
  a.replaceChildAtIndex(0, smallFloat);
  float result2 = a.approximate();
  TreePool::sharedPool()->log();
  return 0;
}
