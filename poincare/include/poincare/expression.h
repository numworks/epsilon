#ifndef POINCARE_EXPRESSION_H
#define POINCARE_EXPRESSION_H

#include <poincare/expression_layout.h>
#include <kandinsky.h>

class Context;

class Expression {
  public:
    enum class Type : uint8_t {
      AbsoluteValue,
      Addition,
      Cosine,
      Float,
      Integer,
      Logarithm,
      Matrix,
      NthRoot,
      Fraction,
      Parenthesis,
      Power,
      Product,
      Sine,
      Subtraction,
      Symbol,
      Tangent,
    };
    static Expression * parse(char const * string);
    virtual ~Expression();

    virtual ExpressionLayout * createLayout() const = 0; // Returned object must be deleted
    virtual const Expression * operand(int i) const = 0;
    virtual int numberOfOperands() const = 0;
    virtual Expression * clone() const = 0;
    virtual Expression * cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands = true) const = 0;

    // TODO: Consider std::unique_ptr - see https://google-styleguide.googlecode.com/svn/trunk/cppguide.html#Ownership_and_Smart_Pointers

    /* This tests whether two expressions are the same, this takes into account
     * commutativity of operators.
     *
     * For example 3+5 is identical to 5+3 but is not identical to 8.
     */
    bool isIdenticalTo(const Expression * e) const;

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
    bool isEquivalentTo(Expression * e) const;

    /* Compare the value of two expressions.
     * This only make sense if the two values are of the same type
     */
    virtual bool valueEquals(const Expression * e) const;
    Expression * simplify() const;

    virtual Type type() const = 0;
    virtual bool isCommutative() const;
    /* The function evaluate creates a new expression and thus mallocs memory.
     * Do not forget to delete the new expression to avoid leaking. */
    virtual Expression * evaluate(Context& context) const = 0;
    virtual float approximate(Context& context) const = 0;
    virtual int writeTextInBuffer(char * buffer, int bufferSize);
  private:
    bool sequentialOperandsIdentity(const Expression * e) const;
    bool commutativeOperandsIdentity(const Expression * e) const;
    bool combinatoryCommutativeOperandsIdentity(const Expression * e,
        bool * operandMatched, int leftToMatch) const;
};

#endif
