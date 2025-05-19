#ifndef POINCARE_MEMORY_PATTERN_MATCHING_H
#define POINCARE_MEMORY_PATTERN_MATCHING_H

#include <omg/always_false.h>
#include <omg/unreachable.h>
#include <poincare/src/expression/variables.h>

#include <array>

#include "k_tree.h"
#include "placeholder.h"
#include "tree.h"
#include "tree_ref.h"

namespace Poincare::Internal {

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

struct ContextScopes {
  uint8_t KA = 0;
  uint8_t KB = 0;
  uint8_t KC = 0;
  uint8_t KD = 0;
  uint8_t KE = 0;
  uint8_t KF = 0;
  uint8_t KG = 0;
  uint8_t KH = 0;
};

class PatternMatching {
 public:
  class Context {
   public:
    Context() : m_array() {}
    Context(const ContextTrees& trees,
            const ContextScopes& scopes = ContextScopes())
        : m_trees(trees)
#if ASSERTIONS
          ,
          m_scopes(scopes)
#endif
    {
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
    const Tree* getTree(KPlaceholder<T>) const {
      return getTree(T);
    }
    template <Placeholder::Tag T>
    const uint8_t getNumberOfTrees(KPlaceholder<T>) const {
      return getNumberOfTrees(T);
    }
    const Tree* getTree(uint8_t tag) const { return m_array[tag]; }
    uint8_t getNumberOfTrees(uint8_t tag) const { return m_numberOfTrees[tag]; }
    bool isAnyTree(uint8_t tag) const {
#if ASSERTIONS
      return m_isAnyTree[tag];
#else
      // Dummy value, unused anyway.
      return true;
#endif
    }
    uint8_t scope(uint8_t tag) const {
#if ASSERTIONS
      return m_scopeArray[tag];
#else
      // Dummy value, unused anyway.
      return 0;
#endif
    }
    template <Placeholder::Tag T>
    void setNode(KPlaceholder<T>, const Tree* node, uint8_t numberOfTrees,
                 bool isAnyTree, uint8_t scope = 0) {
      return setNode(T, node, numberOfTrees, isAnyTree, scope);
    }
    void setNode(uint8_t tag, const Tree* node, uint8_t numberOfTrees,
                 bool isAnyTree, uint8_t scope = 0) {
      assert(isAnyTree || numberOfTrees == 1);
      m_array[tag] = node;
      m_numberOfTrees[tag] = numberOfTrees;
#if ASSERTIONS
      m_isAnyTree[tag] = isAnyTree;
      m_scopeArray[tag] = scope;
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
      const Tree* m_array[Placeholder::Tag::NumberOfTags];
    };
    static_assert(sizeof(m_trees) == sizeof(m_array));
    uint8_t m_numberOfTrees[Placeholder::Tag::NumberOfTags];
#if ASSERTIONS
    // Used only to assert AnyTreePlaceholders are properly used when
    // creating
    bool m_isAnyTree[Placeholder::Tag::NumberOfTags];
    union {
      ContextScopes m_scopes;
      uint8_t m_scopeArray[Placeholder::Tag::NumberOfTags];
    };
    static_assert(sizeof(m_scopes) == sizeof(m_scopeArray));
#endif
  };

#ifndef PLATFORM_DEVICE
  template <KTreeConcept KT>
  static bool Match(KT source, const Tree* pattern, Context* context) {
    // Throw compile time error if source is a KTree
    static_assert(OMG::always_false<KT>,
                  "Match should not be called with a KTree as first argument, "
                  "source and pattern have probably been mixed up");
    OMG::unreachable();
  }
#endif

  static bool Match(const Tree* source, const Tree* pattern, Context* context);
  static Tree* Create(const Tree* structure, const Context context = Context(),
                      bool simplify = false) {
    return CreateTree(structure, context, nullptr, simplify, 0);
  }
  static Tree* CreateSimplify(const Tree* structure,
                              const Context context = Context()) {
    return CreateTree(structure, context, nullptr, true, 0);
  }
  static Tree* Create(const Tree* structure, const ContextTrees& context,
                      const ContextScopes& scopes = ContextScopes(),
                      bool simplify = false) {
    return Create(structure, Context(context, scopes), simplify);
  }
  static Tree* CreateSimplify(const Tree* structure,
                              const ContextTrees& context,
                              const ContextScopes scopes = ContextScopes()) {
    return Create(structure, Context(context, scopes), true);
  }
  static Tree* MatchCreate(const Tree* source, const Tree* pattern,
                           const Tree* structure);
  // Return true if reference has been replaced
  static bool MatchReplace(Tree* source, const Tree* pattern,
                           const Tree* structure, bool simplify = false) {
    return PrivateMatchReplace(source, pattern, structure, simplify);
  }
  // Return true if reference has been replaced
  static bool MatchReplaceSimplify(Tree* source, const Tree* pattern,
                                   const Tree* structure) {
    return PrivateMatchReplace(source, pattern, structure, true);
  }

 private:
  static bool PrivateMatchReplace(Tree* source, const Tree* pattern,
                                  const Tree* structure, bool simplify);

  static bool TrimSourceTree(Tree* source, Context* ctx);

  /* During Match, MatchContext allows keeping track of matched Nary sizes.
   * It keeps track of both source and pattern.
   * For example, we want to prevent source Add(Mult(1,2),3) from matching with
   * pattern Add(Mult(1), 2, 3). At some point, local source will be Mult(1,2)
   * and local pattern will be Mult(1). */
  class MatchContext {
   public:
    constexpr static uint8_t k_globalSource = 0;
    constexpr static uint8_t k_localSource = 1;
    constexpr static uint8_t k_globalPattern = 2;
    constexpr static uint8_t k_localPattern = 3;
    MatchContext(const Tree* source, const Tree* pattern,
                 uint8_t baseScope = 0);
    bool reachedLimit(const Tree* node, uint8_t whichLimit) const {
      return ReachedLimit(node, getEnd(whichLimit));
    }
    // Return the number of siblings right of node in local context.
    int remainingLocalTrees(const Tree* node) const;
    void setLocal(const Tree* source, const Tree* pattern);
    uint8_t getScope(const Tree* pattern) const;
    // Sets the local context to local root parents.
    void setLocalToParent();
#if POINCARE_TREE_LOG
    __attribute__((__used__)) void log() const;
#endif

   private:
    const Block* getEnd(uint8_t which) const {
      assert(which <= k_localPattern);
      switch (which) {
        case k_globalSource:
          return m_globalSourceEnd;
        case k_localSource:
          return m_localSourceEnd;
        case k_globalPattern:
          return m_globalPatternEnd;
        case k_localPattern:
          return m_localPatternEnd;
      }
      OMG::unreachable();
    }
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
    uint8_t m_baseScope = 0;
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
                          Tree* insertedNAry, bool simplify, uint8_t scope);
  /* Return true if source has been matched after squashing pattern.
   * Note: this method can dirty the context if false is returned. */
  static bool MatchSourceWithSquashedPattern(const Tree* source,
                                             const Tree* pattern,
                                             Context* context,
                                             const MatchContext& matchContext);
};  // namespace Poincare::Internal

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

}  // namespace Poincare::Internal

#endif
