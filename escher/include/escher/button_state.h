#ifndef ESCHER_BUTTON_STATE_H
#define ESCHER_BUTTON_STATE_H

#include <escher/button.h>
#include <escher/toggleable_dot_view.h>

class ButtonState : public Button {
public:
  using Button::Button;
  void setState(bool state) { m_stateView.setState(state); }
  KDSize minimalSizeForOptimalDisplay() const override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  // Dot right margin.
  constexpr static KDCoordinate k_stateMargin = 9;
  // Dot vertical position offset.
  constexpr static KDCoordinate k_verticalOffset = 5;
  int numberOfSubviews() const override { return 2; }
  View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  ToggleableDotView m_stateView;
};

#endif
