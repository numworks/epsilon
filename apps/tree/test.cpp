#include "refs.h"
#include <stdio.h>

namespace Test {

void logPool() {
  TreePool::sharedPool()->log();
}

void assert_expression_approximates_to(ExpressionRef e, float result) {
  float b = e.approximate();
  if (b > result) {
    assert(b - result < 0.000001f);
  } else {
    assert(result - b < 0.000001f);
  }
}

AdditionRef buildAddition() {
  int initialNumberOfNodes = TreePool::sharedPool()->numberOfNodes();
  FloatRef smallFloat(0.2f);
  FloatRef bigFloat(3.4f);
  AdditionRef a(smallFloat, bigFloat);
  assert(TreePool::sharedPool()->numberOfNodes() == initialNumberOfNodes + 3);
  return a;
}

void testAddition() {
#if TREE_LOG
  printf("Addition test\n");
#endif
  int initialNumberOfNodes = TreePool::sharedPool()->numberOfNodes();
  AdditionRef a = buildAddition();
  assert(TreePool::sharedPool()->numberOfNodes() == initialNumberOfNodes + 3);

  assert_expression_approximates_to(a, 3.6f);

  FloatRef smallFloat(1.3f);
  a.replaceChildAtIndex(0, smallFloat);

  assert_expression_approximates_to(a, 4.7f);

  int firstChildIdentifier = a.childAtIndex(0).identifier();
  int secondChildIdentifier = a.childAtIndex(1).identifier();
  a.swapChildren(1,0);
  assert(a.childAtIndex(0).identifier() == secondChildIdentifier);
  assert(a.childAtIndex(1).identifier() == firstChildIdentifier);
}

void createNodes() {
  FloatRef smallFloat(0.2f);
  FloatRef bigFloat(3.4f);
  AdditionRef a(smallFloat, bigFloat);
  ExpressionRef e = a;
  ExpressionRef f = e;
}

void testPoolEmpties() {
#if TREE_LOG
  printf("Pool empties test\n");
#endif
  int initialNumberOfNodes = TreePool::sharedPool()->numberOfNodes();
  createNodes();
  assert(TreePool::sharedPool()->numberOfNodes() == initialNumberOfNodes);
}

void testCursorCreateAndRetain() {
#if TREE_LOG
  printf("Cursor create and retain test\n");
#endif
  int initialNumberOfNodes = TreePool::sharedPool()->numberOfNodes();
  CharLayoutRef aChar('a');
  CharLayoutRef bChar('b');
  int aCharID = aChar.identifier();
  int bCharID = bChar.identifier();
  assert(bCharID = aCharID + 1);
  assert(aChar.nodeRetainCount() == 1);
  assert(bChar.nodeRetainCount() == 1);
  assert(strcmp(aChar.node()->description(), "Char a") == 0);
  assert(strcmp(bChar.node()->description(), "Char b") == 0);
  assert(TreePool::sharedPool()->numberOfNodes() == initialNumberOfNodes + 2);

  HorizontalLayoutRef h(aChar, bChar);
  assert(aChar.identifier() == aCharID);
  assert(bChar.identifier() == bCharID);
  assert(h.identifier() == bCharID + 1);
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
#if TREE_LOG
  printf("Cursor move left test\n");
#endif
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
#if TREE_LOG
  printf("Pool expression allocation fail test\n");
#endif

  // Fill the pool for size 256
  FloatRef f1(0.0f);
  FloatRef f2(1.0f);
  AdditionRef a1(f1, f2);

  assert_expression_approximates_to(a1, 1);

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

  assert_expression_approximates_to(a, -1);

  f1.replaceWith(f11);

  assert_expression_approximates_to(a1, -1);
}

void testPoolExpressionAllocationFail2() {
#if TREE_LOG
  printf("Pool expression allocation multiple fail test\n");
#endif

  // Fill the pool for size 256
  FloatRef f1(0.0f);
  FloatRef f2(1.0f);
  AdditionRef a1(f1, f2);

  assert_expression_approximates_to(a1, 1);

  FloatRef f3(2.0f);
  FloatRef f4(3.0f);
  AdditionRef a2(f3, f4);

  assert_expression_approximates_to(a2, 5);

  FloatRef f5(4.0f);
  FloatRef f6(5.0f);
  FloatRef f7(6.0f);
  FloatRef f8(7.0f);
  // Allocation fail
  FloatRef f9(8.0f);
  FloatRef f10(8.0f);

  f1.replaceWith(f9);

  assert_expression_approximates_to(a1, -1);

  f3.replaceWith(f10);

  assert_expression_approximates_to(a2, -1);
  assert_expression_approximates_to(a1, -1);
}

void testPoolExpressionAllocationFailOnImbricatedAdditions() {
#if TREE_LOG
  printf("Pool expression allocation fail second test\n");
#endif

  // Fill the pool for size 256
  FloatRef f1(0.0f);
  FloatRef f2(1.0f);
  AdditionRef a1(f1, f2);

  assert_expression_approximates_to(a1, 1);

  FloatRef f3(2.0f);
  AdditionRef a2(a1, f3);

  assert_expression_approximates_to(a2, 3);

  FloatRef f4(3.0f);
  FloatRef f5(4.0f);
  FloatRef f6(5.0f);
  FloatRef f7(6.0f);
  FloatRef f8(7.0f);
  // Allocation fail
  FloatRef f9(7.0f);
  f1.replaceWith(f9);

  assert_expression_approximates_to(a2, -1);

  a2.removeChild(a1);

  assert_expression_approximates_to(a2, 2);
}

void testStealOperand() {
#if TREE_LOG
  printf("Steal operand test\n");
#endif

  FloatRef f1(0.0f);
  FloatRef f2(1.0f);
  AdditionRef a1(f1, f2);

  assert_expression_approximates_to(a1, 1);

  FloatRef f3(2.0f);
  FloatRef f4(3.0f);
  AdditionRef a2(f4, f3);

  assert_expression_approximates_to(a2, 5);

  a1.addChild(f4);

  assert_expression_approximates_to(a1, 4);
  assert_expression_approximates_to(a2, 2);
}

void testSimplify() {
#if TREE_LOG
  printf("Simplify test\n");
#endif
  AdditionRef a(
      AdditionRef(
        FloatRef(0.0f),
        FloatRef(1.0f)),
      FloatRef(2.0f));

  assert_expression_approximates_to(a, 3);
  ExpressionRef b = a.deepReduce();
  assert_expression_approximates_to(a, 3);
  assert_expression_approximates_to(b, 3);
  assert(b.numberOfChildren() == 1);
  assert(b.childAtIndex(0).typedNode()->type() == ExpressionNode::Type::Float);
  assert(b.childAtIndex(0).typedNode()->approximate() == 3.0f);
}

void testChildSort() {
#if TREE_LOG
  printf("Child sort test\n");
#endif

  AdditionRef a(
      AdditionRef(
        FloatRef(1.0f),
        FloatRef(2.0f)),
      FloatRef(3.0f));
  a.addChild(FloatRef(0.0f));

  assert(a.childAtIndex(0).typedNode()->type() == ExpressionNode::Type::Float);
  assert(a.childAtIndex(1).typedNode()->type() == ExpressionNode::Type::Addition);
  assert(a.childAtIndex(2).typedNode()->type() == ExpressionNode::Type::Float);

  a.sortChildren();

  assert(a.childAtIndex(0).typedNode()->type() == ExpressionNode::Type::Float);
  assert(a.childAtIndex(1).typedNode()->type() == ExpressionNode::Type::Float);
  assert(a.childAtIndex(2).typedNode()->type() == ExpressionNode::Type::Addition);
}


void testPoolLayoutAllocationFail() {
#if TREE_LOG
  printf("Pool layout allocation fail test\n");
#endif

  // Fill the pool for size 256
  CharLayoutRef char1('a');
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
  assert(strcmp(char11.node()->description(), "Allocation Failed") == 0);
}

typedef void (test)();
void runTest(test t) {
  int initialNumberOfNodes = TreePool::sharedPool()->numberOfNodes();
  t();
  assert(TreePool::sharedPool()->numberOfNodes() == initialNumberOfNodes);
}

int main() {
#if TREE_LOG
  printf("\n*******************\nStart running tests\n*******************\n\n");
#endif
  assert(TreePool::sharedPool()->numberOfNodes() == 0);
  runTest(testAddition);
  runTest(testPoolEmpties);
  runTest(testCursorCreateAndRetain);
  runTest(testCursorMoveLeft);
  runTest(testPoolExpressionAllocationFail);
  runTest(testPoolExpressionAllocationFail2);
  runTest(testPoolExpressionAllocationFailOnImbricatedAdditions);
  runTest(testStealOperand);
  runTest(testSimplify);
  runTest(testChildSort);
  runTest(testPoolLayoutAllocationFail);
#if TREE_LOG
  printf("\n*******************\nEnd of tests\n*******************\n\n");
#endif
  return 0;
}

}
