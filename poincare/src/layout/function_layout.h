#ifndef POINCARE_FUNCTION_LAYOUT_H
#define POINCARE_FUNCTION_LAYOUT_H

#include <poincare/expression.h>
#include <poincare/expression_layout.h>

class FunctionLayout : public ExpressionLayout {
  public:
    FunctionLayout(ExpressionLayout * parent, char* function_name, Expression * arg);
    ~FunctionLayout();
  protected:
    void render(KDPoint point) override;
    KDSize computeSize() override;
    ExpressionLayout * child(uint16_t index) override;
    KDPoint positionOfChild(ExpressionLayout * child) override;
  private:
    ExpressionLayout * m_children[4];
};

#endif

