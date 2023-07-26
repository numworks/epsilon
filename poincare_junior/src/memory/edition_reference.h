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

// Helper to turn Tree* inplace editions into EditionReference*
template <class... Args>
inline bool Inplace(bool func(Tree*, Args...), EditionReference* ref,
                    Args... args) {
  Tree* previous = *ref;
  bool result = func(previous, args...);
  *ref = previous;
  return result;
}

/* We could define a variadic macro to add several wrappers at once. It is
 *  complicated but maybe worth ? */
#define INPLACE(F, ...)                              \
  static bool F(EditionReference* r) {               \
    return Inplace(F, r __VA_OPT__(, ) __VA_ARGS__); \
  }

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
