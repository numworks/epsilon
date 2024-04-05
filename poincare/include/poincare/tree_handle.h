#ifndef POINCARE_TREE_HANDLE_H
#define POINCARE_TREE_HANDLE_H

#include <poincare/pool.h>

#include <initializer_list>

namespace Poincare {
/* A TreeHandle references a TreeNode stored somewhere is the OExpression Pool,
 * and identified by its idenfier.
 * Any method that can possibly move the object ("break the this")
 * therefore needs to be implemented in the Handle rather than the Node.
 */
class TreeHandle {
  friend class TreeNode;
  friend class Pool;

 public:
  /* Constructors  */
  /* TreeHandle constructors that take only one argument and this argument is
   * a TreeHandle should be marked explicit. This prevents the code from
   * compiling with, for instance: Logarithm l = clone() (which would be
   * equivalent to Logarithm l = Logarithm(clone())). */
  TreeHandle(const TreeHandle& tr) : m_identifier(TreeNode::NoNodeIdentifier) {
    setIdentifierAndRetain(tr.identifier());
  }

  TreeHandle(TreeHandle&& tr) : m_identifier(tr.m_identifier) {
    tr.m_identifier = TreeNode::NoNodeIdentifier;
  }

  ~TreeHandle() { release(m_identifier); }

  /* Operators */
  TreeHandle& operator=(const TreeHandle& tr) {
    setTo(tr);
    return *this;
  }

  TreeHandle& operator=(TreeHandle&& tr) {
    release(m_identifier);
    m_identifier = tr.m_identifier;
    tr.m_identifier = TreeNode::NoNodeIdentifier;
    return *this;
  }

  /* Comparison */
  inline bool operator==(const TreeHandle& t) const {
    return m_identifier == t.identifier();
  }
  inline bool operator!=(const TreeHandle& t) const {
    return m_identifier != t.identifier();
  }

  /* Clone */
  TreeHandle clone() const;

  uint16_t identifier() const { return m_identifier; }
  TreeNode* node() const;
  bool wasErasedByException() const {
    return hasNode(m_identifier) && node() == nullptr;
  }
  int nodeRetainCount() const { return node()->retainCount(); }
  size_t size() const;
  size_t sizeOfNode() const { return node()->size(); }
  void* addressInPool() const { return reinterpret_cast<void*>(node()); }

  bool isGhost() const { return node()->isGhost(); }
  bool deepIsGhost() const { return node()->deepIsGhost(); }
  bool isUninitialized() const {
    return m_identifier == TreeNode::NoNodeIdentifier;
  }
  bool isDownstreamOf(TreeNode* treePoolCursor) {
    return !isUninitialized() &&
           (node() == nullptr || node() >= treePoolCursor);
  }

  /* Hierarchy */
  bool hasChild(TreeHandle t) const;
  bool hasSibling(TreeHandle t) const { return node()->hasSibling(t.node()); }
  bool hasAncestor(TreeHandle t, bool includeSelf) const {
    return node()->hasAncestor(t.node(), includeSelf);
  }
  TreeHandle commonAncestorWith(TreeHandle t,
                                bool includeTheseNodes = true) const;
  int numberOfChildren() const { return node()->numberOfChildren(); }
  void setNumberOfChildren(int numberOfChildren) {
    node()->setNumberOfChildren(numberOfChildren);
  }
  int indexOfChild(TreeHandle t) const;
  TreeHandle parent() const;
  TreeHandle childAtIndex(int i) const;
  void setParentIdentifier(uint16_t id) { node()->setParentIdentifier(id); }
  void deleteParentIdentifier() { node()->deleteParentIdentifier(); }
  void deleteParentIdentifierInChildren() {
    node()->deleteParentIdentifierInChildren();
  }
  void incrementNumberOfChildren(int increment = 1) {
    node()->incrementNumberOfChildren(increment);
  }
  int numberOfDescendants(bool includeSelf) const {
    return node()->numberOfDescendants(includeSelf);
  }

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

  static TreeHandle Builder(TreeNode::Initializer initializer, size_t size,
                            int numberOfChildren = -1);
  static TreeHandle BuilderWithChildren(TreeNode::Initializer initializer,
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
      TreeNode* node = m_handle.node()->next();
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

  Direct<TreeHandle, TreeNode> directChildren() const {
    return Direct<TreeHandle, TreeNode>(*this);
  }

 protected:
  /* Constructor */
  TreeHandle(const TreeNode* node);
  // Un-inlining this constructor actually inscreases the firmware size
  TreeHandle(uint16_t nodeIndentifier = TreeNode::NoNodeIdentifier)
      : m_identifier(nodeIndentifier) {
    if (hasNode(nodeIndentifier)) {
      node()->retain();
    }
  }

  /* WARNING: if the children table is the result of a cast, the object
   * downcasted has to be the same size as a TreeHandle. */
  template <class T, class U>
  static T NAryBuilder(const Tuple& children = {});
  template <class T, class U>
  static T FixedArityBuilder(const Tuple& children = {});

  static TreeHandle BuildWithGhostChildren(TreeNode* node);

  void setIdentifierAndRetain(uint16_t newId);
  void setTo(const TreeHandle& tr);

  static bool hasNode(uint16_t identifier) {
    return TreeNode::IsValidIdentifier(identifier);
  }

  /* Hierarchy operations */
  // Add
  void addChildAtIndexInPlace(TreeHandle t, int index,
                              int currentNumberOfChildren);
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

}  // namespace Poincare

#endif
