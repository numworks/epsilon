#ifndef SHARED_BUTTON_WITH_SEPARATOR_H
#define SHARED_BUTTON_WITH_SEPARATOR_H

#include <escher/button.h>
#include <escher/metric.h>

namespace Shared {

class ButtonWithSeparator : public Escher::Button {
public:
  ButtonWithSeparator(Responder * parentResponder, I18n::Message textBody, Escher::Invocation invocation);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
private:
  constexpr static KDCoordinate k_margin = 5;
  constexpr static KDCoordinate k_lineThickness = Escher::Metric::CellSeparatorThickness;
  void layoutSubviews(bool force = false) override;
};

}
#endif
