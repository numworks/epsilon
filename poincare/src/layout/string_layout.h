#ifndef POINCARE_STRING_LAYOUT_H
#define POINCARE_STRING_LAYOUT_H

#include <poincare/expression_layout.h>
#include <string.h>

class StringLayout : public ExpressionLayout {
  public:
    StringLayout(ExpressionLayout * parent, const char * string, size_t length);
    ~StringLayout();
  protected:
    void render(KDPoint point) override;
    KDSize computeSize() override;
    ExpressionLayout * child(uint16_t index) override;
    KDPoint positionOfChild(ExpressionLayout * child) override;
  private:
    char * m_string;
};

#endif
