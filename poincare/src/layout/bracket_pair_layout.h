#ifndef POINCARE_BRACKET_PAIR_LAYOUT_H
#define POINCARE_BRACKET_PAIR_LAYOUT_H

#include <poincare/static_layout_hierarchy.h>
#include <poincare/print_float.h>

namespace Poincare {

class BracketPairLayout : public StaticLayoutHierarchy<1> {
public:
  using StaticLayoutHierarchy::StaticLayoutHierarchy;
  ExpressionLayout * clone() const override;
  void collapseSiblingsAndMoveCursor(ExpressionLayoutCursor * cursor) override;
  void deleteBeforeCursor(ExpressionLayoutCursor * cursor) override;
  bool moveLeft(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  bool moveRight(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override;
protected:
  ExpressionLayout * operandLayout();
  KDCoordinate externWidthMargin() const { return 2; }
  virtual KDCoordinate widthMargin() const { return 5; }
  virtual bool renderTopBar() const { return true; }
  virtual bool renderBottomBar() const { return true; }
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  void computeBaseline() override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
private:
  constexpr static KDCoordinate k_bracketWidth = 5;
  constexpr static KDCoordinate k_lineThickness = 1;
  constexpr static KDCoordinate k_verticalMargin = 1;
};

}

#endif

