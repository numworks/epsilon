#ifndef POINCARE_TREE_BY_REFERENCE_H
#define POINCARE_TREE_BY_REFERENCE_H

#include <poincare/tree_pool.h>
#include <initializer_list>

namespace Poincare {
/* TODO: implement an iterator over the children, so we can use "for (TreeHandle
 * c : children)" instead of a for loop over the child index. This should be
 * faster, as we do not have to recompute childAtIndex(i) at each iteration.
 * Caution:
 *  - The iterator should be specialized for Expression, Layout...
 *  - We cannot compose with a TreeNode iterator as the node pointers might
 *  change during the loop. */

/* A TreeHandle references a TreeNode stored somewhere is the Expression Pool,
 * and identified by its idenfier.
 * Any method that can possibly move the object ("break the this")
 * therefore needs to be implemented in the Handle rather than the Node.
 */
class TreeHandle {
  friend class TreeNode;
  friend class TreePool;
public:
  /* Constructors  */
  /* TreeHandle constructors that take only one argument and this argument is
  * a TreeHandle should be marked explicit. This prevents the code from
  * compiling with, for instance: Logarithm l = clone() (which would be
  * equivalent to Logarithm l = Logarithm(clone())). */
  TreeHandle(const TreeHandle & tr) : m_identifier(TreeNode::NoNodeIdentifier) {
    setIdentifierAndRetain(tr.identifier());
  }

  TreeHandle(TreeHandle && tr) : m_identifier(tr.m_identifier) {
    tr.m_identifier = TreeNode::NoNodeIdentifier;
  }

  ~TreeHandle() {
    release(m_identifier);
  }

  /* Operators */
  TreeHandle & operator=(const TreeHandle & tr) {
    setTo(tr);
    return *this;
  }

  TreeHandle & operator=(TreeHandle && tr) {
    release(m_identifier);
    m_identifier = tr.m_identifier;
    tr.m_identifier = TreeNode::NoNodeIdentifier;
    return *this;
  }

  /* Comparison */
  inline bool operator==(const TreeHandle& t) const { return m_identifier == t.identifier(); }
  inline bool operator!=(const TreeHandle& t) const { return m_identifier != t.identifier(); }

  /* Clone */
  TreeHandle clone() const;

  uint16_t identifier() const { return m_identifier; }
  TreeNode * node() const;
  bool wasErasedByException() const {
    return hasNode(m_identifier) && node() == nullptr;
  }
  int nodeRetainCount() const { return node()->retainCount(); }
  size_t size() const;
  size_t sizeOfNode() const { return node()->size(); }
  void * addressInPool() const { return reinterpret_cast<void *>(node()); }

  bool isGhost() const { return node()->isGhost(); }
  bool isUninitialized() const { return m_identifier == TreeNode::NoNodeIdentifier; }
  bool isDownstreamOf(char * treePoolCursor) { return !isUninitialized() && (node() == nullptr || reinterpret_cast<char *>(node()) >= treePoolCursor); }

  /* Hierarchy */
  bool hasChild(TreeHandle t) const;
  bool hasSibling(TreeHandle t) const { return node()->hasSibling(t.node()); }
  bool hasAncestor(TreeHandle t, bool includeSelf) const { return node()->hasAncestor(t.node(), includeSelf); }
  TreeHandle commonAncestorWith(TreeHandle t) const;
  int numberOfChildren() const { return node()->numberOfChildren(); }
  int indexOfChild(TreeHandle t) const;
  TreeHandle parent() const;
  TreeHandle childAtIndex(int i) const;
  void setParentIdentifier(uint16_t id) { node()->setParentIdentifier(id); }
  void deleteParentIdentifier() { node()->deleteParentIdentifier(); }
  void deleteParentIdentifierInChildren() { node()->deleteParentIdentifierInChildren(); }
  void incrementNumberOfChildren(int increment = 1) { node()->incrementNumberOfChildren(increment); }
  int numberOfDescendants(bool includeSelf) const { return node()->numberOfDescendants(includeSelf); }

  /* Hierarchy operations */
  // Replace
  void replaceWithInPlace(TreeHandle t);
  void replaceChildInPlace(TreeHandle oldChild, TreeHandle newChild);
  void replaceChildAtIndexInPlace(int oldChildIndex, TreeHandle newChild);
  void replaceChildAtIndexWithGhostInPlace(int index) {
    assert(index >= 0 && index < numberOfChildren());
    replaceChildWithGhostInPlace(childAtIndex(index));
  }
  void replaceChildWithGhostInPlace(TreeHandle t);
  // Merge
  void mergeChildrenAtIndexInPlace(TreeHandle t, int i);
  // Swap
  void swapChildrenInPlace(int i, int j);

  /* Logging */
#if POINCARE_TREE_LOG
  void log() const;
#endif

  typedef std::initializer_list<TreeHandle> Tuple;

  static TreeHandle Builder(TreeNode * (* const initializer)(void *), size_t size);
  static TreeHandle Maker(TreeNode * (* const initializer)(void *), size_t size, const Tuple & children);

protected:
  /* Constructor */
  TreeHandle(const TreeNode * node);
  // Un-inlining this constructor actually inscreases the firmware size
  TreeHandle(uint16_t nodeIndentifier = TreeNode::NoNodeIdentifier) : m_identifier(nodeIndentifier) {
    if (hasNode(nodeIndentifier)) {
      node()->retain();
    }
  }

  // WARNING: if the children table is the result of a cast, the object downcasted has to be the same size as a TreeHandle.
  template <class T, class U>
  static T NAryBuilder(const Tuple & children = {});
  template <class T, class U>
  static T FixedArityBuilder(const Tuple & children = {});

  static TreeHandle BuildWithGhostChildren(TreeNode * node);

  void setIdentifierAndRetain(uint16_t newId);
  void setTo(const TreeHandle & tr);

  static bool hasNode(uint16_t identifier) { return TreeNode::IsValidIdentifier(identifier); }

  /* Hierarchy operations */
  // Add
  void addChildAtIndexInPlace(TreeHandle t, int index, int currentNumberOfChildren);
  // Remove puts a child at the end of the pool
  void removeChildAtIndexInPlace(int i);
  void removeChildInPlace(TreeHandle t, int childNumberOfChildren);
  void removeChildrenInPlace(int currentNumberOfChildren);

  uint16_t m_identifier;

private:
  template <class U>
  static TreeHandle Builder();

  void detachFromParent();
  // Add ghost children on layout construction
  void buildGhostChildren();
  void release(uint16_t identifier);
};

}

#endif
