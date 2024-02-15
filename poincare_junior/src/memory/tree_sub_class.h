#ifndef POINCARE_JUNIOR_MEMORY_TREE_SUB_CLASS_H
#define POINCARE_JUNIOR_MEMORY_TREE_SUB_CLASS_H

#include <poincare_junior/src/memory/tree.h>

#include <type_traits>

namespace PoincareJ {

/* Helper to be inherited to create NewType, a sub-type of Tree with the
 * restrictions that its siblings are of type NewType as well and its children
 * of type ChildType.
 *
 * NewType only has to provide a public static void Check(const Tree * tree)
 * that ensure a given tree is indeed a member of the NewType. */
template <class NewType, class ChildType>
struct TreeSubClass : Tree {
  /* TreeSubClass is defined as a class to be inherited instead of a template to
   * be aliased to avoid cluttering error messages with the alias definition. */

  static const NewType* From(const Tree* node) {
    /* These asserts are defined here and not in the class scope to allow usage
     * of this helper with incomplete types. */
    static_assert(sizeof(NewType) == sizeof(Tree));
    static_assert(sizeof(ChildType) == sizeof(Tree));
#ifndef PLATFORM_DEVICE
    static_assert(std::is_base_of_v<Tree, NewType>);
    static_assert(std::is_base_of_v<Tree, ChildType>);
#endif
    NewType::Check(node);
    return static_cast<const NewType*>(node);
  }

  static NewType* From(Tree* node) {
    NewType::Check(node);
    return static_cast<NewType*>(node);
  }

  const ChildType* child(int index) const {
    return static_cast<const ChildType*>(Tree::child(index));
  }

  ChildType* child(int index) {
    return static_cast<ChildType*>(Tree::child(index));
  }

  const NewType* nextTree() const { return NewType::From(Tree::nextTree()); }
  NewType* nextTree() { return NewType::From(Tree::nextTree()); }

  // TODO children(), parentOfDescendant() etc
};

/* TODO have a way (external to C++) to ensure that all our typing helpers do
 * not generate any code in release. */

}  // namespace PoincareJ

#endif
