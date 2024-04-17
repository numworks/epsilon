#ifndef POINCARE_MEMORY_K_TREE_H
#define POINCARE_MEMORY_K_TREE_H

#include <omg/concept.h>
#include <omg/print.h>

#include <array>

#include "placeholder.h"
#include "tree.h"

namespace Poincare::Internal {
namespace KTrees {

// https://stackoverflow.com/questions/40920149/is-it-possible-to-create-templated-user-defined-literals-literal-suffixes-for
// https://akrzemi1.wordpress.com/2012/10/29/user-defined-literals-part-iii/

/* Concept that gathers all KTrees */

class AbstractKTree {};

template <class C>
concept KTreeConcept = OMG::Concept::is_derived_from<C, AbstractKTree>;

/* The KTree template class is the compile time representation of a constexpr
 * tree. It's complete block representation is specified as template parameters
 * in order to be able to use the address of the static singleton (in flash) as
 * a Tree*. It also eliminated identical trees since their are all using the
 * same specialized function.
 */

template <Block... Blocks>
class KTree : public AbstractKTree {
 public:
  static constexpr Block k_blocks[] = {Blocks...};
  static constexpr size_t k_size = sizeof...(Blocks);
  constexpr explicit operator const Block*() const {
#if ASSERTION
    // Close with TreeBorder Block when cast into Tree* for navigation
    return &Tree<Blocks..., Type::TreeBorder>::k_blocks[0];
#else
    return &k_blocks[0];
#endif
  }
  constexpr operator const Tree*() const {
    return Tree::FromBlocks(static_cast<const Block*>(*this));
  }
  constexpr TypeBlock type() {
    return TypeBlock(Type(static_cast<uint8_t>(k_blocks[0])));
  }
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
  consteval const Tree* operator()(const Tree* a) const { return KTree<>(); }
};

template <Block Tag, Block... ExtraValues>
struct KBinary : public KTree<Tag, ExtraValues...> {
  template <Block... B1, Block... B2>
  consteval auto operator()(KTree<B1...>, KTree<B2...>) const {
    return KTree<Tag, ExtraValues..., B1..., B2...>();
  }

  consteval const Tree* operator()(const Tree* a, const Tree* b) const {
    return KTree<>();
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
    return KTree<>();
  }
};

template <Block Tag>
struct KNAry {
  template <KTreeConcept... CTS>
  consteval auto operator()(CTS...) const {
    return Concat<decltype(node<sizeof...(CTS)>), CTS...>();
  }

  template <size_t Nb>
  static constexpr KTree<Tag, Nb> node{};

  template <class... Args>
    requires HasATreeConcept<Args...>
  consteval const Tree* operator()(Args... args) const {
    return KTree<>();
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
      if (!(c & 0b10000000) || ((c & 0b11100000) == 0b11000000)) {
        n++;
      }
    }
    return n;
  }
  consteval char16_t codePointAt(std::size_t i) const {
    size_t k = 0;
    while (i--) {
      char c = m_data[k];
      if (!(c & 0b10000000)) {
        k++;
      } else {
        assert((c & 0b11100000) == 0b11000000);
        k += 2;
      }
    }
    if (!(m_data[k] & 0b10000000)) {
      return m_data[k];
    }
    return (m_data[k] & 0b00011111) << 6 | (m_data[k + 1] & 0b00111111);
  }
};

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
