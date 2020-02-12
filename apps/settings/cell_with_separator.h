#ifndef SETTINGS_CELL_WITH_SEPARATOR_H
#define SETTINGS_CELL_WITH_SEPARATOR_H

#include <escher.h>

namespace Settings {

class CellWithSeparator : public HighlightCell {
public:
  CellWithSeparator() {}
  void setHighlighted(bool highlight) override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void reloadCell() override { cell()->reloadCell(); }
  Responder * responder() override { return cell()->responder(); }
  constexpr static KDCoordinate k_margin = 10;
private:
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  virtual HighlightCell * cell() = 0;
};

}

#endif
