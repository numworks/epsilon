#ifndef POINCARE_TREE_NODE_H
#define POINCARE_TREE_NODE_H

#include <assert.h>
#include <stddef.h>
#include <strings.h>
#include <stdint.h>
#if POINCARE_TREE_LOG
#include <ostream>
#endif
#include <poincare/helpers.h>
#include <poincare/preferences.h>

/* What's in a TreeNode, really?
 *  - a vtable pointer
 *  - an identifier
 *  - a parent identifier
 *  - a reference counter
 */

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
  static constexpr int NoNodeIdentifier = -1;

  // Constructor and destructor
  virtual void initToMatchSize(size_t goalSize) { assert(false); }
  virtual ~TreeNode() {}

  // Attributes
  void setParentIdentifier(int parentID) { m_parentIdentifier = parentID; }
  void deleteParentIdentifier() { m_parentIdentifier = NoNodeIdentifier; }
  virtual size_t size() const = 0;
  int identifier() const { return m_identifier; }
  int retainCount() const { return m_referenceCounter; }
  size_t deepSize(int realNumberOfChildren) const;

  // Ghost
  virtual bool isGhost() const { return false; }

  // Node operations
  void setReferenceCounter(int refCount) { m_referenceCounter = refCount; }
  void retain() { m_referenceCounter++; }
  void release(int currentNumberOfChildren);
  void rename(int identifier, bool unregisterPreviousIdentifier);

  // Hierarchy
  virtual TreeNode * parent() const;
  virtual TreeNode * root();
  virtual int numberOfChildren() const = 0;
  virtual void incrementNumberOfChildren(int increment = 1) {} // Do no put an assert(false), we need this method for instance in GridLayout::removeRow
  virtual void decrementNumberOfChildren(int decrement = 1) {} // Do no put an assert(false), we need this method for instance in GridLayout::removeRow
  virtual void eraseNumberOfChildren() {}
  int numberOfDescendants(bool includeSelf) const;
  virtual TreeNode * childAtIndex(int i) const;
  int indexOfChild(const TreeNode * child) const;
  int indexInParent() const;
  bool hasChild(const TreeNode * child) const;
  bool hasAncestor(const TreeNode * node, bool includeSelf) const;
  bool hasSibling(const TreeNode * e) const;
  void deleteParentIdentifierInChildren() const {
    changeParentIdentifierInChildren(NoNodeIdentifier);
  }
  // AddChild collateral effect
  virtual void didAddChildAtIndex(int newNumberOfChildren) {}

  // Serialization
  virtual int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = 0) const { assert(false); return 0; }
  virtual bool childNeedsParenthesis(const TreeNode * child) const { return false; };

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
    Direct(const T * node) : m_node(const_cast<T *>(node)) {}
    class Iterator : public TreeNode::Iterator<T> {
    public:
      using TreeNode::Iterator<T>::Iterator;
      Iterator & operator++() {
        this->m_node = static_cast<T*>(this->m_node->nextSibling());
        return *this;
      }
    };
    Iterator begin() const { return Iterator(static_cast<T *>(m_node->next())); }
    Iterator end() const { return Iterator(static_cast<T *>(m_node->nextSibling())); }
  private:
    T * m_node;
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
  void log(std::ostream & stream, bool recursive = true);
#endif

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
  void changeParentIdentifierInChildren(int id) const;
  int16_t m_identifier;
  int16_t m_parentIdentifier;
  int8_t m_referenceCounter;
};

}

#endif
