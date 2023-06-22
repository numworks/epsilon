#ifndef POINCARE_MEMORY_PATTERN_MATCHING_H
#define POINCARE_MEMORY_PATTERN_MATCHING_H

#include <poincare_junior/src/memory/placeholder.h>

#include <array>

#include "edition_reference.h"
#include "k_creator.h"
#include "node.h"
#include "node_iterator.h"
#include "pool.h"

namespace PoincareJ {

/* TODO: Profile the code to decide whether to:
 * - reduce node size by unsymmetrizing them but forbid the use of parent
 * - replace parentOfDescendant by an easier call to parent()
 * */

class PatternMatching {
  /* TODO : Force and ensure via assertions that AnyTrees filter are also used
   * in CreateTree's structure. This could be stored in a boolean array here and
   * checked on creation.
   * This would prevent mistakes such as replacing
   * KMult(KPlaceholder<A>(),KAdd(KPlaceholder<B>(),KAnyTreesPlaceholder<C>()))
   * with
   * KAdd(KMult(KPlaceholder<A>(), KPlaceholder<B>()),
   *      KMult(KPlaceholder<A>(), KPlaceholder<C>()))
   * instead of
   * KAdd(KMult(KPlaceholder<A>(), KPlaceholder<B>()),
   *      KMult(KPlaceholder<A>(), KAdd(KAnyTreesPlaceholder<C>())))
   * Such assertion might not be possible anymore if we remove filters for an
   * optional filter method in Matching.
   */
 public:
  class Context {
   public:
    const Node* getNode(uint8_t tag) const { return m_array[tag]; }
    uint8_t getNumberOfTrees(uint8_t tag) const { return m_numberOfTrees[tag]; }
    void setNode(uint8_t tag, const Node* node, uint8_t numberOfTrees) {
      m_array[tag] = node;
      m_numberOfTrees[tag] = numberOfTrees;
    }
    void setNumberOfTrees(uint8_t tag, uint8_t numberOfTrees) {
      m_numberOfTrees[tag] = numberOfTrees;
    }
    bool isUninitialized() const;

#if POINCARE_MEMORY_TREE_LOG
    __attribute__((__used__)) void log() const;
#endif

   private:
    const Node* m_array[Placeholder::Tag::NumberOfTags];
    uint8_t m_numberOfTrees[Placeholder::Tag::NumberOfTags];
  };

  static bool Match(const Node* pattern, const Node* source, Context* context);
  static EditionReference Create(const Node* structure,
                                 const Context context = Context()) {
    return CreateTree(structure, context, nullptr);
  }

 private:
  /* During Match, MatchContext allow keeping track of matched Nary sizes.
   * It keeps track of both source and pattern.
   * For example, we want to prevent source Add(Mult(1,2),3) from matching with
   * pattern Add(Mult(1), 2, 3). At some point, local source will be Mult(1,2)
   * and local pattern will be Mult(1). */
  class MatchContext {
   public:
    MatchContext(Node* source, Node* pattern);
    bool reachedLimit(Node* node, bool global, bool source) const {
      return ReachedLimit(
          node, global ? (source ? m_globalSourceEnd : m_globalPatternEnd)
                       : (source ? m_localSourceEnd : m_localPatternEnd));
    }
    // Return the number of siblings right of node in local context.
    int remainingLocalTrees(Node* node) const;
    void setLocal(Node* source, Node* pattern);
    /* From a local pattern and source node, sets the local context (node's
     * parents) */
    void setLocalFromChild(Node* source, Node* pattern);

   private:
    static bool ReachedLimit(Node* node, const TypeBlock* end) {
      assert(node->block() <= end);
      return node->block() == end;
    }

    // Local context
    Node* m_localSourceRoot;
    TypeBlock* m_localSourceEnd;
    TypeBlock* m_localPatternEnd;
    // Global context
    const Node* m_globalSourceRoot;
    const Node* m_globalPatternRoot;
    const TypeBlock* m_globalSourceEnd;
    const TypeBlock* m_globalPatternEnd;
  };

  // Match an AnyTree Placeholder
  static bool MatchAnyTrees(Placeholder::Tag tag, Node* source, Node* pattern,
                            Context* context, MatchContext matchContext);
  // Match source with pattern with given MatchContext constraints.
  static bool MatchNodes(Node* source, Node* pattern, Context* context,
                         MatchContext matchContext);
  // Create structure tree with context's placeholder nodes in EditionPool
  static EditionReference CreateTree(const Node* structure,
                                     const Context context, Node* insertedNAry);
};

}  // namespace PoincareJ

#endif
