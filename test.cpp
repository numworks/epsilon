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

void testPoolExpressionAllocationFail() {
  printf("Pool expression allocation fail test\n");

  ExpressionRef::failedAllocationNode();
  assert(TreePool::sharedPool()->numberOfNodes() == 1);

  // Fill the pool for size 256
  FloatRef f1(0.0f);
  FloatRef f2(1.0f);
  AdditionRef a1(f1, f2);
  float result1 = a1.approximate();
  assert(result1 == 1);

  FloatRef f3(2.0f);
  FloatRef f4(3.0f);
  FloatRef f5(4.0f);
  FloatRef f6(5.0f);
  FloatRef f7(6.0f);
  FloatRef f8(7.0f);
  FloatRef f9(8.0f);

  // Allocation fail
  FloatRef f11(10.0f);
  AdditionRef a(f11, f3);
  float result = a.approximate();
  assert(result == -1);
  assert(ExpressionRef::failedAllocationNode()->retainCount() == 3);

  f1.replaceWith(f11);
  float result2 = a1.approximate();
  assert(result2 == 0);
}

void testPoolExpressionAllocationFail2() {
  printf("Pool expression allocation fail second test\n");

  ExpressionRef::failedAllocationNode();
  assert(TreePool::sharedPool()->numberOfNodes() == 1);

  // Fill the pool for size 256
  FloatRef f1(0.0f);
  FloatRef f2(1.0f);
  AdditionRef a1(f1, f2);
  float result1 = a1.approximate();
  assert(result1 == 1);

  FloatRef f3(2.0f);
  FloatRef f4(3.0f);
  AdditionRef a2(f3, f4);
  float result2 = a2.approximate();
  assert(result2 == 5);

  FloatRef f5(4.0f);
  FloatRef f6(5.0f);
  FloatRef f7(6.0f);
  FloatRef f8(7.0f);
  // Allocation fail
  FloatRef f9(8.0f);
  FloatRef f10(8.0f);

  printf("\n");
  TreePool::sharedPool()->log();
  printf("\n");
  f1.replaceWith(f9);
  result1 = a1.approximate();
  assert(result1 == -1);

  TreePool::sharedPool()->log();
  printf("\n");
  f3.replaceWith(f10);
  result2 = a2.approximate();
  assert(result2 == -1);

  printf("\n");
  TreePool::sharedPool()->log();
  result1 = a1.approximate();
  printf("a1 number children %d\n", a1.numberOfChildren());
  printf("a1 %f\n", result1);
  assert(result1 == -1);
}

void testPoolLayoutAllocationFail() {
  printf("Pool layout allocation fail test\n");

  // Fill the pool for size 256
  CharLayoutRef char1('a');
  LayoutRef::failedAllocationNode();
  CharLayoutRef char2('b');
  CharLayoutRef char3('a');
  CharLayoutRef char4('b');
  CharLayoutRef char5('a');
  CharLayoutRef char6('b');
  CharLayoutRef char7('a');
  CharLayoutRef char8('b');
  CharLayoutRef char9('a');
  CharLayoutRef char10('b');

  // Allocation fail
  CharLayoutRef char11('a');
  /*Expression e = ;
    e.simplify*/
}

typedef void (test)();
void runTest(test t) {
  // TODO add aserts on the pool size once we decide to create allocationFailureNodesFromTheStart
  t();
}

int main() {
  printf("\n*******************\nStart running tests\n*******************\n\n");
  runTest(testAddition);
  runTest(testPoolEmpties);
  runTest(testCursorCreateAndRetain);
  runTest(testCursorMoveLeft);
  //runTest(testPoolExpressionAllocationFail);
  runTest(testPoolExpressionAllocationFail2);
  printf("\n*******************\nEnd of tests\n*******************\n\n");
  return 0;
}
