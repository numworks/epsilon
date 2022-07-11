#ifndef APPS_SEQUENCE_SEQUENCE_TITLE_CELL_H
#define APPS_SEQUENCE_SEQUENCE_TITLE_CELL_H

#include <escher/even_odd_expression_cell.h>
#include <apps/shared/function_title_cell.h>
#include <poincare/code_point_layout.h>

namespace Sequence {

class SequenceTitleCell : public Shared::FunctionTitleCell {
public:
  SequenceTitleCell(Shared::FunctionTitleCell::Orientation orientation = Shared::FunctionTitleCell::Orientation::HorizontalIndicator, KDFont::Size font = KDFont::Size::Large);
  void setLayout(Poincare::Layout layout);
  void setEven(bool even) override;
  void setHighlighted(bool highlight) override;
  void setColor(KDColor color) override;
  KDFont::Size font() const override {
    return m_titleTextView.font();
  }
  Poincare::Layout layout() const override {
    return m_titleTextView.layout();
  }
  void reloadCell() override;
private:
  constexpr static float k_horizontalOrientationAlignment = KDContext::k_alignCenter;
  constexpr static float k_verticalOrientationHorizontalAlignment = 0.9f;
  int numberOfSubviews() const override;
  Escher::View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  float verticalAlignmentGivenExpressionBaselineAndRowHeight(KDCoordinate expressionBaseline, KDCoordinate rowHeight) const override;
  Escher::EvenOddExpressionCell m_titleTextView;
};

}

#endif
