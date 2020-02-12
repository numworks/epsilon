#ifndef SEQUENCE_SEQUENCE_TITLE_CELL_H
#define SEQUENCE_SEQUENCE_TITLE_CELL_H

#include "../shared/function_title_cell.h"
#include <poincare_layouts.h>

namespace Sequence {

class SequenceTitleCell : public Shared::FunctionTitleCell {
public:
  SequenceTitleCell();
  void setLayout(Poincare::Layout layout);
  void setEven(bool even) override;
  void setHighlighted(bool highlight) override;
  void setColor(KDColor color) override;
  void setOrientation(Orientation orientation) override;
  const KDFont * font() const override {
    return Poincare::CodePointLayoutNode::k_defaultFont;
  }
  Poincare::Layout layout() const override {
    return m_titleTextView.layout();
  }
private:
  static constexpr float k_horizontalOrientationAlignment = 0.5f;
  static constexpr float k_verticalOrientationHorizontalAlignment = 0.9f;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  float verticalAlignmentGivenExpressionBaselineAndRowHeight(KDCoordinate expressionBaseline, KDCoordinate rowHeight) const override;
  EvenOddExpressionCell m_titleTextView;
};

}

#endif
