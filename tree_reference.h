#ifndef TREE_REFERENCE_H
#define TREE_REFERENCE_H

#include "tree_pool.h"
#include <stdio.h>

static inline int min(int i, int j) { return i < j ? i : j; }
static inline int max(int i, int j) { return i > j ? i : j; }

template <class T>
class TreeReference {
public:
  TreeReference(const TreeReference & tr) {
    int trNodeIdentifier = tr.identifier();
    TreeNode * nodeCopy = TreePool::sharedPool()->deepCopy(TreePool::sharedPool()->node(trNodeIdentifier));
    m_identifier = nodeCopy->identifier();
  }

  ~TreeReference() {
    assert(node());
    assert(node()->identifier() == m_identifier);
    node()->release();
  }

  operator TreeReference<TreeNode>() const {
    // TODO: make sure this is kosher
    // static_assert(sizeof(ExpressionReference<T>) == sizeof(ExpressionReference<ExpressionNode>), "All ExpressionReference are supposed to have the same size");
    return *(reinterpret_cast<const TreeReference<TreeNode> *>(this));
  }

  T * node() const {
    // TODO: Here, assert that the node type is indeed T
    return static_cast<T*>(TreePool::sharedPool()->node(m_identifier));
  }

  int identifier() const { return m_identifier; }

  // Hierarchy
  int numberOfChildren() const {
    return node()->numberOfChildren();
  }

  TreeReference<T> parent() const {
    return TreeReference(node()->parentTree());
  }

  TreeReference<T> childAtIndex(int i) const {
    return TreeReference(node()->child(i));
  }

  // Hierarchy operations

  void addChild(TreeReference<TreeNode> t) {
    TreeNode * deepCopy = TreePool::sharedPool()->deepCopy(t.node());
    TreePool::sharedPool()->move(deepCopy, node()->next());
  }

  void removeChild(TreeReference<TreeNode> t) {
    TreePool::sharedPool()->move(t.node(), TreePool::sharedPool()->last());
    t.node()->release();
  }

  void replaceWith(TreeReference<TreeNode> t) {
    parent().replaceChild(node()->indexOfChild(t.node()), t);
  }

  void replaceChildAtIndex(int oldChildIndex, TreeReference<TreeNode> newChild) {
    assert(oldChildIndex >= 0 && oldChildIndex < numberOfChildren());
    TreeReference<T> oldChild = childAtIndex(oldChildIndex);
    TreePool::sharedPool()->move(newChild.node(), oldChild.node()->next());
    newChild.node()->retain();
    TreePool::sharedPool()->move(oldChild.node(), TreePool::sharedPool()->last());
    oldChild.node()->release();
  }

  void swapChildren(int i, int j) {
    assert(i >= 0 && i < numberOfChildren());
    assert(j >= 0 && j < numberOfChildren());
    if (i == j) {
      return;
    }
    int firstChildIndex = min(i, j);
    int secondChildIndex = max(i, j);
    TreeReference<T> firstChild = childAtIndex(firstChildIndex);
    TreeReference<T> secondChild = childAtIndex(secondChildIndex);
    TreeNode * firstChildNode = firstChild.node();
    TreePool::sharedPool()->move(firstChildNode, secondChild.node()->next());
    TreePool::sharedPool()->move(secondChild.node(), firstChildNode);
  }

protected:
  TreeReference() {
    TreeNode * node = TreePool::sharedPool()->createTreeNode<T>();
    m_identifier = node->identifier();
  }

private:
  TreeReference(TreeNode * node) :
    m_identifier(node->identifier())
  {
    node->retain();
  }

  int m_identifier;
};

#endif
