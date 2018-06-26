#include "float_node.h"
#include "char_layout_node.h"
#include "horizontal_layout_node.h"
#include "addition_node.h"
#include "cursor.h"
#include <stdio.h>

AdditionRef buildAddition() {
  FloatRef smallFloat(0.2f);
  FloatRef bigFloat(3.4f);
  AdditionRef a(smallFloat, bigFloat);
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
  CharLayoutRef aChar('c');
  CharLayoutRef bChar('b');
  TreePool::sharedPool()->log();

  HorizontalLayoutRef h(aChar, bChar);
  TreePool::sharedPool()->log();

  LayoutCursor cursor = h.childAtIndex(1).cursor();
 // LayoutCursor cursor2 = aChar.cursor();
  /*cursor.log();
  bool recompute = false;
  cursor.moveLeft(&recompute);
  cursor.log();
  cursor.moveLeft(&recompute);
  cursor.log();*/
  TreePool::sharedPool()->log();

  /*cursor.log();
  bool recompute = false;
  cursor.moveLeft(&recompute);
  cursor.log();
  cursor.moveRight(&recompute);
  cursor.log();*/
  return 0;
}
