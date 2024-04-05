#ifndef POINCARE_MEMORY_PATTERN_MATCHING_H
#define POINCARE_MEMORY_PATTERN_MATCHING_H

#include <poincare_junior/src/memory/placeholder.h>

#include <array>

#include "k_tree.h"
#include "node_iterator.h"
#include "tree.h"

namespace PoincareJ {

struct ContextTrees {
  const Tree* KA = nullptr;
  const Tree* KB = nullptr;
  const Tree* KC = nullptr;
  const Tree* KD = nullptr;
  const Tree* KE = nullptr;
  const Tree* KF = nullptr;
  const Tree* KG = nullptr;
  const Tree* KH = nullptr;
};

class PatternMatching {
 public:
  class Context {
   public:
    Context() : m_array() {}
    Context(const ContextTrees& trees) : m_trees(trees) {
      for (int i = 0; i < Placeholder::Tag::NumberOfTags; i++) {
        if (m_array[i]) {
          m_numberOfTrees[i] = 1;
        }
#if ASSERTIONS
        m_isAnyTree[i] = false;
#endif
      }
    }
    template <Placeholder::Tag T>
    const Tree* getNode(KPlaceholder<T>) const {
      return getNode(T);
    }
    const Tree* getNode(uint8_t tag) const { return m_array[tag]; }
    uint8_t getNumberOfTrees(uint8_t tag) const { return m_numberOfTrees[tag]; }
    bool isAnyTree(uint8_t tag) const {
#if ASSERTIONS
      return m_isAnyTree[tag];
#else
      // Dummy value, unused anyway.
      return true;
#endif
    }
    template <Placeholder::Tag T>
    void setNode(KPlaceholder<T>, const Tree* node, uint8_t numberOfTrees,
                 bool isAnyTree) {
      return setNode(T, node, numberOfTrees, isAnyTree);
    }
    void setNode(uint8_t tag, const Tree* node, uint8_t numberOfTrees,
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

#if POINCARE_TREE_LOG
    __attribute__((__used__)) void log() const;
#endif

   private:
    union {
      ContextTrees m_trees;
      struct {
        const Tree* m_array[Placeholder::Tag::NumberOfTags];
        uint8_t m_numberOfTrees[Placeholder::Tag::NumberOfTags];
#if ASSERTIONS
        // Used only to assert AnyTreePlaceholders are properly used when
        // creating
        bool m_isAnyTree[Placeholder::Tag::NumberOfTags];
#endif
      };
      static_assert(sizeof(m_trees) == sizeof(m_array));
    };
  };

  static bool Match(const Tree* pattern, const Tree* source, Context* context);
  static Tree* Create(const Tree* structure, const Context context = Context(),
                      bool simplify = false) {
    return CreateTree(structure, context, nullptr, simplify);
  }
  static Tree* CreateAndSimplify(const Tree* structure, const Context context) {
    return CreateTree(structure, context, nullptr, true);
  }
  static Tree* Create(const Tree* structure, const ContextTrees& context,
                      bool simplify = false) {
    return Create(structure, Context(context), simplify);
  }
  static Tree* CreateAndSimplify(const Tree* structure,
                                 const ContextTrees& context) {
    return Create(structure, Context(context), true);
  }
  static Tree* MatchAndCreate(const Tree* source, const Tree* pattern,
                              const Tree* structure);
  // Return true if reference has been replaced
  static bool MatchAndReplace(Tree* node, const Tree* pattern,
                              const Tree* structure) {
    return PrivateMatchAndReplace(node, pattern, structure, false);
  }
  EDITION_REF_WRAP_2(MatchAndReplace, const Tree*, const Tree*);
  // Return true if reference has been replaced
  static bool MatchReplaceAndSimplify(Tree* node, const Tree* pattern,
                                      const Tree* structure) {
    return PrivateMatchAndReplace(node, pattern, structure, true);
  }
  EDITION_REF_WRAP_2(MatchReplaceAndSimplify, const Tree*, const Tree*);

 private:
  static bool PrivateMatchAndReplace(Tree* node, const Tree* pattern,
                                     const Tree* structure, bool simplify);

