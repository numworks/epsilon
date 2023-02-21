#ifndef ESCHER_BUTTON_STATE_H
#define ESCHER_BUTTON_STATE_H

#include <escher/abstract_button_cell.h>
#include <escher/toggleable_view.h>

namespace Escher {

class ButtonState : public AbstractButtonCell {
 public:
  ButtonState(Responder* parentResponder, I18n::Message textBody,
              Invocation invocation, ToggleableView* stateView,
              KDFont::Size font = KDFont::Size::Small,
              KDColor textColor = KDColorBlack);
  bool state() const { return m_stateView->state(); }
  void setState(bool state);
  KDSize minimalSizeForOptimalDisplay() const override;
  void drawRect(KDContext* ctx, KDRect rect) const override;

 private:
  // Dot right margin.
  constexpr static KDCoordinate k_stateMargin = 9;
  int numberOfSubviews() const override { return 2; }
  View* subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  ToggleableView* m_stateView;
};

}  // namespace Escher

#endif
