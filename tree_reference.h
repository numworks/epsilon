#ifndef TREE_REFERENCE_H
#define TREE_REFERENCE_H

#include "tree_pool.h"
#include <stdio.h>

static inline int min(int i, int j) { return i < j ? i : j; }
static inline int max(int i, int j) { return i > j ? i : j; }

class Cursor;

template <typename T>
class TreeReference {
  friend class Cursor;
  template <typename U>
  friend class TreeReference;
  template <typename U>
  friend class ExpressionReference;
  template <typename U>
  friend class LayoutReference;
public:
  TreeReference(const TreeReference & tr) { setTo(tr); }
  TreeReference(TreeReference&& tr) { setTo(tr); }
  TreeReference& operator=(const TreeReference& tr) {
    setTo(tr);
    return *this;
  }
  TreeReference& operator=(TreeReference&& tr) {
    setTo(tr);
    return *this;
  }

  inline bool operator==(TreeReference<TreeNode> t) { return m_identifier == t.identifier(); }

  void setTo(const TreeReference & tr) {
    m_identifier = tr.identifier();
    TreePool::sharedPool()->node(m_identifier)->retain();
  }

  TreeReference<T> clone() const {
    TreeNode * nodeCopy = TreePool::sharedPool()->deepCopy(node());
    return TreeReference<T>(nodeCopy);
  }

  ~TreeReference() {
    if (m_identifier >= 0) {
      assert(node());
      assert(node()->identifier() == m_identifier);
      node()->release();
    }
  }

  bool isDefined() const { return m_identifier >= 0 && TreePool::sharedPool()->node(m_identifier) != nullptr; }

  int nodeRetainCount() const { return node()->retainCount(); }
  void incrementNumberOfChildren() { return node()->incrementNumberOfChildren(); }
  void decrementNumberOfChildren() { return node()->decrementNumberOfChildren(); }

  operator TreeReference<TreeNode>() const {
    return TreeReference<TreeNode>(this->node());
  }

  T * castedNode() const {
    // TODO: Here, assert that the node type is indeed T
    // ?? Might be allocation failure, not T
    return static_cast<T*>(TreePool::sharedPool()->node(m_identifier));
  }

  TreeNode * node() const {
    return TreePool::sharedPool()->node(m_identifier);
  }


  int identifier() const { return m_identifier; }

  // Hierarchy
  int numberOfChildren() const {
    return node()->numberOfChildren();
  }

  TreeReference<T> parent() const {
    return TreeReference(node()->parentTree());
  }

  TreeReference<T> treeChildAtIndex(int i) const {
    return TreeReference(node()->childTreeAtIndex(i));
  }

  // Hierarchy operations

  void addTreeChild(TreeReference<TreeNode> t) {
    t.node()->retain();
    TreePool::sharedPool()->move(t.node(), node()->next());
    node()->incrementNumberOfChildren();
  }

  void removeChild(TreeReference<TreeNode> t) {
    TreePool::sharedPool()->move(t.node(), TreePool::sharedPool()->last());
    t.node()->release();
  }

  void replaceWith(TreeReference<TreeNode> t) {
    TreeReference<TreeNode> p = parent();
    p.replaceChildAtIndex(p.node()->indexOfChildByIdentifier(identifier()), t);
  }

  void replaceChildAtIndex(int oldChildIndex, TreeReference<TreeNode> newChild) {
    // TODO decrement the children count of the new child parent
    TreeReference<TreeNode> p = newChild.parent();
    if (p.isDefined()) {
      p.decrementNumberOfChildren();
    }
    assert(oldChildIndex >= 0 && oldChildIndex < numberOfChildren());
    TreeReference<T> oldChild = treeChildAtIndex(oldChildIndex);
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
    TreeReference<T> firstChild = treeChildAtIndex(firstChildIndex);
    TreeReference<T> secondChild = treeChildAtIndex(secondChildIndex);
    TreeNode * firstChildNode = firstChild.node();
    TreePool::sharedPool()->move(firstChildNode, secondChild.node()->next());
    TreePool::sharedPool()->move(secondChild.node(), firstChildNode);
  }

protected:
  TreeReference() {
    TreeNode * node = TreePool::sharedPool()->createTreeNode<T>();
    m_identifier = node->identifier();
  }

  TreeReference(TreeNode * node) {
    if (node == nullptr) {
      m_identifier = -1;
    } else {
      m_identifier = node->identifier();
      node->retain();
    }
  }
  int m_identifier;
};

typedef TreeReference<TreeNode> TreeRef;

#endif
