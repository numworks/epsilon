#ifndef TREE_NODE_H
#define TREE_NODE_H

#include <assert.h>
#include <stddef.h>
#include <strings.h>

#define TREE_LOGGING 1

#include <stdio.h>

class TreeNode {
  //friend class TreeReference;
  // friend class TreePool;
public:
  virtual ~TreeNode() {
  }

  // Iterators

  class Iterator {
  public:
    Iterator(TreeNode * node) : m_node(node) {}
    TreeNode * operator*() { return m_node; }
    bool operator!=(const Iterator& it) const { return (m_node != it.m_node); }
  protected:
    TreeNode * m_node;
  };

  class Direct {
  public:
    Direct(TreeNode * node) : m_node(node) {}
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

  Direct directChildren() { return Direct(this); }

  class DepthFirst {
  public:
    DepthFirst(TreeNode * node) : m_node(node) {}
    class Iterator : public TreeNode::Iterator {
    public:
      using TreeNode::Iterator::Iterator;
      Iterator & operator++() {
        // printf("  Iterating from %d(%p) to %d(%p)\n", m_node->m_identifier, m_node, m_node->next()->m_identifier, m_node->next());
        m_node = m_node->next();
        return *this;
      }
    };
    Iterator begin() const { return Iterator(m_node->next()); }
    Iterator end() const { return Iterator(m_node->nextSibling()); }
  private:
    TreeNode * m_node;
  };

  DepthFirst depthFirstChildren() { return DepthFirst(this); }


  int identifier() const { return m_identifier; }

#if TREE_LOGGING
  virtual const char * description() const {
    return "UNKNOWN";
  }
#endif

  virtual size_t size() const {
    return sizeof(TreeNode);
  }

  void retain() {
    m_referenceCounter++;
  }
  void release();

  void rename(int identifier) {
    m_identifier = identifier;
    m_referenceCounter = 1;
  }

  int retainCount() {
    return m_referenceCounter;
  }

  virtual int numberOfChildren() const {
    return 0;
  }

  TreeNode * childAtIndex(int i) const {
    assert(i >= 0);
    assert(i < numberOfChildren());
    TreeNode * child = next();
    while (i>0) {
      child = child->nextSibling();
      assert(child != nullptr);
      i--;
    }
    return child;
  }

//private:

    // FIXME: Make this private
  TreeNode(int identifier) :
    m_identifier(identifier),
    m_referenceCounter(1)
  {
  }

  TreeNode * next() const {
    // Simple version would be "return this + 1;", with pointer arithmetics taken care of by the compiler.
    // Unfortunately, we want TreeNode to have a VARIABLE size
    return reinterpret_cast<TreeNode *>(reinterpret_cast<char *>(const_cast<TreeNode *>(this)) + size());
  }

  TreeNode * nextSibling() const {
    TreeNode * node = const_cast<TreeNode *>(this);
    int remainingNodesToVisit = 0;
    do {
      remainingNodesToVisit += node->numberOfChildren();
      node = node->next();
      remainingNodesToVisit--;
    } while (remainingNodesToVisit > 0);
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

//private:
  int m_identifier;
  int m_referenceCounter;
};

#endif
