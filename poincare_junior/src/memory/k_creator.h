#ifndef POINCARE_MEMORY_K_CREATOR_H
#define POINCARE_MEMORY_K_CREATOR_H

#include <omg/print.h>
#include <omgpj/concept.h>
#include <omgpj/print.h>

#include <array>

#include "node.h"
#include "placeholder.h"

namespace PoincareJ {

// https://stackoverflow.com/questions/40920149/is-it-possible-to-create-templated-user-defined-literals-literal-suffixes-for
// https://akrzemi1.wordpress.com/2012/10/29/user-defined-literals-part-iii/

/* These two abstract classes and their associated concepts are here to allow
 * templated functions using Tree to be called with any TreeCompatible which
 * then casted to Tree and its template arguments deduced. */

class AbstractTreeCompatible {};

template <class C>
concept TreeCompatibleConcept =
    Concept::is_derived_from<C, AbstractTreeCompatible>;

class AbstractTree : AbstractTreeCompatible {};

template <class C>
concept TreeConcept = Concept::is_derived_from<C, AbstractTree>;

/* The Tree template class is the compile time representation of a constexpr
 * tree. It's complete block representation is specified as template parameters
 * in order to be able to use the address of the static singleton (in flash) as
 * a Node*. It also eliminated identical trees since their are all using the
 * same specialized function.
 */

template <Block... Blocks>
class Tree : public AbstractTree {
 public:
  static constexpr Block k_blocks[] = {Blocks...};
  static constexpr size_t k_size = sizeof...(Blocks);
  // Enclose with TreeBorder Blocks when cast into Node* for navigation
  constexpr operator Node*() const {
    return Tree<BlockType::TreeBorder, Blocks...,
                BlockType::TreeBorder>::k_blocks +
           1;
  }
};

/* Helper to concatenate Trees */

/* Usage:
 * template <Block Tag, TreeConcept CT1, TreeConcept CT2> consteval auto
 * Binary(CT1, CT2) { return Concat<Tree<Tag>, CT1, CT2>();
 * }
 */

template <size_t N1, const Block B1[N1], size_t N2, const Block B2[N2],
          typename IS = decltype(std::make_index_sequence<N1 + N2>())>
struct __BlockConcat;

template <size_t N1, const Block B1[N1], size_t N2, const Block B2[N2],
          std::size_t... I>
struct __BlockConcat<N1, B1, N2, B2, std::index_sequence<I...>> {
  using tree = Tree<((I < N1) ? B1[I] : B2[I - N1])...>;
};

template <TreeConcept CT1, TreeConcept CT2>
using __ConcatTwo = typename __BlockConcat<CT1::k_size, CT1::k_blocks,
                                           CT2::k_size, CT2::k_blocks>::tree;

template <TreeConcept CT1, TreeConcept... CT>
struct Concat;
template <TreeConcept CT1>
struct Concat<CT1> : CT1 {};
template <TreeConcept CT1, TreeConcept... CT>
struct Concat : __ConcatTwo<CT1, Concat<CT...>> {};

// Helpers

template <Block Tag, Block... B1>
consteval auto KUnary(Tree<B1...>) {
  return Tree<Tag, B1...>();
}

template <Block Tag, TreeCompatibleConcept A>
consteval auto KUnary(A a) {
  return KUnary<Tag>(Tree(a));
}

template <Block Tag, Block... B1, Block... B2>
consteval auto KBinary(Tree<B1...>, Tree<B2...>) {
  return Tree<Tag, B1..., B2...>();
}

template <Block Tag, Block... B1, Block... B2, Block... B3>
consteval auto KTrinary(Tree<B1...>, Tree<B2...>, Tree<B3...>) {
  return Tree<Tag, B1..., B2..., B3...>();
}

template <Block Tag, TreeCompatibleConcept A, TreeCompatibleConcept B>
consteval auto KBinary(A a, B b) {
  return KBinary<Tag>(Tree(a), Tree(b));
}

template <Block Tag, TreeCompatibleConcept A, TreeCompatibleConcept B,
          TreeCompatibleConcept C>
consteval auto KTrinary(A a, B b, C c) {
  return KTrinary<Tag>(Tree(a), Tree(b), Tree(c));
}

template <Block Tag, TreeConcept... CTS>
static consteval auto __NAry(CTS...) {
  return Concat<Tree<Tag, sizeof...(CTS), Tag>, CTS...>();
}

template <Block Tag, TreeCompatibleConcept... CTS>
consteval auto KNAry(CTS... args) {
  return __NAry<Tag>(Tree(args)...);
}

/* The following dummy constructors are here to make the error message clearer
 * when someone tries to use a Node* inside a Tree constructor.
 * Without these you get "no matching function for call to 'Unary'" and details
 * on why each candidate concept is unmatched.
 * With these constructors, they match and then you get a "call to consteval
 * function ... is not a constant expression" since they are marked consteval.
 */

template <Block Tag>
consteval Node* KUnary(Node* a) {
  return Tree<>();
}

template <Block Tag>
consteval Node* KBinary(Node* a, Node* b) {
  return Tree<>();
}

template <Block Tag>
consteval Node KTrinary(Node a, Node b, Node c) {
  return Tree<>();
}

template <class... Args>
concept HasANodeConcept = (false || ... || std::is_same<Node*, Args>::value);
template <Block Tag, class... Args>
  requires HasANodeConcept<Args...>
consteval Node* KNAry(Args... args) {
  return Tree<>();
}

// String type used for templated string litterals

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
};

template <Placeholder::Tag Tag>
consteval auto KPlaceholder() {
  return Tree<BlockType::Placeholder,
              Placeholder::ParamsToValue(Tag, Placeholder::Filter::None),
              BlockType::Placeholder>();
}

template <Placeholder::Tag Tag>
consteval auto KAnyTreesPlaceholder() {
  return Tree<BlockType::Placeholder,
              Placeholder::ParamsToValue(Tag, Placeholder::Filter::AnyTrees),
              BlockType::Placeholder>();
}

}  // namespace PoincareJ

#endif
