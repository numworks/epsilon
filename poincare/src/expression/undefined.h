#ifndef POINCARE_EXPRESSION_UNDEFINED_H
#define POINCARE_EXPRESSION_UNDEFINED_H

#include <stdint.h>

namespace Poincare::Internal {

class Tree;

class Undefined {
 public:
  /* When an expression has multiple undefined children, we bubble up the
   * "biggest" one by default. */
  enum class Type : uint8_t {
    None = 0,
    // Nonreal,          // TODO_PR
    ZeroPowerZero,       // 0^0 -> Should be ZeroDivision ?
    ZeroDivision,        // 1/0, tan(nπ/2)
    UnhandledDimension,  // [[1,2]] + [[1],[2]]
    Unhandled,           // inf - inf, 0 * inf, unimplemented
    BadType,             // non-integers in gcd,lcm,...
    OutOfDefinition,     // arg(0)
    NotDefined,          // f(x) with f not defined
  };
  static Type GetType(const Tree* undefined);
  // Override Tree with Undefined tree.
  static void Set(Tree* e, Type type);
  static Tree* Push(Type type);
  static bool ShallowBubbleUpUndef(Tree* e);
};

}  // namespace Poincare::Internal

#endif
