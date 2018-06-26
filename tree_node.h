#ifndef TREE_NODE_H
#define TREE_NODE_H

#include <assert.h>
#include <stddef.h>
#include <strings.h>

#define TREE_LOGGING 1

#include <stdio.h>

/* What's in a TreeNode, really?
 *  - a vtable
 *  - an identifier
 *  - a reference counter
 */

class TreeNode {
//  friend class TreeReference<T>;
  friend class TreePool;
public:
  virtual ~TreeNode() {}

  // Attributes
  virtual size_t size() const = 0;
  int identifier() const { return m_identifier; }
  int retainCount() const { return m_referenceCounter; }
#if TREE_LOGGING
  virtual const char * description() const {
    return "UNKNOWN";
  }
#endif

  // Node operations
  void retain() { m_referenceCounter++; }
  void release();
  void rename(int identifier) {
    m_identifier = identifier;
    m_referenceCounter = 1;
  }

  // Hierarchy
  TreeNode * parentTree() const;
  TreeNode * editableRootTree();
  virtual int numberOfChildren() const = 0;
  int numberOfDescendants(bool includeSelf) const;
  TreeNode * childTreeAtIndex(int i) const;
  int indexOfChildByIdentifier(int childID) const;
  int indexOfChild(const TreeNode * child) const;
  bool hasChild(const TreeNode * child) const;
  bool hasAncestor(const TreeNode * node, bool includeSelf) const;
  bool hasSibling(const TreeNode * e) const;

  class Iterator {
    public:
    Iterator(const TreeNode * node) : m_node(const_cast<TreeNode *>(node)) {}
    TreeNode * operator*() { return m_node; }
    bool operator!=(const Iterator& it) const { return (m_node != it.m_node); }
  protected:
    TreeNode * m_node;
  };

  class Direct {
  public:
    Direct(const TreeNode * node) : m_node(const_cast<TreeNode *>(node)) {}
    class Iterator : public TreeNode::Iterator {
    public:
      using TreeNode::Iterator::Iterator;
      Iterator & operator++() {
        m_node = m_node->nextSibling();
        return *this;
      }
    };
    Iterator begin() const { return Iterator(m_node->next()); }
    Iterator end() const { return Iterator(m_node->nextSibling()); }
  private:
    TreeNode * m_node;
  };

  class DepthFirst {
  public:
    DepthFirst(const TreeNode * node) : m_node(const_cast<TreeNode *>(node)) {}
    class Iterator : public TreeNode::Iterator {
    public:
      using TreeNode::Iterator::Iterator;
      Iterator & operator++() {
        m_node = m_node->next();
        return *this;
      }
    };
    Iterator begin() const { return Iterator(m_node->next()); }
    Iterator end() const { return Iterator(m_node->nextSibling()); }
  private:
    TreeNode * m_node;
  };

  Direct directChildren() const { return Direct(this); }
  DepthFirst depthFirstChildren() const { return DepthFirst(this); }

  TreeNode * next() const {
    // Simple version would be "return this + 1;", with pointer arithmetics taken care of by the compiler.
    // Unfortunately, we want TreeNode to have a VARIABLE size
    return reinterpret_cast<TreeNode *>(reinterpret_cast<char *>(const_cast<TreeNode *>(this)) + size());
  }

protected:
  TreeNode() :
    m_identifier(-1),
    m_referenceCounter(1)
  {
  }

  TreeNode * nextSibling() const {
    int remainingNodesToVisit = numberOfChildren();
    TreeNode * node = const_cast<TreeNode *>(this)->next();
    while (remainingNodesToVisit > 0) {
      remainingNodesToVisit += node->numberOfChildren();
      node = node->next();
      remainingNodesToVisit--;
    }
    return node;
  }

  /*TreeNode * lastDescendant() const {
    TreeNode * node = const_cast<TreeNode *>(this);
    int remainingNodesToVisit = node->numberOfChildren();
    while (remainingNodesToVisit > 0) {
      node = node->next();
      remainingNodesToVisit--;
      remainingNodesToVisit += node->numberOfChildren();
    }
    return node;
  }*/

  TreeNode * lastChild() const {
    if (numberOfChildren() == 0) {
      return const_cast<TreeNode *>(this);
    }
    TreeNode * node = next();
    for (int i = 0; i < numberOfChildren() - 1; i++) {
      node = node->nextSibling();
    }
    return node;
  }

  size_t deepSize() const {
    // TODO: Error handling
    return
      reinterpret_cast<char *>(nextSibling())
      -
      reinterpret_cast<const char *>(this);
    ;
  }

private:
  int m_identifier;
  int m_referenceCounter;
};

#endif
