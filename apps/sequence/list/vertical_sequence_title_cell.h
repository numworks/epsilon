#ifndef APPS_SEQUENCE_VERTICAL_SEQUENCE_TITLE_CELL_H
#define APPS_SEQUENCE_VERTICAL_SEQUENCE_TITLE_CELL_H

#include "../sequence_title_cell.h"

namespace Sequence {

class VerticalSequenceTitleCell : public SequenceTitleCell {
public:
  constexpr static KDFont::Size k_font = KDFont::Size::Large;

  VerticalSequenceTitleCell();
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void connectColorIndicator(bool status) { m_connectColorIndicator = status; }

private:
  constexpr static KDCoordinate k_equalWidthWithMargins = 10;  // Ad hoc value
  constexpr static float k_verticalOrientationHorizontalAlignment = 0.9f;

  KDRect subviewFrame() const override;
  void layoutSubviews(bool force = false) override;
  float verticalAlignment() const;
  float verticalAlignmentGivenExpressionBaselineAndRowHeight(KDCoordinate expressionBaseline, KDCoordinate rowHeight) const;

  bool m_connectColorIndicator;
};

}  // namespace Sequence

#endif
