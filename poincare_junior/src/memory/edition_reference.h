#ifndef POINCARE_MEMORY_EDITION_REFERENCE_H
#define POINCARE_MEMORY_EDITION_REFERENCE_H

#include "edition_pool.h"
#include "tree.h"

namespace PoincareJ {

class EditionReference {
 public:
  EditionReference()
      : m_identifier(EditionPool::ReferenceTable::NoNodeIdentifier) {}
  EditionReference(Tree* node);
  EditionReference(const Tree* node);
  EditionReference(TypeBlock* blocks)
      : EditionReference(Tree::FromBlocks(blocks)) {}

#if POINCARE_MEMORY_TREE_LOG
  __attribute__((__used__)) void log() const;
#endif

  Tree* operator->() { return node(); }

  /* Comparison */
  inline bool operator==(const EditionReference& t) const {
    return m_identifier == t.identifier() ||
           (!isUninitialized() && !t.isUninitialized() && node() == t.node());
  }
  inline bool operator!=(const EditionReference& t) const {
    return m_identifier != t.identifier() &&
           (isUninitialized() || t.isUninitialized() || node() != t.node());
  }

  operator Tree*() const { return node(); }
  bool isUninitialized() const { return node() == nullptr; }
  void uninitialize() {
    m_identifier = EditionPool::ReferenceTable::UninitializedOffset;
  }

  uint16_t identifier() const { return m_identifier; }

  typedef void (*InPlaceTreeFunction)(EditionReference reference);
  void recursivelyEdit(InPlaceTreeFunction treeFunction);

 private:
  Tree* node() const;
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

bool MatchAndReplace(EditionReference& target, const Tree* pattern,
                     const Tree* structure);

void SwapTrees(EditionReference* u, EditionReference* v);
void CloneNodeAtNode(EditionReference* target, const Tree* nodeToClone);
void CloneTreeAtNode(EditionReference* target, const Tree* treeToClone);

void MoveNodeAtNode(EditionReference* target, Tree* nodeToMove);
void MoveTreeAtNode(EditionReference* target, Tree* treeToMove);

inline void MoveNodeOverTree(EditionReference* u, Tree* n) {
  *u = (*u)->moveNodeOverTree(n);
}

inline void MoveTreeOverTree(EditionReference* u, Tree* n) {
  *u = (*u)->moveTreeOverTree(n);
}

inline void MoveNodeOverNode(EditionReference* u, Tree* n) {
  *u = (*u)->moveNodeOverNode(n);
}

inline void MoveTreeOverNode(EditionReference* u, Tree* n) {
  *u = (*u)->moveTreeOverNode(n);
}

inline void CloneTreeOverNode(EditionReference* u, const Tree* n) {
  *u = (*u)->cloneTreeOverNode(n);
}

inline void CloneTreeOverTree(EditionReference* u, const Tree* n) {
  *u = (*u)->cloneTreeOverTree(n);
}

inline void CloneNodeOverNode(EditionReference* u, const Tree* n) {
  *u = (*u)->cloneNodeOverNode(n);
}

inline void CloneNodeOverTree(EditionReference* u, const Tree* n) {
  *u = (*u)->cloneNodeOverTree(n);
}

}  // namespace PoincareJ

#endif
