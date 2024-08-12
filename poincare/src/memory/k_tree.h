#ifndef POINCARE_MEMORY_K_TREE_H
#define POINCARE_MEMORY_K_TREE_H

#include <omg/concept.h>
#include <omg/print.h>

#include <array>

#include "k_tree_concept.h"
#include "placeholder.h"
#include "tree.h"

namespace Poincare::Internal {
namespace KTrees {

// https://stackoverflow.com/questions/40920149/is-it-possible-to-create-templated-user-defined-literals-literal-suffixes-for
// https://akrzemi1.wordpress.com/2012/10/29/user-defined-literals-part-iii/

/* Tree has a false flexible member m_valueBlocks[0]. It can be used to access
 * and set the value blocks as expected but it cannot be filled in a constexpr
 * way since it is of size zero.
 * This sub-class adds an array member m_valueBlock[] of the suitable size.
 *
 * As an alternative, we could use a real flexible member directly in Tree but
 * Clang does not accept flexible members in base classes even when inherited
 * classes (Rack...) have no members. */
template <size_t N>
struct ConstexprTree : public Tree {
  template <class... Blocks>
  consteval ConstexprTree(Block type, Blocks... blocks)
      : Tree(static_cast<Type>(static_cast<uint8_t>(type))),
        m_valueBlocks{
            static_cast<ValueBlock>(static_cast<uint8_t>(blocks))...} {}

  ValueBlock m_valueBlocks[N - 1];  // 1 for the type, N-1 for the values
#if ASSERTIONS
  // Marker used by assertion to ensure we never access out
  ValueBlock m_border = static_cast<uint8_t>(Type::TreeBorder);
#endif
};

/* The KTree template class is the compile time representation of a constexpr
 * tree. It's complete block representation is specified as template parameters
 * in order to be able to use the address of the static singleton (in flash) as
 * a Tree*. It also eliminated identical trees since their are all using the
 * same specialized function. */
template <Block... Blocks>
struct KTree : public AbstractKTree {
  static constexpr size_t k_size = sizeof...(Blocks);
  static_assert(k_size > 0);
  static constexpr Block k_blocks[k_size] = {Blocks...};
  static constexpr ConstexprTree<k_size> k_tree{Blocks...};
  constexpr operator const Tree*() const { return &k_tree; }
  constexpr TypeBlock type() { return k_tree.type(); }
  const Tree* operator->() const { return operator const Tree*(); }
};

/* Helper to concatenate KTrees */

/* Usage:
 * template <Block Tag, KTreeConcept CT1, KTreeConcept CT2> consteval auto
 * Binary(CT1, CT2) { return Concat<Tree<Tag>, CT1, CT2>();
 * }
 */

template <size_t N1, const Block B1[N1], size_t N2, const Block B2[N2],
          typename IS = decltype(std::make_index_sequence<N1 + N2>())>
struct __BlockConcat;

template <size_t N1, const Block B1[N1], size_t N2, const Block B2[N2],
          std::size_t... I>
struct __BlockConcat<N1, B1, N2, B2, std::index_sequence<I...>> {
  using tree = KTree<((I < N1) ? B1[I] : B2[I - N1])...>;
};

template <KTreeConcept CT1, KTreeConcept CT2>
using __ConcatTwo = typename __BlockConcat<CT1::k_size, CT1::k_blocks,
                                           CT2::k_size, CT2::k_blocks>::tree;

template <KTreeConcept CT1, KTreeConcept... CT>
struct Concat;
template <KTreeConcept CT1>
struct Concat<CT1> : CT1 {};
template <KTreeConcept CT1, KTreeConcept... CT>
struct Concat : __ConcatTwo<CT1, Concat<CT...>> {};

// Helpers

template <Block Tag, Block... ExtraValues>
  requires(TypeBlock(Type(Tag.m_content)).nodeSize() ==
           sizeof...(ExtraValues) + 1)
struct KUnary : public KTree<Tag, ExtraValues...> {
  template <Block... B1>
  consteval auto operator()(KTree<B1...>) const {
    return KTree<Tag, ExtraValues..., B1...>();
  }

  /* The following dummy constructor is here to make the error message clearer
   * when someone tries to use a Tree* inside a KTree constructor.  Without
   * these you get "no matching function for call to 'Unary'" and details on
   * why each candidate concept is unmatched.  With these constructors, they
   * match and then you get a "call to consteval function ... is not a
   * constant expression" since they are marked consteval. */
  consteval const Tree* operator()(const Tree* a) const { return nullptr; }
};

template <Block Tag, Block... ExtraValues>
struct KBinary : public KTree<Tag, ExtraValues...> {
  template <Block... B1, Block... B2>
  consteval auto operator()(KTree<B1...>, KTree<B2...>) const {
    return KTree<Tag, ExtraValues..., B1..., B2...>();
  }

