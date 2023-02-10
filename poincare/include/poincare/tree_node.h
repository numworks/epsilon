#ifndef POINCARE_TREE_NODE_H
#define POINCARE_TREE_NODE_H

#include <assert.h>
#include <stddef.h>
#include <strings.h>
#include <stdint.h>
#if POINCARE_TREE_LOG
#include <ostream>
#include <iostream>
#endif
#include <poincare/helpers.h>
#include <poincare/preferences.h>

/* What's in a TreeNode, really?
 *  - a vtable pointer
 *  - an identifier
 *  - a parent identifier
 *  - a reference counter
 */

/* CAUTION: To make node operations faster, the pool needs all adresses and
 * sizes of TreeNodes to be a multiple of 4. */

namespace Poincare {

#if __EMSCRIPTEN__
/* Emscripten memory representation assumes loads and stores are aligned.
 * Because the TreePool buffer is going to store double values, Node addresses
 * have to be aligned on 8 bytes (provided that emscripten addresses are 8 bytes
 * long which ensures that v-tables are also aligned). */
typedef uint64_t AlignedNodeBuffer;
#else
/* Memory copies are done quicker on 4 bytes aligned data. We force the TreePool
 * to allocate 4-byte aligned range to leverage this. */
typedef uint32_t AlignedNodeBuffer;
#endif
constexpr static int ByteAlignment = sizeof(AlignedNodeBuffer);

class TreeNode {
  friend class TreePool;
public:
  constexpr static uint16_t NoNodeIdentifier = -2;
  // Used for Integer
  constexpr static uint16_t OverflowIdentifier = TreeNode::NoNodeIdentifier + 1;

  // Constructor and destructor
  virtual ~TreeNode() {}
  typedef TreeNode * (* const Initializer)(void *);

  // Attributes
  void setParentIdentifier(uint16_t parentID) { m_parentIdentifier = parentID; }
  void deleteParentIdentifier() { m_parentIdentifier = NoNodeIdentifier; }
  virtual size_t size() const = 0;
  uint16_t identifier() const { return m_identifier; }
  int retainCount() const { return m_referenceCounter; }
  size_t deepSize(int realNumberOfChildren) const;

  // Ghost
  virtual bool isGhost() const { return false; }
  bool deepIsGhost() const;

  // Node operations
  void setReferenceCounter(int refCount) { m_referenceCounter = refCount; }
  void retain() { m_referenceCounter++; }
  void release(int currentNumberOfChildren);
  void rename(uint16_t identifier, bool unregisterPreviousIdentifier, bool skipChildrenUpdate = false);

  // Hierarchy
  TreeNode * parent() const;
  TreeNode * root();
  virtual int numberOfChildren() const = 0;
  /* The following methods are only used for nodes that have a variable number
  * of children like List, HorizontalLayout or Randint */
  virtual void setNumberOfChildren(int numberOfChildren) {} // Do not assert false.
  void incrementNumberOfChildren(int increment = 1) { setNumberOfChildren(numberOfChildren() + increment); }
  void eraseNumberOfChildren() { setNumberOfChildren(0); }
  int numberOfDescendants(bool includeSelf) const;
  TreeNode * childAtIndex(int i) const;
  int indexOfChild(const TreeNode * child) const;
  int indexInParent() const;
  bool hasChild(const TreeNode * child) const;
  bool hasAncestor(const TreeNode * node, bool includeSelf) const;
  bool hasSibling(const TreeNode * e) const;
  void deleteParentIdentifierInChildren() const {
    changeParentIdentifierInChildren(NoNodeIdentifier);
  }
  // AddChild collateral effect
  virtual void didChangeArity(int newNumberOfChildren) {}

  // Serialization
  // Return the number of chars written, without the null-terminating char.
  virtual int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const { assert(false); return 0; }

  /* When serializing, we turn a tree into a string. In order not to lose
   * structure information, we sometimes need to add system parentheses (that
   * are invisible when turning the string back into a tree).
   * For example:
   * - Layout:
   *  2                                                                               (2
   * --- π --> [[2]/[3]]π which keeps the omitted multiplication and forbid to parse --- π
   *  3                                                                                3)
   * - Expression:
   *  2+3
   * ----- --> [2+3]/4 to keep the fraction structure
   *   4
   *
   * */
  virtual bool childNeedsSystemParenthesesAtSerialization(const TreeNode * child) const { return false; }

  template <typename T>
  class Iterator {
  public:
    Iterator(const T * node) : m_node(const_cast<T *>(node)) {}
    T * operator*() { return m_node; }
    bool operator!=(const Iterator& it) const { return (m_node != it.m_node); }
  protected:
    T * m_node;
  };

  template <typename T>
  class Direct final {
  public:
    Direct(const T * node, int firstIndex = 0) : m_node(const_cast<T *>(node)), m_firstIndex(firstIndex) {}
    class Iterator : public TreeNode::Iterator<T> {
    public:
      using TreeNode::Iterator<T>::Iterator;
      Iterator & operator++() {
        this->m_node = static_cast<T*>(this->m_node->nextSibling());
        return *this;
      }
    };
    Iterator begin() const {
      TreeNode * n = m_node->next();
      for (int i = 0; i < m_firstIndex; i++) {
        n = n->nextSibling();
      }
      return Iterator(static_cast<T *>(n));
    }
    Iterator end() const { return Iterator(static_cast<T *>(m_node->nextSibling())); }
  private:
    T * m_node;
    int m_firstIndex;
  };

  template <typename T>
  class DepthFirst final {
  public:
    DepthFirst(const TreeNode * node) : m_node(const_cast<TreeNode *>(node)) {}
    class Iterator : public TreeNode::Iterator<T> {
    public:
      using TreeNode::Iterator<T>::Iterator;
      Iterator & operator++() {
        this->m_node = this->m_node->next();
        return *this;
      }
    };
    Iterator begin() const { return Iterator(m_node->next()); }
    Iterator end() const { return Iterator(m_node->nextSibling()); }
  private:
    T * m_node;
  };

  Direct<TreeNode> directChildren() const { return Direct<TreeNode>(this); }
  DepthFirst<TreeNode> depthFirstChildren() const { return DepthFirst<TreeNode>(this); }

  TreeNode * next() const {
    /* Simple version would be "return this + 1;", with pointer arithmetics
     * taken care of by the compiler. Unfortunately, we want TreeNode to have a
     * VARIABLE size */
    return reinterpret_cast<TreeNode *>(reinterpret_cast<char *>(const_cast<TreeNode *>(this)) + Helpers::AlignedSize(size(), ByteAlignment));
  }
  TreeNode * nextSibling() const;
  TreeNode * lastDescendant() const;

#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const = 0;
  virtual void logAttributes(std::ostream & stream) const {}
  void log(std::ostream & stream, bool recursive = true, int indentation = 0, bool verbose = true);
  void log() { log(std::cout); std::cout << std::endl; }
#endif

  static bool IsValidIdentifier(uint16_t id) { return id < NoNodeIdentifier; }

protected:
  TreeNode() :
    m_identifier(NoNodeIdentifier),
    m_parentIdentifier(NoNodeIdentifier),
    m_referenceCounter(0)
  {}

private:
  void updateParentIdentifierInChildren() const {
    changeParentIdentifierInChildren(m_identifier);
  }
  void changeParentIdentifierInChildren(uint16_t id) const;
  uint16_t m_identifier;
  uint16_t m_parentIdentifier;
  int8_t m_referenceCounter;
};

template<typename T>
static TreeNode * Initializer(void * buffer) {
  return new (buffer) T;
}

}

#endif
