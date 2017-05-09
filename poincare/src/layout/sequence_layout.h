#ifndef POINCARE_SEQUENCE_LAYOUT_H
#define POINCARE_SEQUENCE_LAYOUT_H

#include <poincare/expression.h>
#include <poincare/expression_layout.h>

namespace Poincare {

class SequenceLayout : public ExpressionLayout {
public:
  SequenceLayout(ExpressionLayout * lowerBoundLayout, ExpressionLayout * upperBoundLayout, ExpressionLayout * argumentLayout);
  ~SequenceLayout();
  SequenceLayout(const SequenceLayout& other) = delete;
  SequenceLayout(SequenceLayout&& other) = delete;
  SequenceLayout& operator=(const SequenceLayout& other) = delete;
  SequenceLayout& operator=(SequenceLayout&& other) = delete;
  constexpr static KDCoordinate k_symbolHeight = 15;
  constexpr static KDCoordinate k_symbolWidth = 9;
protected:
  constexpr static KDCoordinate k_boundHeightMargin = 2;
  ExpressionLayout * m_lowerBoundLayout;
  ExpressionLayout * m_upperBoundLayout;
  ExpressionLayout * m_argumentLayout;
private:
  KDSize computeSize() override;
  ExpressionLayout * child(uint16_t index) override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
  constexpr static KDCoordinate k_argumentWidthMargin = 2;
};

}

#endif
