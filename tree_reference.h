#ifndef TREE_REFERENCE_H
#define TREE_REFERENCE_H

#include "tree_pool.h"
#include <new>

#include <stdio.h>

template <class T>
class TreeReference {
public:
  TreeReference(const TreeReference & tr) :
    m_pool(tr.m_pool),
    m_identifier(tr.m_identifier)
  {
    printf("TreeReference copy\n");
    node()->retain();
  }

  ~TreeReference() {
    TreeNode * node = this->node();
    node->release();
    if (node->retainCount() == 0) {
      printf("Discarding node %d(%p)\n", node->identifier(), node);
      // Here the static_cast should fail if T is not a subclass of TreeNode
      size_t deepNodeSize = node->deepSize();
#if 0
      // Here, if needed, call reclaimIdentifier
      for (TreeNode * child : node->depthFirstChildren()) {
        m_pool->reclaimIdentifier(child->identifier());
      }
      m_pool->reclaimIdentifier(node->identifier());
#endif
      static_cast<T*>(node)->~T();
      m_pool->dealloc(node, deepNodeSize);
    }
  }

  int numberOfChildren() const {
    return node()->numberOfChildren();
  }

  TreeReference<T> childAtIndex(int i) const {
    return TreeReference(node()->childAtIndex(i));
  }

  void addOperand(TreeReference<TreeNode> t) {
    assert(t.m_pool == m_pool);
    /*
    m_pool->move(
      t->node(),
      t->next()
    );
    */
  }

protected:
  TreeReference(TreePool * pool) :
   m_pool(pool)
  {
    int identifier = pool->generateIdentifier();
    void * area = pool->alloc(sizeof(T));
    TreeNode * n = new (area) T(identifier);
    m_identifier = n->identifier();
    assert(m_identifier == identifier);
    //m_cachedNode = n;
  }

  T * node() const {
    // TODO: Here, assert that the node type is indeed T
    return static_cast<T*>(m_pool->node(m_identifier));
  }
private:
  TreeReference(TreePool * pool, TreeNode * node) :
    m_pool(pool),
    m_identifier(node->identifier())
    //m_cachedNode(node)
  {
    node->retain();
  }


  TreePool * m_pool;
  int m_identifier;
  //TreeNode * m_cachedNode;
};

#endif
