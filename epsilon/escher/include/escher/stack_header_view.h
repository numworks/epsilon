#pragma once

#include <escher/bordered.h>
#include <escher/theme/components.h>
#include <escher/view.h>

namespace Escher {

class ViewController;

class StackHeaderView : public View, public Bordered {
 public:
  StackHeaderView(
      const ViewController* controller = nullptr,
      ThemeComponent textColor = ThemeComponent::TextDefault,
      ThemeComponent backgroundColor = ThemeComponent::BackgroundPopup,
      ThemeComponent separatorColor = ThemeComponent::TokenBlack,
      ThemeComponent borderColor = ThemeComponent::TokenBlack);
  void drawRect(KDContext* ctx, KDRect rect) const override;

 protected:
#if ESCHER_VIEW_LOGGING
  const char* className() const override;
  void logAttributes(std::ostream& os) const override;
#endif
 private:
  constexpr static KDFont::Size k_font = KDFont::Size::Small;

  ThemeComponent m_textColor;
  ThemeComponent m_backgroundColor;
  // The separators are above and below the cell
  ThemeComponent m_separatorColor;
  // The borders are on the left and right sides of the cell
  ThemeComponent m_borderColor;
  const ViewController* m_controller;
};

}  // namespace Escher
