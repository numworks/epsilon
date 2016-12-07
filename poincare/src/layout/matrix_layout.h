#ifndef POINCARE_MATRIX_LAYOUT_H
#define POINCARE_MATRIX_LAYOUT_H

#include <poincare/expression.h>
#include <poincare/expression_layout.h>

class MatrixLayout : public ExpressionLayout {
  public:
    MatrixLayout(ExpressionLayout ** entryLayouts, int numberOfRows, int numberOfColumns);
    ~MatrixLayout();
  protected:
    void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
    KDSize computeSize() override;
    ExpressionLayout * child(uint16_t index) override;
    KDPoint positionOfChild(ExpressionLayout * child) override;
  private:
    constexpr static KDCoordinate k_matrixEntryMargin = 4;
    constexpr static KDCoordinate k_matrixBracketMargin = 2;
    constexpr static KDCoordinate k_matrixBracketWidth = 2;
    KDCoordinate rowBaseline(int i);
    KDCoordinate rowHeight(int i);
    KDCoordinate height();
    KDCoordinate columnWidth(int j);
    KDCoordinate width();
    ExpressionLayout ** m_entryLayouts;
    int m_numberOfRows;
    int m_numberOfColumns;
};

#endif
