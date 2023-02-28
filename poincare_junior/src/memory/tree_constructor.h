#ifndef POINCARE_MEMORY_TREE_CONSTRUCTOR_H
#define POINCARE_MEMORY_TREE_CONSTRUCTOR_H

#include <array>
#include "node.h"
#include <omgpj/assert.h>
#include <omgpj/concept.h>
#include <omg/print.h>
#include <omgpj/print.h>

namespace PoincareJ {

// https://stackoverflow.com/questions/40920149/is-it-possible-to-create-templated-user-defined-literals-literal-suffixes-for
// https://akrzemi1.wordpress.com/2012/10/29/user-defined-literals-part-iii/

/* These two abstract classes and their associated concepts are here to allow
 * templated functions using Tree to be called with any TreeCompatible which
 * then casted to Tree and its template arguments deduced. */

class AbstractTreeCompatible {};

template <class C> concept TreeCompatibleConcept = Concept::is_derived_from<C, AbstractTreeCompatible>;

class AbstractTree : AbstractTreeCompatible {};

template <class C> concept TreeConcept = Concept::is_derived_from<C, AbstractTree>;


/* The Tree template class is the compile time representation of a constexpr
 * tree. It's complete block representation is specified as template parameters
 * in order to be able to use the address of the static singleton (in flash) as
 * a Node. It also eliminated identical trees since their are all using the same
 * specialized function.
 */

template <Block... Blocks>
class Tree : public AbstractTree {
public:
  static constexpr Block k_blocks[] = { Blocks... };
  static constexpr size_t k_size = sizeof...(Blocks);
  constexpr operator Node () const { return k_blocks; }
};


/* Helper to concatenate Trees */

/* Usage:
 * template <Block Tag, TreeConcept CT1, TreeConcept CT2> consteval auto Binary(CT1, CT2) {
 *   return Concat<Tree<Tag>, CT1, CT2>();
 * }
 */

template <size_t N1, const Block B1[N1], size_t N2, const Block B2[N2], typename IS = decltype(std::make_index_sequence<N1 + N2>())> struct __BlockConcat;

template <size_t N1, const Block B1[N1], size_t N2, const Block B2[N2], std::size_t... I>
struct __BlockConcat<N1, B1, N2, B2, std::index_sequence<I...>> {
  using tree = Tree<((I < N1) ? B1[I] : B2[I - N1])...>;
};

template <TreeConcept CT1, TreeConcept CT2> using __ConcatTwo = typename __BlockConcat<CT1::k_size, CT1::k_blocks, CT2::k_size, CT2::k_blocks>::tree;

template <TreeConcept CT1, TreeConcept... CT> struct Concat;
template <TreeConcept CT1> struct Concat<CT1> : CT1 {};
template <TreeConcept CT1, TreeConcept... CT> struct Concat : __ConcatTwo<CT1, Concat<CT...>> {};


// Helpers

template <Block Tag, Block... B1> consteval auto Unary(Tree<B1...>) {
  return Tree<Tag, B1...>();
}

template <Block Tag, TreeCompatibleConcept A> consteval auto Unary(A a) {
  return Unary<Tag>(Tree(a));
}

template <Block Tag, Block... B1, Block... B2> consteval auto Binary(Tree<B1...>, Tree<B2...>) {
  return Tree<Tag, B1..., B2...>();
}

template <Block Tag, TreeCompatibleConcept A, TreeCompatibleConcept B> consteval auto Binary(A a, B b) {
  return Binary<Tag>(Tree(a), Tree(b));
}

template<Block Tag, TreeConcept ...CTS> static consteval auto __NAry(CTS...) {
  return Concat<Tree<Tag, sizeof...(CTS), Tag>, CTS...>();
}

template <Block Tag, TreeCompatibleConcept ...CTS> consteval auto NAry(CTS... args) { return __NAry<Tag>(Tree(args)...); }


/* The following dummy constructors are here to make the error message clearer
 * when someone tries to use a Node inside a Tree constructor.
 * Without these you get "no matching function for call to 'Unary'" and details
 * on why each candidate concept is unmatched.
 * With these constructors, they match and then you get a "call to consteval
 * function ... is not a constant expression" since they are marked consteval.
 */

template <Block Tag> consteval Node Unary(Node a) { return Tree<>(); }

template <Block Tag> consteval Node Binary(Node a, Node b) { return Tree<>(); }

template <class...Args> concept HasANodeConcept = (false || ... || std::is_same<Node, Args>::value);
template <Block Tag, class...Args> requires HasANodeConcept<Args...> consteval Node NAry(Args...args) { return Tree<>(); }


// String type used for templated string litterals

template<size_t N>
struct String {
  char m_data[N];
  constexpr size_t size() const { return N; }
  template <std::size_t... Is>
  constexpr String(const char (&arr)[N], std::integer_sequence<std::size_t, Is...>) : m_data{arr[Is]...} {}
  constexpr String(char const(&arr)[N]) : String(arr, std::make_integer_sequence<std::size_t, N>()) {}
  constexpr const char & operator[](std::size_t i) const { return m_data[i]; }
};



}

#endif
