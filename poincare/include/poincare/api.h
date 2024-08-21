#ifndef POINCARE_EXPRESSION_API_H
#define POINCARE_EXPRESSION_API_H

#include <omg/enums.h>
#include <poincare/layout.h>
#include <poincare/old/pool_handle.h>
#include <poincare/old/pool_object.h>
#include <poincare/src/expression/dimension_vector.h>
#include <poincare/src/memory/block.h>

#include "preferences.h"

namespace Poincare::Internal {
class Tree;
struct ContextTrees;
}  // namespace Poincare::Internal

namespace Poincare::API {

using Layout = Poincare::Layout;

class UserExpression;
class SystemExpression;
class SystemFunction;

class Symbol;

class JuniorPoolObject : PoolObject {
  friend class JuniorPoolHandle;

 public:
  JuniorPoolObject(const Internal::Tree* tree, size_t treeSize);
  // PoolObject
  size_t size() const override;
  int numberOfChildren() const override { return 0; }

#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "JuniorPoolObject";
  }
#endif

  // PCJ
  const Internal::Tree* tree() const;

 private:
  Internal::Block m_blocks[0];
};

class JuniorPoolHandle : public PoolHandle {
 public:
  JuniorPoolHandle() {}
  static JuniorPoolHandle Builder(const Internal::Tree* tree);
  // Eat the tree
  static JuniorPoolHandle Builder(Internal::Tree* tree);

  static JuniorPoolHandle Create(const Internal::Tree* structure,
                                 Internal::ContextTrees ctx);

  /* Reference */
  JuniorPoolObject* node() const {
    assert(identifier() != PoolObject::NoNodeIdentifier &&
           !PoolHandle::object()->isGhost());
    return static_cast<JuniorPoolObject*>(PoolHandle::object());
  }

  const Internal::Tree* tree() const {
    return isUninitialized() ? nullptr : node()->tree();
  }

  operator const Internal::Tree*() const { return tree(); }
};

/**
 * Settings when creating a layout from an expression
 *
 * @param linearMode Produce a rack layout containing only codepoints, that may
 *   be turned into a char * easily.
 *   Div(a,b) will be rendered as a/b instead of Fraction(a,b) for instance.
 */

struct LayoutFormat {
  bool linearMode = false;
  int8_t numberOfSignificantDigits = -1;
  Preferences::PrintFloatMode floatMode = Preferences::PrintFloatMode::Decimal;
  OMG::Base base = OMG::Base::Decimal;
};

#if 0
/* TODO */
class NewLayout : private JuniorPoolHandle {
 public:
  /* TODO */
  void render() {}
  /* TODO */
  UserExpression parse();
};
#endif

/**
 * Represent mathematical expression in form close to what users input and read.
 *
 * Used to:
 *   - display expressions via layouts
 *   - save user input as it have been parsed
 *   - make some simple enhancements
 *   - guess the context
 *
 * Equivalent to old `ReductionTarget::User` + Beautification
 * Dimension may be invalid.
 */
class UserExpression : public JuniorPoolHandle {
 public:
  // Builders
  static UserExpression Builder(const Internal::Tree* tree);
  // Eat the tree
  static UserExpression Builder(Internal::Tree* tree);

  static UserExpression FromFloat(float v);
  static UserExpression FromDouble(double v);
  static UserExpression FromInt(int v);
  static UserExpression FromSymbol(const char* name);

  static UserExpression Create(const Internal::Tree* structure,
                               Internal::ContextTrees ctx);

  /**
   * Create a layout to represent the expression
   *
   * Includes:
   *   - Adding parentheses if needed `Mult(Add(…,…),…)` → `(…+…)*…`
   *   - Chose the suitable multiplication symbol and apply it everywhere
   *   - Insert separators everywhere if they are needed somewhere
   *     For instance thousand separators if a integer has >= 5 digits
   */
  Layout createLayout(LayoutFormat format = {}) const;

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

  /**
   * Change complex format from Real to Complex if the expression contains "i".
   */
  void guessAndUpdateComplexFormat();

 private:
#if 0
  // TODO_PCJ: move in the object
  Internal::ComplexFormat m_complexFormat;
  Internal::ComplexFormat m_angleUnit;  // gathered under a Preference Context ?
#endif
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
class SystemExpression : public JuniorPoolHandle {
 public:
  // Builders
  static SystemExpression Builder(const Internal::Tree* tree,
                                  Internal::Units::SIVector dimension);
  // Eat the tree
  static SystemExpression Builder(Internal::Tree* tree,
                                  Internal::Units::SIVector dimension);
  /** Turn the expression back into a more user-friendly form.
   *
   * This includes:
   *   - Turning a*b^-1*c^-2 into a/(b*c^2)
   *   - Inserting the most appropriate unit
   */
  UserExpression beautified() const;  // + old target::User behavior

#if 0
  /** Return an expression where every known value has been replaced by a
   * floating point approximation. Mult(π,x) -> Mult(3.14159,x) */
  SystemExpression approximated();

  SystemExpression clone() const;

  SystemExpression advancedSimplified();
  SystemExpression expanded();

  SystemFunction preparedForApproximation(Symbol symbol);
#endif
  void setDimension(Internal::Units::SIVector dimension) {
    m_dimension = dimension;
  }

 private:
  Internal::Units::SIVector m_dimension;
};

/**
 * Compute as much stuff as possible before grapher or solver.
 *
 * Dimension is correct, only a scalars and points are allowed.
 */
class SystemFunction : private JuniorPoolHandle {
 public:
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

}  // namespace Poincare::API

#endif
