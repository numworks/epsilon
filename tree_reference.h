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
    printf("TreeReference copy of %d\n", m_identifier);
    node()->retain();
  }

  ~TreeReference() {
    printf("TreeRef destroy of %d\n", m_identifier);
    assert(node()->m_identifier == m_identifier);
    node()->release();
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

  int identifier() const { return m_identifier; }

protected:
  TreeReference(TreePool * pool) :
   m_pool(pool)
  {
    TreeNode * node = new T();
    m_identifier = node->identifier();
    printf("TreeNode orig build of %d\n", m_identifier);
  }

private:
  TreeReference(TreePool * pool, TreeNode * node) :
    m_pool(pool),
    m_identifier(node->identifier())
    //m_cachedNode(node)
  {
    printf("TreeNode build of %d\n", m_identifier);
    node->retain();
  }


  TreePool * m_pool;
  int m_identifier;
  //TreeNode * m_cachedNode;
};

#endif
