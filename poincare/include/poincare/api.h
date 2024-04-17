#ifndef POINCARE_EXPRESSION_API_H
#define POINCARE_EXPRESSION_API_H

#include <poincare/old/pool_handle.h>
#include <poincare/old/pool_object.h>
#include <poincare/src/memory/block.h>

namespace Poincare {

class Internal::Tree;
struct Internal::ContextTrees;

class Layout;

class UserExpression;
class SystemExpression;
class SystemFunction;

class Dimension;
class Symbol;

class JuniorPoolObject : PoolObject {
  friend class JuniorPoolHandle;

 public:
  JuniorPoolObject(const Internal::Tree* tree, size_t treeSize);
  // PoolObject
  size_t size() const override;
  int numberOfChildren() const override { return 0; }

 private:
  // PCJ
  const Internal::Tree* tree() const;

  Internal::Block m_blocks[0];
}

class JuniorPoolHandle : public PoolHandle {
  JuniorPoolHandle() {}
  static JuniorPoolHandle Builder(const Internal::Tree* tree);
  // Eat the tree
  static JuniorPoolHandle Builder(Internal::Tree* tree);

  static JuniorExpression Create(const Internal::Tree* structure,
                                 Internal::ContextTrees ctx);

  const Internal::Tree* tree() const {
    return isUninitialized() ? nullptr : node()->tree();
  }
}

/* TODO */
class NewLayout : private JuniorPoolHandle {
  /* TODO */
  void render() {}
  /* TODO */
  UserExpression parse();
};

/**
 * Used to:
 *   - display expressions via layouts
 *   - save user input as it have been parsed
 *   - make some simple enhancements
 *   - guess the context
 *
 * Equivalent to old ReductionTarget::User + Beautification
 * Dimension may be invalid.
 */
class UserExpression : private JuniorPoolHandle {
  /**
   * Create a layout to represent the expression
   *
   * Includes:
   *   - Adding parentheses if needed Mult(Add(…,…),…) -> (…+…)*…
   *   - Chose the suitable multiplication symbol and apply it everywhere
   *   - Insert separators everywhere if they are needed somewhere
   *     For instance thousand separators if a integer has >= 5 digits
   *
   * @param linearMode Produce a rack layout containing only codepoints, that
   *   may be turned into a char * easily.
   *   Div(a,b) will be rendered as a/b instead of Fraction(a,b) for instance.
   */
  Layout createLayout(bool linearMode) const;

  /**
   * Turn the UserExpression into a SystemExpression
   *
   * This includes :
   *   - Check that the dimension is correct
   *   - Normalize some types to simpler equivalents
   *     Sub(a,b) -> Add(a, Mult(-1, b))
   *   - Index local variables with an id instead of a name
   *   - Replace unit by their ratio in SI e.g. cm -> 1/100
   *   - Apply SystematicReduction
   *
   * TODO: what if the dimension is invalid ?
   */
  SystemExpression projected() const;
};

/**
 * Used to:
 *   - manipulate the expression
 *   - ask for advanced simplifications
 *   - save expression in an already processed and efficient form
 *
 * Equivalent to old ReductionTarget::SystemForAnalysis once reduced.
 * Dimension is consistent and may be asked at any time.
 */
class SystemExpression : private JuniorPoolHandle {
  /** Turn the expression back into a more user-friendly form.
   *
   * This includes:
   *   - Turning a*b^-1*c^-2 into a/(b*c^2)
   *   - Inserting the most appropriate unit
   */
  UserExpression beautified() const;  // + old target::User behavior

  /** Return an expression where every known value has been replaced by a
   * floating point approximation. Mult(π,x) -> Mult(3.14159,x) */
  SystemExpression approximated();

  SystemExpression clone() const;

  SystemExpression advancedSimplified();
  SystemExpression expanded();

  SystemFunction preparedForApproximation(Symbol symbol);
};

/**
 * Compute as much stuff as possible before grapher or solver.
 *
 * Dimension is correct, only a scalars and points are allowed.
 */
class SystemFunction : private JuniorPoolHandle {
  /* A projected tree with a global VarX and dim = scalar */
  double evaluate(double x);

 private:
  // name of the unique variable ?
};

/* OR specialize function with its return type : */

/* should be approximable with a const Tree * */
/* ex reduction target forApproximation */
/* correct dimension, only a subset are allowed */
class SystemCartesianFunction {
  /* A projected tree with a global VarX and dim = scalar */
  double evaluate(double x);

 private:
  // name of the unique variable ?
};

class SystemParametricFunction : private JuniorPoolHandle {
  /* A projected tree with a global VarX and dim = point */
  std::pair<double, double> evaluate(double t);

 private:
  // name of the unique variable ?
};

/* TODO:
   derivative on function or SE ?
   don't forget to prepare for approx in a integral approx
 */

}  // namespace Poincare

#endif
