#include "tree_reference.h"

#if 0
void TreeReference::addOperand(TreeReference<TreeNode> t) {
  // At this point, e has been copied.
  // We can therefore pilfer its node!
  Node * n = node();
  pool->move(
      t->node(),
      t->next()
      );
  node->stealAsOperand(t->node());
}
#endif
