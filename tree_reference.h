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
#if 0
    if (node->retainCount() == 0) {


      // Here we deal with removing a node.
      // It's not as easy as one may think.
      // -> When a node is not needed anymore


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
#endif
  }

  operator TreeReference<TreeNode>() const {
    // TODO: make sure this is kosher
    // static_assert(sizeof(ExpressionReference<T>) == sizeof(ExpressionReference<ExpressionNode>), "All ExpressionReference are supposed to have the same size");
    return *(reinterpret_cast<const TreeReference<TreeNode> *>(this));
  }

  int numberOfChildren() const {
    return node()->numberOfChildren();
  }

  TreeReference<T> childAtIndex(int i) const {
    return TreeReference(node()->childAtIndex(i));
  }

  void addChild(TreeReference<TreeNode> t) {
    //assert(t.m_pool == m_pool);
    //t.node()->retain();
    TreeNode * deepCopy = m_pool->deepCopy(t.node());
    m_pool->move(
      deepCopy,
      node()->next()
    );
  }

  void removeChild(TreeReference<TreeNode> t) {
    m_pool->move(
        t.node(),
        m_pool->last()
    );
    t.node()->release();
  }

  T * node() const {
    // TODO: Here, assert that the node type is indeed T
    return static_cast<T*>(m_pool->node(m_identifier));
  }

protected:
  TreeReference(TreePool * pool) :
   m_pool(pool)
  {
    TreeNode * node = new T();
    m_identifier = node->identifier();
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
