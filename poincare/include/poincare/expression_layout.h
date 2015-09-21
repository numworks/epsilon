#ifndef POINCARE_EXPRESSION_LAYOUT_H
#define POINCARE_EXPRESSION_LAYOUT_H

extern "C" {
#include <kandinsky.h>
}

class ExpressionLayout {
  public:
    virtual void draw() = 0;
    KDRect m_frame;
};

#endif
