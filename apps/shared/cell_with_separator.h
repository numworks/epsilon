#ifndef SETTINGS_CELL_WITH_SEPARATOR_H
#define SETTINGS_CELL_WITH_SEPARATOR_H

#include <escher/metric.h>
#include <escher/table_cell.h>

namespace Shared {

class CellWithSeparator : public Escher::HighlightCell {
 public:
  CellWithSeparator() {}
  void setHighlighted(bool highlight) override;
  void drawRect(KDContext *ctx, KDRect rect) const override;
  void reloadCell() override { cell()->reloadCell(); }
  Escher::Responder *responder() override { return cell()->responder(); }
  KDSize minimalSizeForOptimalDisplay() const override;
  constexpr static KDCoordinate k_margin = Escher::Metric::CommonMenuMargin;
  constexpr static KDCoordinate k_lineThickness =
      Escher::Metric::CellSeparatorThickness;

 protected:
  virtual const Escher::TableCell *constCell() const = 0;

 private:
  int numberOfSubviews() const override;
  View *subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  Escher::TableCell *cell() {
    return const_cast<Escher::TableCell *>(constCell());
  }
  virtual bool separatorAboveCell() const { return true; }
};

}  // namespace Shared

#endif
