#ifndef POINCARE_EXPRESSION_H
#define POINCARE_EXPRESSION_H

#include <poincare/expression_layout.h>
#include <kandinsky.h>

class Context;

class Expression {
  public:
    enum class Type : uint8_t {
      Addition,
      Float,
      Fraction,
      Integer,
      Power,
      Product,
      Subtraction,
      Symbol
    };
    static Expression * parse(char const * string);
    virtual ~Expression();

    //virtual Expression ** children() = 0; // NULL-terminated

    virtual ExpressionLayout * createLayout(ExpressionLayout * parent) = 0; // Returned object must be deleted
    // TODO: Consider std::unique_ptr - see https://google-styleguide.googlecode.com/svn/trunk/cppguide.html#Ownership_and_Smart_Pointers

    /* identicalTo means strictly the same tree. For example, 3+5 is NOT identi-
     * cal to 5+3. Those are equal, but not identical. */
    //virtual bool identicalTo(Expression * e);
    Expression * simplify();

    virtual Type type() = 0;

    virtual float approximate(Context& context) = 0;
  /*private:
    void forEachChild(ExpressionAction);
    */
};

#endif
