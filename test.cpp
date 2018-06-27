#include "refs.h"
#include <stdio.h>

AdditionRef buildAddition() {
  FloatRef smallFloat(0.2f);
  FloatRef bigFloat(3.4f);
  AdditionRef a(smallFloat, bigFloat);
  assert(TreePool::sharedPool()->numberOfNodes() == 3);
  return a;
}

void testAddition() {
  printf("Addition test\n");
  assert(TreePool::sharedPool()->numberOfNodes() == 0);
  AdditionRef a = buildAddition();
  assert(TreePool::sharedPool()->numberOfNodes() == 3);

  float result = a.approximate();
  assert(result = 3.6f);

  FloatRef smallFloat(1.3f);
  a.replaceChildAtIndex(0, smallFloat);
  float result2 = a.approximate();
  assert(result2 == 4.7f);

  a.swapChildren(1,0);
  assert(a.childAtIndex(0).identifier() == 1);
  assert(a.childAtIndex(1).identifier() == 3);
}

void createNodes() {
  FloatRef smallFloat(0.2f);
  FloatRef bigFloat(3.4f);
  AdditionRef a(smallFloat, bigFloat);
  ExpressionRef e = a;
  ExpressionRef f = e;
}

void testPoolEmpties() {
  printf("Pool empties test\n");
  assert(TreePool::sharedPool()->numberOfNodes() == 0);
  createNodes();
  assert(TreePool::sharedPool()->numberOfNodes() == 0);
}

void testCursorCreateAndRetain() {
  printf("Cursor create and retain test\n");
  CharLayoutRef aChar('a');
  CharLayoutRef bChar('b');
  assert(aChar.identifier() == 0);
  assert(bChar.identifier() == 1);
  assert(aChar.nodeRetainCount() == 1);
  assert(bChar.nodeRetainCount() == 1);
  assert(strcmp(aChar.node()->description(), "Char a") == 0);
  assert(strcmp(bChar.node()->description(), "Char b") == 0);
  assert(TreePool::sharedPool()->numberOfNodes() == 2);

  HorizontalLayoutRef h(aChar, bChar);
  assert(aChar.identifier() == 0);
  assert(bChar.identifier() == 1);
  assert(h.identifier() == 2);
  assert(aChar.nodeRetainCount() == 2);
  assert(bChar.nodeRetainCount() == 2);
  assert(h.nodeRetainCount() == 1);
  assert(aChar == h.childAtIndex(0));
  assert(bChar == h.childAtIndex(1));

  LayoutCursor cursorA = aChar.cursor();
  assert(cursorA.layoutIdentifier() == aChar.identifier());
  assert(aChar.nodeRetainCount() == 3);
}

void testCursorMoveLeft() {
  printf("Cursor move left test\n");
  CharLayoutRef aChar('a');
  CharLayoutRef bChar('b');
  HorizontalLayoutRef h(aChar, bChar);

  LayoutCursor cursor = h.childAtIndex(1).cursor();
  assert(bChar.nodeRetainCount() == 3);
  assert(cursor.layoutIdentifier() == h.childAtIndex(1).identifier());

  bool recompute = false;
  assert(cursor.layoutIdentifier() == bChar.identifier());
  assert(cursor.position() == LayoutCursor::Position::Right);
  cursor.moveLeft(&recompute);
  assert(cursor.layoutIdentifier() == bChar.identifier());
  assert(cursor.position() == LayoutCursor::Position::Left);
  assert(bChar.nodeRetainCount() == 3);
  assert(aChar.nodeRetainCount() == 2);
  cursor.moveLeft(&recompute);
  assert(cursor.layoutIdentifier() == aChar.identifier());
  assert(cursor.position() == LayoutCursor::Position::Left);
  assert(aChar.nodeRetainCount() == 3);
}

void testPoolAllocationFail() {
  printf("Pool allocation fail test\n");

  // Fill the pool for size 256

  // Allocation fail
  assert(TreePool::sharedPool()->numberOfNodes() == 0);
  AllocationFailedExpressionRef a;
  assert(TreePool::sharedPool()->numberOfNodes() == 1);

  /*Expression e = ;
  e.simplify*/
}

typedef void (test)();
void runTest(test t) {
  assert(TreePool::sharedPool()->numberOfNodes() == 0);
  t();
  assert(TreePool::sharedPool()->numberOfNodes() == 0);
}

int main() {
  printf("\n*******************\nStart running tests\n*******************\n\n");
  runTest(testAddition);
  runTest(testPoolEmpties);
  runTest(testCursorCreateAndRetain);
  runTest(testCursorMoveLeft);
  runTest(testPoolAllocationFail);
  printf("\n*******************\nEnd of tests\n*******************\n\n");
  return 0;
}