  /* During Match, MatchContext allow keeping track of matched Nary sizes.
   * It keeps track of both source and pattern.
   * For example, we want to prevent source Add(Mult(1,2),3) from matching with
   * pattern Add(Mult(1), 2, 3). At some point, local source will be Mult(1,2)
   * and local pattern will be Mult(1). */
  class MatchContext {
   public:
    MatchContext(const Tree* source, const Tree* pattern);
    bool reachedLimit(const Tree* node, bool global, bool source) const {
      return ReachedLimit(
          node, global ? (source ? m_globalSourceEnd : m_globalPatternEnd)
                       : (source ? m_localSourceEnd : m_localPatternEnd));
    }
    // Return the number of siblings right of node in local context.
    int remainingLocalTrees(const Tree* node) const;
    void setLocal(const Tree* source, const Tree* pattern);
    // Sets the local context to local root parents.
    void setLocalToParent();
#if POINCARE_TREE_LOG
    __attribute__((__used__)) void log() const;
#endif

   private:
    static bool ReachedLimit(const Tree* node, const Block* end) {
      assert(node->block() <= end);
      return node->block() == end;
    }

    // Local context
    const Tree* m_localSourceRoot;
    const Block* m_localSourceEnd;
    const Tree* m_localPatternRoot;
    const Block* m_localPatternEnd;
    // Global context
    const Tree* const m_globalSourceRoot;
    const Tree* const m_globalPatternRoot;
    const Block* const m_globalSourceEnd;
    const Block* const m_globalPatternEnd;
  };

  // Match an AnyTree Placeholder
  static bool MatchAnyTrees(Placeholder::Tag tag, const Tree* source,
                            const Tree* pattern, Context* context,
                            MatchContext matchContext);
  // Match source with pattern with given MatchContext constraints.
  static bool MatchNodes(const Tree* source, const Tree* pattern,
                         Context* context, MatchContext matchContext);
  // Create structure tree with context's placeholder nodes in TreeStack
  static Tree* CreateTree(const Tree* structure, const Context context,
                          Tree* insertedNAry, bool simplify);
  /* Return true if source has been matched after squashing pattern.
   * Note: this method can dirty the context if false is returned. */
  static bool MatchSourceWithSquashedPattern(const Tree* source,
                                             const Tree* pattern,
                                             Context* context);
};

namespace KTrees {
// Aliases for convenience
constexpr auto KA = KPlaceholder<Placeholder::Tag::A>();
constexpr auto KB = KPlaceholder<Placeholder::Tag::B>();
constexpr auto KC = KPlaceholder<Placeholder::Tag::C>();
constexpr auto KD = KPlaceholder<Placeholder::Tag::D>();
constexpr auto KE = KPlaceholder<Placeholder::Tag::E>();
constexpr auto KF = KPlaceholder<Placeholder::Tag::F>();
constexpr auto KG = KPlaceholder<Placeholder::Tag::G>();
constexpr auto KH = KPlaceholder<Placeholder::Tag::H>();

constexpr auto KA_p = KOneOrMorePlaceholder<Placeholder::Tag::A>();
constexpr auto KB_p = KOneOrMorePlaceholder<Placeholder::Tag::B>();
constexpr auto KC_p = KOneOrMorePlaceholder<Placeholder::Tag::C>();
constexpr auto KD_p = KOneOrMorePlaceholder<Placeholder::Tag::D>();
constexpr auto KE_p = KOneOrMorePlaceholder<Placeholder::Tag::E>();
constexpr auto KF_p = KOneOrMorePlaceholder<Placeholder::Tag::F>();
constexpr auto KG_p = KOneOrMorePlaceholder<Placeholder::Tag::G>();
constexpr auto KH_p = KOneOrMorePlaceholder<Placeholder::Tag::H>();

constexpr auto KA_s = KZeroOrMorePlaceholder<Placeholder::Tag::A>();
constexpr auto KB_s = KZeroOrMorePlaceholder<Placeholder::Tag::B>();
constexpr auto KC_s = KZeroOrMorePlaceholder<Placeholder::Tag::C>();
constexpr auto KD_s = KZeroOrMorePlaceholder<Placeholder::Tag::D>();
constexpr auto KE_s = KZeroOrMorePlaceholder<Placeholder::Tag::E>();
constexpr auto KF_s = KZeroOrMorePlaceholder<Placeholder::Tag::F>();
constexpr auto KG_s = KZeroOrMorePlaceholder<Placeholder::Tag::G>();
constexpr auto KH_s = KZeroOrMorePlaceholder<Placeholder::Tag::H>();
}  // namespace KTrees

}  // namespace PoincareJ

#endif
