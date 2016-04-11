#ifndef POINCARE_EXPRESSION_H
#define POINCARE_EXPRESSION_H

#include <poincare/expression_layout.h>
#include <kandinsky.h>

class Context;

class Expression {
  public:
    enum class Type : uint8_t {
      Addition,
      Cosine,
      Float,
      Integer,
      Fraction,
      Power,
      Product,
      Sine,
      Subtraction,
      Symbol,
      Tangent,
    };
    static Expression * parse(char const * string);
    virtual ~Expression();

    virtual ExpressionLayout * createLayout() = 0; // Returned object must be deleted
    virtual Expression * operand(int i) = 0;
    virtual int numberOfOperands() = 0;
    virtual Expression * clone() = 0;
    virtual Expression * cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands = true) = 0;

    // TODO: Consider std::unique_ptr - see https://google-styleguide.googlecode.com/svn/trunk/cppguide.html#Ownership_and_Smart_Pointers

    /* This tests whether two expressions are the same, this takes into account
     * commutativity of operators.
     *
     * For example 3+5 is identical to 5+3 but is not identical to 8.
     */
    bool isIdenticalTo(Expression * e);

    /* This tests whether two expressions are equivalent.
     * This is done by testing wheter they simplify to the same expression.
     *
     * For example:
     * - 3+5 and 4+4 are equivalent.
     * - (x+y)*z and x*z+y*z are equivalent.
     *
     * Here we assume that two equivalent expressions have the same
     * simplification, we don't really know whether that's the case,
     * nevertheless we are sure that if two expressions simplify to the same
     * expression they are indeed equivalent.
     */
    bool isEquivalentTo(Expression * e);

    /* Compare the value of two expressions.
     * This only make sense if the two values are of the same type
     */
    virtual bool valueEquals(Expression * e);
    Expression * simplify();

    virtual Type type() = 0;
    virtual bool isCommutative();

    virtual float approximate(Context& context) = 0;
  private:
    bool sequentialOperandsIdentity(Expression * e);
    bool commutativeOperandsIdentity(Expression * e);
    bool combinatoryCommutativeOperandsIdentity(Expression * e, bool * operandMatched,
        int leftToMatch);
};

#endif
