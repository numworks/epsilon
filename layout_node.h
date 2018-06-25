#ifndef LAYOUT_NODE_H
#define LAYOUT_NODE_H

#include "tree_node.h"

class LayoutNode : public TreeNode {
public:
  /* Hierarchy */
  LayoutNode * parent() const { return static_cast<LayoutNode *>(parentTree()); }

  /* Rendering */
  void draw();
  int origin();
  int absoluteOrigin();

  LayoutNode * childAtIndex(int i) { return static_cast<LayoutNode *>(childTreeAtIndex(i)); }

protected:
  class Iterator {
    public:
    Iterator(LayoutNode * node) : m_node(node) {}
    LayoutNode * operator*() { return m_node; }
    bool operator!=(const Iterator& it) const { return (m_node != it.m_node); }
  protected:
    LayoutNode * m_node;
  };
  class DirectChildren {
  public:
    DirectChildren(LayoutNode * node) : m_node(node) {}
    class Iterator : public LayoutNode::Iterator {
    public:
      using LayoutNode::Iterator::Iterator;
      Iterator & operator++() {
        m_node = static_cast<LayoutNode *>(m_node->nextSibling());
        return *this;
      }
    };
    Iterator begin() const { return Iterator(static_cast<LayoutNode *>(m_node->next())); }
    Iterator end() const { return Iterator(static_cast<LayoutNode *>(m_node->nextSibling())); }
  private:
    LayoutNode * m_node;
  };
  DirectChildren children() { return DirectChildren(this); }

private:
  virtual void render() {};
};

#endif
