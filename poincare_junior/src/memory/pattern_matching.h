#ifndef POINCARE_MEMORY_PATTERN_MATCHING_H
#define POINCARE_MEMORY_PATTERN_MATCHING_H

#include <poincare_junior/src/memory/placeholder.h>

#include <array>

#include "k_creator.h"
#include "node.h"
#include "node_iterator.h"
#include "pool.h"

namespace PoincareJ {

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
    Context() : m_array() {}
    const Node* getNode(uint8_t tag) const { return m_array[tag]; }
    uint8_t getNumberOfTrees(uint8_t tag) const { return m_numberOfTrees[tag]; }
    bool isAnyTree(uint8_t tag) const {
#if ASSERTIONS
      return m_isAnyTree[tag];
#else
      // Dummy value, unused anyway.
      return true;
#endif
    }
    void setNode(uint8_t tag, const Node* node, uint8_t numberOfTrees,
                 bool isAnyTree) {
      assert(isAnyTree || numberOfTrees == 1);
      m_array[tag] = node;
      m_numberOfTrees[tag] = numberOfTrees;
#if ASSERTIONS
      m_isAnyTree[tag] = isAnyTree;
#endif
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
#if ASSERTIONS
    // Used only to assert AnyTreePlaceholders are properly used when creating
    bool m_isAnyTree[Placeholder::Tag::NumberOfTags];
#endif
  };

  static bool Match(const Node* pattern, const Node* source, Context* context);
  static Node* Create(const Node* structure,
                      const Context context = Context()) {
    return CreateTree(structure, context, nullptr);
  }
  static Node* MatchAndCreate(const Node* source, const Node* pattern,
                              const Node* structure);
  // Return true if reference has been replaced
  static bool MatchAndReplace(Node* node, const Node* pattern,
                              const Node* structure);

 private:
  /* During Match, MatchContext allow keeping track of matched Nary sizes.
   * It keeps track of both source and pattern.
   * For example, we want to prevent source Add(Mult(1,2),3) from matching with
   * pattern Add(Mult(1), 2, 3). At some point, local source will be Mult(1,2)
   * and local pattern will be Mult(1). */
  class MatchContext {
   public:
    MatchContext(const Node* source, const Node* pattern);
    bool reachedLimit(const Node* node, bool global, bool source) const {
      return ReachedLimit(
          node, global ? (source ? m_globalSourceEnd : m_globalPatternEnd)
                       : (source ? m_localSourceEnd : m_localPatternEnd));
    }
    // Return the number of siblings right of node in local context.
    int remainingLocalTrees(const Node* node) const;
    void setLocal(const Node* source, const Node* pattern);
    /* From a local pattern and source node, sets the local context (node's
     * parents) */
    void setLocalFromChild(const Node* source, const Node* pattern);

   private:
    static bool ReachedLimit(const Node* node, const TypeBlock* end) {
      assert(node->block() <= end);
      return node->block() == end;
    }

    // Local context
    const Node* m_localSourceRoot;
    const TypeBlock* m_localSourceEnd;
    const TypeBlock* m_localPatternEnd;
    // Global context
    const Node* m_globalSourceRoot;
    const Node* m_globalPatternRoot;
    const TypeBlock* m_globalSourceEnd;
    const TypeBlock* m_globalPatternEnd;
  };

  // Match an AnyTree Placeholder
  static bool MatchAnyTrees(Placeholder::Tag tag, const Node* source,
                            const Node* pattern, Context* context,
                            MatchContext matchContext);
  // Match source with pattern with given MatchContext constraints.
  static bool MatchNodes(const Node* source, const Node* pattern,
                         Context* context, MatchContext matchContext);
  // Create structure tree with context's placeholder nodes in EditionPool
  static Node* CreateTree(const Node* structure, const Context context,
                          Node* insertedNAry);
};

}  // namespace PoincareJ

#endif
