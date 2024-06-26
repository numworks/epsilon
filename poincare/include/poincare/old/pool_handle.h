#ifndef POINCARE_TREE_HANDLE_H
#define POINCARE_TREE_HANDLE_H

#include <initializer_list>

#include "pool.h"

namespace Poincare {
/* A PoolHandle references a PoolObject stored somewhere is the OExpression
 * Pool, and identified by its idenfier. Any method that can possibly move the
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
  size_t sizeOfNode() const { return object()->size(); }
  void* addressInPool() const { return reinterpret_cast<void*>(object()); }

  bool isGhost() const { return object()->isGhost(); }
  bool deepIsGhost() const { return object()->deepIsGhost(); }
  bool isUninitialized() const {
    return m_identifier == PoolObject::NoNodeIdentifier;
  }
  bool isDownstreamOf(PoolObject* treePoolCursor) {
    return !isUninitialized() &&
           (object() == nullptr || object() >= treePoolCursor);
  }

  /* Hierarchy */
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

  /* Hierarchy operations */
  // Replace
  void replaceWithInPlace(PoolHandle t);
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

  /* Logging */
#if POINCARE_TREE_LOG
  void log() const;
#endif

  typedef std::initializer_list<PoolHandle> Tuple;

  static PoolHandle Builder(PoolObject::Initializer initializer, size_t size,
                            int numberOfChildren = -1);
  static PoolHandle BuilderWithChildren(PoolObject::Initializer initializer,
                                        size_t size, const Tuple& children);

  // Iterator
  template <typename Handle, typename Node>
  class Direct final {
   public:
    Direct(const Handle handle, int firstIndex = 0)
        : m_handle(handle), m_firstIndex(firstIndex) {}

    class Iterator {
     public:
      Iterator(Node* node) : m_handle(NodePointerInPool(node)), m_node(node) {}

      Handle operator*() const { return m_handle; }
      bool operator!=(const Iterator& rhs) const {
        return m_handle != rhs.m_handle;
      }
      Iterator& operator++() {
        m_node = static_cast<Node*>(m_node->nextSibling());
        m_handle = Handle(NodePointerInPool(m_node));
        return *this;
      }

     private:
      /* This iterator needs to keep both a node and a handle:
       * - a handle to ensure termination even if tree modifications move the
       * next sibling.
       * - a node to make sure it keeps navigating the same tree even if the
       * curent node is moved.
       * e.g. Pool is: |-|abs|a|b|*|
       *      We want to iterate over the subtraction and move its children in
       *      the multiplication.
       * 1) |+|abs|a|b|*|
       *        ^      ^end
       *        current node
       *
       * 2) |+|ghost|b|*|abs|a|
       *         ^     ^end
       *    Here the address of |*| has changed, but the end iterator still
       *    refers to it because it holds a handle. However, by holding a
       *    pointer to its current node, iteration will continue from where
       *    |abs|a| was, instead of where it has been moved.
       * */
      Handle m_handle;
      Node* m_node;
    };

    Iterator begin() const {
      PoolObject* node = m_handle.node()->next();
      for (int i = 0; i < m_firstIndex; i++) {
        node = node->nextSibling();
      }
      return Iterator(static_cast<Node*>(node));
    }
    Iterator end() const {
      return Iterator(static_cast<Node*>(m_handle.node()->nextSibling()));
    }

    Node* node() const { return m_handle.node(); }

   private:
    static Node* NodePointerInPool(Node* node) {
      return node < Pool::sharedPool->cursor() ? node : nullptr;
    }

    Handle m_handle;
    int m_firstIndex;
  };

  Direct<PoolHandle, PoolObject> directChildren() const {
    return Direct<PoolHandle, PoolObject>(*this);
  }

 protected:
  /* Constructor */
  PoolHandle(const PoolObject* node);
  // Un-inlining this constructor actually inscreases the firmware size
  PoolHandle(uint16_t nodeIndentifier = PoolObject::NoNodeIdentifier)
      : m_identifier(nodeIndentifier) {
    if (hasNode(nodeIndentifier)) {
      object()->retain();
    }
  }

  /* WARNING: if the children table is the result of a cast, the object
   * downcasted has to be the same size as a PoolHandle. */
  template <class T, class U>
  static T NAryBuilder(const Tuple& children = {});
  template <class T, class U>
  static T FixedArityBuilder(const Tuple& children = {});

  static PoolHandle BuildWithGhostChildren(PoolObject* node);

  void setIdentifierAndRetain(uint16_t newId);
  void setTo(const PoolHandle& tr);

  static bool hasNode(uint16_t identifier) {
    return PoolObject::IsValidIdentifier(identifier);
  }

  /* Hierarchy operations */
  // Add
  void addChildAtIndexInPlace(PoolHandle t, int index,
                              int currentNumberOfChildren);
  // Remove puts a child at the end of the pool
  void removeChildAtIndexInPlace(int i);
  void removeChildInPlace(PoolHandle t, int childNumberOfChildren);
  void removeChildrenInPlace(int currentNumberOfChildren);

  uint16_t m_identifier;

 private:
  template <class U>
  static PoolHandle Builder();

  void detachFromParent();
  // Add ghost children on layout construction
  void buildGhostChildren();
  void release(uint16_t identifier);
};

}  // namespace Poincare

#endif
