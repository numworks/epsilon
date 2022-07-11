#ifndef APPS_SEQUENCE_SEQUENCE_TITLE_CELL_H
#define APPS_SEQUENCE_SEQUENCE_TITLE_CELL_H

#include <escher/even_odd_expression_cell.h>
#include <apps/shared/function_title_cell.h>
#include <poincare/code_point_layout.h>

namespace Sequence {

class SequenceTitleCell : public Shared::FunctionTitleCell {
public:
  SequenceTitleCell(KDFont::Size font = KDFont::Size::Small);
  void setLayout(Poincare::Layout layout);
  void setEven(bool even) override;
  void setHighlighted(bool highlight) override;
  void setColor(KDColor color) override;
  Poincare::Layout layout() const override {
    return m_titleTextView.layout();
  }
  void reloadCell() override;

protected:
  void layoutSubviews(bool force = false) override;

  Escher::EvenOddExpressionCell m_titleTextView;
private:
  constexpr static float k_horizontalOrientationAlignment = KDContext::k_alignCenter;

  int numberOfSubviews() const override;
  Escher::View * subviewAtIndex(int index) override;
};

}

#endif
