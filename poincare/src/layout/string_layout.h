#ifndef POINCARE_STRING_LAYOUT_H
#define POINCARE_STRING_LAYOUT_H

#include <poincare/expression_layout.h>

class StringLayout : public ExpressionLayout {
  public:
    StringLayout(const char * string);
    virtual void draw();
  private:
    const char * m_string;
};

#endif
