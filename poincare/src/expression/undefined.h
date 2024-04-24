#ifndef POINCARE_EXPRESSION_UNDEFINED_H
#define POINCARE_EXPRESSION_UNDEFINED_H

#include <poincare/src/memory/tree.h>
#include <poincare/src/memory/tree_stack.h>

namespace Poincare::Internal {

class Undefined {
 public:
  // TODO_PR: Order them by importance (NotDefined + Nonreal -> undef)
  enum class Type : uint8_t {
    None,
    // Nonreal,          // TODO_PR
    ZeroPowerZero,       // 0^0 -> Should be ZeroDivision ?
    ZeroDivision,        // 1/0, tan(nπ/2)
    UnhandledDimension,  // [[1,2]] + [[1],[2]]
    Unhandled,           // inf - inf, 0 * inf, unimplemented
    BadType,             // non-integers in gcd,lcm,...
    OutOfDefinition,     // arg(0)
    NotDefined,          // f(x) with f not defined
  };
  static Type GetType(const Tree* undefined) {
    return static_cast<Type>(undefined->nodeValue(0));
  }
  // Override Tree with Undefined tree.
  static void Set(Tree* e, Type type) {
    e->moveTreeOverTree(SharedTreeStack->push<Internal::Type::Undef>(type));
  }
  static Tree* Push(Type type) {
    return SharedTreeStack->push<Internal::Type::Undef>(type);
  }
};

}  // namespace Poincare::Internal

#endif
