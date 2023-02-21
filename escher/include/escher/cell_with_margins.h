#ifndef ESCHER_CELL_WITH_MARGINS_H
#define ESCHER_CELL_WITH_MARGINS_H

#include <escher/highlight_cell.h>

namespace Escher {

// TODO: inherit from AbstractHighlightCell

class CellWithMargins : public HighlightCell {
 public:
  CellWithMargins(HighlightCell* innerCell);

  // View
  KDSize minimalSizeForOptimalDisplay() const override;
  void drawRect(KDContext* ctx, KDRect rect) const override;

  // HighlightCell
  void setHighlighted(bool highlight) override;
  Responder* responder() override { return m_innerCell->responder(); }
  const char* text() const override { return m_innerCell->text(); }
  Poincare::Layout layout() const override { return m_innerCell->layout(); }

 private:
  // View
  int numberOfSubviews() const override { return 1; }
  HighlightCell* subviewAtIndex(int i) override { return m_innerCell; }
  void layoutSubviews(bool force) override;

  HighlightCell* m_innerCell;
};

}  // namespace Escher

#endif