  consteval const Tree* operator()(const Tree* a, const Tree* b) const {
    return nullptr;
  }
};

template <class... Args>
concept HasATreeConcept = (false || ... ||
                           std::is_same<const Tree*, Args>::value);

template <size_t Nb, Block Tag, Block... ExtraValues>
struct KFixedArity : public KTree<Tag, ExtraValues...> {
  template <KTreeConcept... CTS>
    requires(sizeof...(CTS) == Nb)
  consteval auto operator()(CTS...) const {
    return Concat<KTree<Tag, ExtraValues...>, CTS...>();
  }

  template <class... Args>
    requires(HasATreeConcept<Args...> && sizeof...(Args) == Nb)
  consteval const Tree* operator()(Args... args) const {
    return nullptr;
  }
};

template <Block Tag, Block... ExtraValues>
struct KNAry {
  template <KTreeConcept... CTS>
  consteval auto operator()(CTS...) const {
    return Concat<decltype(node<sizeof...(CTS)>), CTS...>();
  }

  template <size_t Nb>
  static constexpr KTree<Tag, Nb, ExtraValues...> node{};

  template <class... Args>
    requires HasATreeConcept<Args...>
  consteval const Tree* operator()(Args... args) const {
    return nullptr;
  }
};

template <Block Tag>
struct KNAry16 {
  template <KTreeConcept... CTS>
  consteval auto operator()(CTS... args) const {
    return Concat<decltype(node<sizeof...(CTS)>), CTS...>();
  }

  template <size_t Nb>
  static constexpr KTree<Tag, Nb % 256, Nb / 256> node{};
};

// String type used for templated string literals

template <size_t N>
struct String {
  char m_data[N];
  constexpr size_t size() const { return N; }
  template <std::size_t... Is>
  constexpr String(const char (&arr)[N],
                   std::integer_sequence<std::size_t, Is...>)
      : m_data{arr[Is]...} {}
  constexpr String(char const (&arr)[N])
      : String(arr, std::make_integer_sequence<std::size_t, N>()) {}
  constexpr const char& operator[](std::size_t i) const { return m_data[i]; }
  consteval size_t codePointSize() const {
    size_t n = 0;
    for (char c : m_data) {
      // if not a continuation byte
      if ((c & 0b11000000) != 0b10000000) {
        n++;
      }
    }
    return n;
  }
  consteval char16_t codePointAt(std::size_t i) const {
    size_t k = 0;
    while (i--) {
      do {
        k++;
        // skip continuation bytes
      } while ((m_data[k] & 0b11000000) == 0b10000000);
    }
    if (!(m_data[k] & 0b10000000)) {
      return m_data[k];
    }
    if ((m_data[k] & 0b11100000) == 0b11000000) {
      return (m_data[k] & 0b00011111) << 6 | (m_data[k + 1] & 0b00111111);
    }
    if ((m_data[k] & 0b11110000) == 0b11100000) {
      return (m_data[k] & 0b00001111) << 12 |
             (m_data[k + 1] & 0b00111111) << 6 | (m_data[k + 2] & 0b00111111);
    }
    assert((m_data[k] & 0b11111000) == 0b11110000);
    return (m_data[k] & 0b00000111) << 18 | (m_data[k + 1] & 0b00111111) << 12 |
           (m_data[k + 2] & 0b00111111) << 6 | (m_data[k + 3] & 0b00111111);
  }
};

// KTree for Arbitrary

template <typename T, T data,
          typename IS = decltype(std::make_index_sequence<sizeof(T)>())>
struct _KArbitraryHelper;

template <typename T, T data, std::size_t... I>
struct _KArbitraryHelper<T, data, std::index_sequence<I...>>
    : KNAry<Type::Arbitrary, sizeof(T) & 0xFF, (sizeof(T) >> 8),
            std::bit_cast<std::array<uint8_t, sizeof(T)>>(data)[I]...> {};

template <typename T, T data>
constexpr auto KArbitrary = _KArbitraryHelper<T, data>();

// KTree for Placeholder

template <Placeholder::Tag T, Placeholder::Filter F>
struct KPlaceholderFilter
    : public KTree<Type::Placeholder, Placeholder::ParamsToValue(T, F)> {
  static constexpr Placeholder::Tag k_tag = T;
};

template <Placeholder::Tag Tag>
using KPlaceholder = KPlaceholderFilter<Tag, Placeholder::Filter::One>;

template <Placeholder::Tag Tag>
using KOneOrMorePlaceholder =
    KPlaceholderFilter<Tag, Placeholder::Filter::OneOrMore>;

template <Placeholder::Tag Tag>
using KZeroOrMorePlaceholder =
    KPlaceholderFilter<Tag, Placeholder::Filter::ZeroOrMore>;

}  // namespace KTrees

using namespace KTrees;
}  // namespace Poincare::Internal

#endif
