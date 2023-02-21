#ifndef SHARED_BUTTON_CELL_H
#define SHARED_BUTTON_CELL_H

#include <escher/abstract_button_cell.h>
#include <escher/metric.h>

namespace Escher {

class ButtonCell : public Escher::AbstractButtonCell {
 public:
  ButtonCell(Responder* parentResponder, I18n::Message textBody,
             Escher::Invocation invocation,
             KDColor backgroundColor = Escher::Palette::WallScreen,
             KDCoordinate horizontalMargins = 0);
  void drawRect(KDContext* ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;

  KDColor backgroundColor() const { return m_backgroundColor; }
  void setBackgroundColor(const KDColor backgroundColor) {
    m_backgroundColor = backgroundColor;
  }

 protected:
  constexpr static KDCoordinate k_lineThickness =
      Escher::Metric::CellSeparatorThickness;
  constexpr static KDCoordinate k_margin = Escher::Metric::CommonMenuMargin;

 private:
  void layoutSubviews(bool force = false) override;

  KDColor m_backgroundColor;
  KDCoordinate m_horizontalMargins;
};

}  // namespace Escher
#endif
