#include "float_node.h"
#include "addition_node.h"
#include <stdio.h>


int main() {
  printf("Hello\n");

  ExpressionNode::Pool()->log();

  Float e(0.2f);

  ExpressionNode::Pool()->log();

  if (true) {
    Expression j = e;

    ExpressionNode::Pool()->log();

    Float m(3.4f);

    ExpressionNode::Pool()->log();
  }

  ExpressionNode::Pool()->log();

  Addition a(e, e);

  ExpressionNode::Pool()->log();

  /*
  Expression e = Parse("1+1");
  Expression j = e;
  Expression f = Addition(e, Parse("5"));
  f.removeChildAtIndex(0);
  */
#if 0
  TreeNodePool pool;
  TreeNodePool * p = &pool;

  pool.log();

  TreeNode * t1 = new (pool) AdditionNode();

  TreeNode * t1 = TreeNode::Create(p, 11);
  TreeNode * t2 = TreeNode::Create(p, 12);
  TreeNode * t3 = TreeNode::Create(p, 13);

  TreeNode * t4 = AddNode::Create(t1, t3);
  //TreeNode * t4 = new(p) IntNode(4);
  //
  t4->createChild(Addition()

  pool.log();

  pool.move(t1, t3);

  pool.log();
#endif

  return 0;
}


#if 0

static inline swap(uint32_t * a, uint32_t * b) {
  uint32_t tmp = *a;
  *a = *b;
  *b = tmp;
}

void insert(char * source, char * destination, size_t length) {
  assert(length % 4 == 0);
  assert(source % 4 == 0);
  assert(destination % 4 == 0);
  uint32_t * src = reinterpret_cast<uint32_t *>(source);
  uint32_t * dst = reinterpret_cast<uint32_t *>(destination);
  size_t len = length/4;

  if (dst < src) {
    if (src - dst <= len) {
      uint32_t * srcPointer = src;
      uint32_t * dstPointer = dst;
      while (dstPointer != src) {
        swap(srcPointer, dstPointer);
        srcPointer++;
        dstPointer++;
        if (srcPointer == src + len) {
          srcPointer = src;
        }
      }
    }
  }
}

void TreeNodePool::move(TreeNode * source, TreeNode * destination) {
  if (source == destination) {
    return;
  }
  insert(source, destination, source->deepSize());
}

bool TreeNode::hasVariableNumberOfChildren() const {
  return false;
}
#endif
