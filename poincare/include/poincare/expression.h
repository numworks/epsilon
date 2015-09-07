#ifndef POINCARE_EXPRESSION_H
#define POINCARE_EXPRESSION_H

extern "C" {
#include <kandinsky.h>
}

void CreateFromString(char * string);

class Expression;
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
  private:
    void forEachChild(ExpressionAction);
};

#endif
