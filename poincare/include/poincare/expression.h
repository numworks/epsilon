#ifndef POINCARE_EXPRESSION_H
#define POINCARE_EXPRESSION_H

#include <poincare/expression_layout.h>
#include <kandinsky.h>

#ifdef DEBUG
#define PRINT_AST(e)  e->__printAst(0)
#else
#define PRINT_AST(e) ((void)0)
#endif

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

    // TODO: Consider std::unique_ptr - see https://google-styleguide.googlecode.com/svn/trunk/cppguide.html#Ownership_and_Smart_Pointers

    /* identicalTo means strictly the same tree. For example, 3+5 is NOT identi-
     * cal to 5+3. Those are equal, but not identical. */
    bool isIdenticalTo(Expression * e);
    // Compare the value of two expressions.
    // This only make sense if the two values are of the same type
    virtual bool valueEquals(Expression * e);
    Expression * simplify();

    virtual Type type() = 0;

    virtual float approximate(Context& context) = 0;
  /*private:
    void forEachChild(ExpressionAction);
    */
#ifdef DEBUG
    virtual int getPrintableVersion(char* txt) = 0;
    void __printAst(int level=0);
#endif
};

#endif
