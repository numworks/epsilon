#ifndef POINCARE_EXPRESSION_H
#define POINCARE_EXPRESSION_H

extern "C" {
#include <kandinsky.h>
}

class Expression {
  public:
    virtual void draw() = 0;
    virtual void layout() = 0;
    KDRect m_frame;
};

#endif
