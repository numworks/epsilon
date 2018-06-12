#ifndef TREE_POOL_H
#define TREE_POOL_H

#include <stddef.h>

#include "tree_node.h"

class TreePool {
public:
  TreePool() : m_lastIdentifier(0), m_cursor(m_buffer) {}

  int generateIdentifier() {
    assert(node(m_lastIdentifier) == nullptr);
    return m_lastIdentifier++;
  }
  void reclaimIdentifier(int identifier) {
  }

  void * alloc(size_t size);
  void dealloc(void * ptr, size_t size);

  TreeNode * node(int identifier) const;
#if TREE_LOGGING
  void log();
#endif

private:

  TreeNode::DepthFirst::Iterator begin() const { return TreeNode::DepthFirst::Iterator(reinterpret_cast<TreeNode *>(const_cast<char *>(m_buffer))); }
  TreeNode::DepthFirst::Iterator end() const { return TreeNode::DepthFirst::Iterator(reinterpret_cast<TreeNode *>(const_cast<char *>(m_cursor))); }

  int m_lastIdentifier;
  char * m_cursor;
  char m_buffer[256];
};

#if 0
class TreePool {
public:
  TreePool() :
    m_identifier(0),
    m_cursor(m_data)
  {
  }

  class Slot {
  public:
    Slot(int identifier, void * area) :
      m_identifier(identifier), m_area(area) {}
    int identifier() const { return m_identifier; }
    void * area() const { return m_area; }
  private:
    int m_identifier;
    void * m_area;
    int m_size;
  };

  Slot alloc(size_t size);
  void dealloc(Slot s, size_t size);

  //TreeNode * node(int identifier) const;
  TreeNode * alloc(size_t size) {

  }
  void dealloc(TreeNode * node);

  void move(TreeNode * source, TreeNode * destination);
//#define TREE_LOGGING 1
#if TREE_LOGGING
  void log();
#endif
 private:
  int m_identifier;
  char * m_cursor;
  char m_data[200];
};
#endif

#endif
