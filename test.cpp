#include "float_node.h"
#include "char_layout_node.h"
#include "addition_node.h"
#include "layout_cursor.h"
#include <stdio.h>

Addition buildAddition() {
  Float smallFloat(0.2f);
  Float bigFloat(3.4f);
  Addition a(smallFloat, bigFloat);
  TreePool::sharedPool()->log();
  return a;
}

int main() {
  /*Addition a = buildAddition();
  float result = a.approximate();
  Float smallFloat(1.3f);
  a.replaceChildAtIndex(0, smallFloat);
  float result2 = a.approximate();
  a.swapChildren(1,0);
  TreePool::sharedPool()->log();*/

  printf("\nCHAR LAYOUT\n");
  CharLayout aChar('a');
  TreePool::sharedPool()->log();
  LayoutCursor cursor(aChar.pointer());
  TreePool::sharedPool()->log();
  cursor.log();
  bool recompute = false;
  cursor.moveLeft(&recompute);
  cursor.log();
  cursor.moveRight(&recompute);
  cursor.log();
  return 0;
}
