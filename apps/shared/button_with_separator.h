#ifndef SHARED_BUTTON_WITH_SEPARATOR_H
#define SHARED_BUTTON_WITH_SEPARATOR_H

#include <escher/button.h>
#include <escher/metric.h>

namespace Shared {

class ButtonWithSeparator : public Escher::Button {
public:
  ButtonWithSeparator(Responder * parentResponder,
                      I18n::Message textBody,
                      Escher::Invocation invocation,
                      KDColor backgroundColor = Escher::Palette::WallScreen);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;

  KDColor backgroundColor() const { return m_backgroundColor; }
  void setBackgroundColor(const KDColor backgroundColor) { m_backgroundColor = backgroundColor; }

private:
  constexpr static KDCoordinate k_margin = Escher::Metric::CommonMenuMargin;
  constexpr static KDCoordinate k_lineThickness = Escher::Metric::CellSeparatorThickness;
  void layoutSubviews(bool force = false) override;

  KDColor m_backgroundColor;
};

}  // namespace Shared
#endif
