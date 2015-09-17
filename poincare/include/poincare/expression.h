#ifndef POINCARE_EXPRESSION_H
#define POINCARE_EXPRESSION_H

extern "C" {
#include <kandinsky.h>
}

void CreateFromString(char * string);

class Expression;
class Integer;
typedef void (Expression::*ExpressionAction)(void);

class Expression {
  public:
    static Expression * parse(char * string);
    void recursiveDraw();
    void recursiveLayout();
    KDRect m_frame;
    virtual Expression ** children() = 0; // NULL-terminated
    virtual void draw();
    virtual void layout();
    /* identicalTo means strictly the same tree. For example, 3+5 is NOT identi-
     * cal to 5+3. Those are equal, but not identical. */
    virtual bool identicalTo(Expression * e);
    //virtual Integer approximate();
    //virtual Expression * simplify();

    virtual float approximate() = 0;
  private:
    void forEachChild(ExpressionAction);
};

#endif
