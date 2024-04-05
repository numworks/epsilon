#ifndef POINCARE_MEMORY_EDITION_REFERENCE_H
#define POINCARE_MEMORY_EDITION_REFERENCE_H

#include "k_tree.h"
#include "tree.h"
#include "tree_stack.h"

namespace PoincareJ {

class TreeRef {
 public:
  TreeRef() : m_identifier(TreeStack::ReferenceTable::NoNodeIdentifier) {}
  TreeRef(Tree* node);

  template <TreeConcept T>
  TreeRef(T t) : TreeRef(static_cast<const Tree*>(t)) {}

  TreeRef(Block* blocks) : TreeRef(Tree::FromBlocks(blocks)) {}

  // Copies create a new ref with a new id
  TreeRef(const TreeRef& other) : TreeRef(static_cast<Tree*>(other)) {}

  TreeRef& operator=(const TreeRef& other) {
    *this = TreeRef(static_cast<Tree*>(other));
    return *this;
  }

  TreeRef& operator=(Tree* tree);

  // Moves steal the id
  TreeRef(TreeRef&& other) {
    m_identifier = other.m_identifier;
    other.m_identifier = TreeStack::ReferenceTable::NoNodeIdentifier;
  }

  TreeRef& operator=(TreeRef&& other) {
    if (m_identifier != other.m_identifier) {
      SharedTreeStack->deleteIdentifier(m_identifier);
      m_identifier = other.m_identifier;
      other.m_identifier = TreeStack::ReferenceTable::NoNodeIdentifier;
    }
    return *this;
  }

  ~TreeRef() { stopTracking(); }

  void stopTracking() {
    SharedTreeStack->deleteIdentifier(m_identifier);
    m_identifier = TreeStack::ReferenceTable::NoNodeIdentifier;
  }

#if POINCARE_TREE_LOG
  __attribute__((__used__)) void log() const;
#endif

  // Make if (ref) do the same thing as if (treePtr)
  explicit operator bool() const { return tree() != nullptr; }

  operator Tree*() const { return tree(); }
  Tree* operator->() { return tree(); }

  /* Comparison */
  inline bool operator==(const TreeRef& t) const {
    return m_identifier == t.identifier() ||
           (!isUninitialized() && !t.isUninitialized() && tree() == t.tree());
  }
  inline bool operator!=(const TreeRef& t) const {
    return m_identifier != t.identifier() &&
           (isUninitialized() || t.isUninitialized() || tree() != t.tree());
  }

  bool isUninitialized() const { return tree() == nullptr; }

  uint16_t identifier() const { return m_identifier; }

  typedef void (*InPlaceTreeFunction)(TreeRef reference);
  void recursivelyEdit(InPlaceTreeFunction treeFunction);

 private:
  TreeRef(const Tree* tree) : TreeRef(tree->clone()){};
  Tree* tree() const;
  uint16_t m_identifier;
};

inline bool operator==(Tree* n, const TreeRef& r) {
  return n == static_cast<Tree*>(r);
}

/* We have a semantical conflict between most our functions that want to alter a
 * tree where it is and the TreeRef update mechanism that invalidates
 * the reference when the tree it points to is replaced by something else. This
 * helper is used to associate to F(Tree *) that alters a tree in-place a
 * wrapper F(TreeRef &) that alters the tree pointed by the reference
 * and keeps the reference valid. */
template <class Result, class... Args>
inline Result ApplyPreservingReference(Result treeFunction(Tree*, Args...),
                                       TreeRef& ref, Args... args) {
  Tree* location = ref;
  /* ref will be overriden after treeFunction and is no longer worth preserving.
   * Any Raise could also corrupt the previous identifier. */
  ref.stopTracking();
  Result result = treeFunction(location, args...);
  ref = location;
  return result;
}

/* Macros to define a method working with references from a method on tree */

/* No argument */
#define EDITION_REF_WRAP(F) \
  static bool F(TreeRef& r) { return ApplyPreservingReference(F, r); }

/* One argument */
#define EDITION_REF_WRAP_1(F, T) \
  static bool F(TreeRef& r, T a1) { return ApplyPreservingReference(F, r, a1); }

/* One argument with default value */
#define EDITION_REF_WRAP_1D(F, T, D)           \
  static bool F(TreeRef& r, T a1 = D) {        \
    return ApplyPreservingReference(F, r, a1); \
  }

/* Two arguments */
#define EDITION_REF_WRAP_2(F, T1, T2)              \
  static bool F(TreeRef& r, T1 u1, T2 u2) {        \
    return ApplyPreservingReference(F, r, u1, u2); \
  }

/* Two arguments with default value */
#define EDITION_REF_WRAP_2D(F, T1, D1, T2, D2)        \
  static bool F(TreeRef& r, T1 u1 = D1, T2 u2 = D2) { \
    return ApplyPreservingReference(F, r, u1, u2);    \
  }

void CloneNodeAtNode(TreeRef& target, const Tree* nodeToClone);
void CloneTreeAtNode(TreeRef& target, const Tree* treeToClone);

void MoveAt(TreeRef& target, Tree* toMove, bool tree, bool before);

inline void MoveNodeAtNode(TreeRef& target, Tree* nodeToMove) {
  MoveAt(target, nodeToMove, false, false);
}

inline void MoveTreeAtNode(TreeRef& target, Tree* treeToMove) {
  MoveAt(target, treeToMove, true, false);
}

inline void MoveNodeBeforeNode(TreeRef& target, Tree* nodeToMove) {
  MoveAt(target, nodeToMove, false, true);
}

inline void MoveTreeBeforeNode(TreeRef& target, Tree* treeToMove) {
  MoveAt(target, treeToMove, true, true);
}

inline void MoveNodeOverTree(TreeRef& u, Tree* n) {
  u = u->moveNodeOverTree(n);
}

inline void MoveTreeOverTree(TreeRef& u, Tree* n) {
  u = u->moveTreeOverTree(n);
}

inline void MoveNodeOverNode(TreeRef& u, Tree* n) {
  u = u->moveNodeOverNode(n);
}

inline void MoveTreeOverNode(TreeRef& u, Tree* n) {
  u = u->moveTreeOverNode(n);
}

inline void CloneTreeOverNode(TreeRef& u, const Tree* n) {
  u = u->cloneTreeOverNode(n);
}

inline void CloneTreeOverTree(TreeRef& u, const Tree* n) {
  u = u->cloneTreeOverTree(n);
}

inline void CloneNodeOverNode(TreeRef& u, const Tree* n) {
  u = u->cloneNodeOverNode(n);
}

inline void CloneNodeOverTree(TreeRef& u, const Tree* n) {
  u = u->cloneNodeOverTree(n);
}

}  // namespace PoincareJ

#endif
