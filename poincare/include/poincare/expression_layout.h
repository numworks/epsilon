#ifndef POINCARE_EXPRESSION_LAYOUT_H
#define POINCARE_EXPRESSION_LAYOUT_H

extern "C" {
#include <kandinsky.h>
}

class ExpressionLayout {
  public:
    ExpressionLayout(ExpressionLayout * parent);
    void draw(KDPoint point);
    KDPoint origin();
    KDSize size();
  protected:
    virtual void render(KDPoint point) = 0;
    virtual KDSize computeSize() = 0;
    virtual ExpressionLayout * child(uint16_t index) = 0;
    virtual KDPoint positionOfChild(ExpressionLayout * child) = 0;
  private:
    KDPoint absoluteOrigin();
    //void computeLayout();//ExpressionLayout * parent, uint16_t childIndex);
    ExpressionLayout * m_parent;
    bool m_sized, m_positioned;
    KDRect m_frame;
};

#endif
