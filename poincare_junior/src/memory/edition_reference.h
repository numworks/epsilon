#ifndef POINCARE_MEMORY_EDITION_REFERENCE_H
#define POINCARE_MEMORY_EDITION_REFERENCE_H

#include "edition_pool.h"
#include "k_tree.h"
#include "tree.h"

namespace PoincareJ {

class EditionReference {
 public:
  EditionReference()
      : m_identifier(EditionPool::ReferenceTable::NoNodeIdentifier) {}
  EditionReference(Tree* node);

  template <TreeCompatibleConcept T>
  EditionReference(T t) : EditionReference(static_cast<const Tree*>(t)) {}

  EditionReference(TypeBlock* blocks)
      : EditionReference(Tree::FromBlocks(blocks)) {}

#if POINCARE_MEMORY_TREE_LOG
  __attribute__((__used__)) void log() const;
#endif

  operator Tree*() const { return tree(); }
  Tree* operator->() { return tree(); }

  /* Comparison */
  inline bool operator==(const EditionReference& t) const {
    return m_identifier == t.identifier() ||
           (!isUninitialized() && !t.isUninitialized() && tree() == t.tree());
  }
  inline bool operator!=(const EditionReference& t) const {
    return m_identifier != t.identifier() &&
           (isUninitialized() || t.isUninitialized() || tree() != t.tree());
  }

  bool isUninitialized() const { return tree() == nullptr; }
  void uninitialize() {
    m_identifier = EditionPool::ReferenceTable::UninitializedOffset;
  }

  uint16_t identifier() const { return m_identifier; }

  typedef void (*InPlaceTreeFunction)(EditionReference reference);
  void recursivelyEdit(InPlaceTreeFunction treeFunction);

 private:
  EditionReference(const Tree* tree) : EditionReference(tree->clone()){};
  Tree* tree() const;
  uint16_t m_identifier;
};

inline bool operator==(Tree* n, const EditionReference& r) {
  return n == static_cast<Tree*>(r);
}

/* We have a semantical conflict between most our functions that want to alter a
 * tree where it is and the EditionReference update mechanism that invalidates
 * the reference when the tree it points to is replaced by something else. This
 * helper is used to associate to F(Tree *) that alters a tree in-place a
 * wrapper F(EditionReference &) that alters the tree pointed by the reference
 * and keeps the reference valid. */
template <class Result, class... Args>
inline Result ApplyPreservingReference(Result treeFunction(Tree*, Args...),
                                       EditionReference& ref, Args... args) {
  Tree* location = ref;
  Result result = treeFunction(location, args...);
  ref = location;
  return result;
}

/* Macros to define a method working with references from a method on tree */

/* No argument */
#define EDITION_REF_WRAP(F) \
  static bool F(EditionReference& r) { return ApplyPreservingReference(F, r); }

/* One argument */
#define EDITION_REF_WRAP_1(F, T)               \
  static bool F(EditionReference& r, T a1) {   \
    return ApplyPreservingReference(F, r, a1); \
  }

/* One argument with default value */
#define EDITION_REF_WRAP_1D(F, T, D)             \
  static bool F(EditionReference& r, T a1 = D) { \
    return ApplyPreservingReference(F, r, a1);   \
  }

/* Two argument */
#define EDITION_REF_WRAP_2(F, T1, T2)                \
  static bool F(EditionReference& r, T1 u1, T2 u2) { \
    return ApplyPreservingReference(F, r, u1, u2);   \
  }

void SwapTrees(EditionReference& u, EditionReference& v);
void CloneNodeAtNode(EditionReference& target, const Tree* nodeToClone);
void CloneTreeAtNode(EditionReference& target, const Tree* treeToClone);

void MoveNodeAtNode(EditionReference& target, Tree* nodeToMove);
void MoveTreeAtNode(EditionReference& target, Tree* treeToMove);

inline void MoveNodeOverTree(EditionReference& u, Tree* n) {
  u = u->moveNodeOverTree(n);
}

inline void MoveTreeOverTree(EditionReference& u, Tree* n) {
  u = u->moveTreeOverTree(n);
}

inline void MoveNodeOverNode(EditionReference& u, Tree* n) {
  u = u->moveNodeOverNode(n);
}

inline void MoveTreeOverNode(EditionReference& u, Tree* n) {
  u = u->moveTreeOverNode(n);
}

inline void CloneTreeOverNode(EditionReference& u, const Tree* n) {
  u = u->cloneTreeOverNode(n);
}

inline void CloneTreeOverTree(EditionReference& u, const Tree* n) {
  u = u->cloneTreeOverTree(n);
}

inline void CloneNodeOverNode(EditionReference& u, const Tree* n) {
  u = u->cloneNodeOverNode(n);
}

inline void CloneNodeOverTree(EditionReference& u, const Tree* n) {
  u = u->cloneNodeOverTree(n);
}

void SwapTrees(EditionReference& u, EditionReference& v);

}  // namespace PoincareJ

#endif
