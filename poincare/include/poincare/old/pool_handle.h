#ifndef POINCARE_TREE_HANDLE_H
#define POINCARE_TREE_HANDLE_H

#include <initializer_list>

#include "pool.h"

namespace Poincare {

// TODO_PCJ: See comment in pool_object.h
#define PCJ_DELETE 1

/* A PoolHandle references a PoolObject stored somewhere is the OExpression
 * Pool, and identified by its identifier. Any method that can possibly move the
 * object ("break the this") therefore needs to be implemented in the Handle
 * rather than the Node.
 */
class PoolHandle {
  friend class PoolObject;
  friend class Pool;

 public:
  /* Constructors  */
  /* PoolHandle constructors that take only one argument and this argument is
   * a PoolHandle should be marked explicit. This prevents the code from
   * compiling with, for instance: Logarithm l = clone() (which would be
   * equivalent to Logarithm l = Logarithm(clone())). */
  PoolHandle(const PoolHandle& tr)
      : m_identifier(PoolObject::NoNodeIdentifier) {
    setIdentifierAndRetain(tr.identifier());
  }

  PoolHandle(PoolHandle&& tr) : m_identifier(tr.m_identifier) {
    tr.m_identifier = PoolObject::NoNodeIdentifier;
  }

  ~PoolHandle() { release(m_identifier); }

  /* Operators */
  PoolHandle& operator=(const PoolHandle& tr) {
    setTo(tr);
    return *this;
  }

  PoolHandle& operator=(PoolHandle&& tr) {
    release(m_identifier);
    m_identifier = tr.m_identifier;
    tr.m_identifier = PoolObject::NoNodeIdentifier;
    return *this;
  }

  /* Comparison */
  inline bool operator==(const PoolHandle& t) const {
    return m_identifier == t.identifier();
  }
  inline bool operator!=(const PoolHandle& t) const {
    return m_identifier != t.identifier();
  }

  /* Clone */
  PoolHandle clone() const;

  uint16_t identifier() const { return m_identifier; }
  PoolObject* object() const;
  bool wasErasedByException() const {
    return hasNode(m_identifier) && object() == nullptr;
  }
  int nodeRetainCount() const { return object()->retainCount(); }
  size_t size() const;
  void* addressInPool() const { return reinterpret_cast<void*>(object()); }

  bool isGhost() const { return object()->isGhost(); }
#if PCJ_DELETE
  bool deepIsGhost() const { return object()->deepIsGhost(); }
#endif
  bool isUninitialized() const {
    return m_identifier == PoolObject::NoNodeIdentifier;
  }
  bool isDownstreamOf(PoolObject* treePoolCursor) {
    return !isUninitialized() &&
           (object() == nullptr || object() >= treePoolCursor);
  }

/* Hierarchy */
#if PCJ_DELETE
  bool hasChild(PoolHandle t) const;
  bool hasSibling(PoolHandle t) const {
    return object()->hasSibling(t.object());
  }
  bool hasAncestor(PoolHandle t, bool includeSelf) const {
    return object()->hasAncestor(t.object(), includeSelf);
  }
  PoolHandle commonAncestorWith(PoolHandle t,
                                bool includeTheseNodes = true) const;
  int numberOfChildren() const { return object()->numberOfChildren(); }
  void setNumberOfChildren(int numberOfChildren) {
    object()->setNumberOfChildren(numberOfChildren);
  }
  int indexOfChild(PoolHandle t) const;
  PoolHandle parent() const;
  PoolHandle childAtIndex(int i) const;
  void setParentIdentifier(uint16_t id) { object()->setParentIdentifier(id); }
  void deleteParentIdentifier() { object()->deleteParentIdentifier(); }
  void deleteParentIdentifierInChildren() {
    object()->deleteParentIdentifierInChildren();
  }
  void incrementNumberOfChildren(int increment = 1) {
    object()->incrementNumberOfChildren(increment);
  }
  int numberOfDescendants(bool includeSelf) const {
    return object()->numberOfDescendants(includeSelf);
  }
#endif

  /* Hierarchy operations */
  // Replace
  void replaceWithInPlace(PoolHandle t);
#if PCJ_DELETE
  void replaceChildInPlace(PoolHandle oldChild, PoolHandle newChild);
  void replaceChildAtIndexInPlace(int oldChildIndex, PoolHandle newChild);
  void replaceChildAtIndexWithGhostInPlace(int index) {
    assert(index >= 0 && index < numberOfChildren());
    replaceChildWithGhostInPlace(childAtIndex(index));
  }
  void replaceChildWithGhostInPlace(PoolHandle t);
  // Merge
  void mergeChildrenAtIndexInPlace(PoolHandle t, int i);
  // Swap
  void swapChildrenInPlace(int i, int j);
#endif

  /* Logging */
#if POINCARE_TREE_LOG
  void log() const;
#endif

  typedef std::initializer_list<PoolHandle> Tuple;

  static PoolHandle Builder(PoolObject::Initializer initializer, size_t size,
                            int numberOfChildren = -1);
#if PCJ_DELETE
  static PoolHandle BuilderWithChildren(PoolObject::Initializer initializer,
                                        size_t size, const Tuple& children);

#endif

 protected:
  /* Constructor */
  PoolHandle(const PoolObject* node);
  // Un-inlining this constructor actually increases the firmware size
  PoolHandle(uint16_t nodeIdentifier = PoolObject::NoNodeIdentifier)
      : m_identifier(nodeIdentifier) {
    if (hasNode(nodeIdentifier)) {
      object()->retain();
    }
  }

#if PCJ_DELETE
  /* WARNING: if the children table is the result of a cast, the object
   * downcasted has to be the same size as a PoolHandle. */
  template <class T, class U>
  static T NAryBuilder(const Tuple& children = {});
  template <class T, class U>
  static T FixedArityBuilder(const Tuple& children = {});

  static PoolHandle BuildWithGhostChildren(PoolObject* node);
#endif

  void setIdentifierAndRetain(uint16_t newId);
  void setTo(const PoolHandle& tr);

  static bool hasNode(uint16_t identifier) {
    return PoolObject::IsValidIdentifier(identifier);
  }

/* Hierarchy operations */
#if PCJ_DELETE
  // Add
  void addChildAtIndexInPlace(PoolHandle t, int index,
                              int currentNumberOfChildren);
  // Remove puts a child at the end of the pool
  void removeChildAtIndexInPlace(int i);
  void removeChildInPlace(PoolHandle t, int childNumberOfChildren);
#endif
  uint16_t m_identifier;

 private:
  template <class U>
  static PoolHandle Builder();

#if PCJ_DELETE
  void detachFromParent();
  // Add ghost children on layout construction
  void buildGhostChildren();
#endif
  void release(uint16_t identifier);
};

}  // namespace Poincare

#endif
