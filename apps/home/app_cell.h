#ifndef HOME_APP_CELL_H
#define HOME_APP_CELL_H

#include <escher.h>

namespace Home {

class AppCell final : public HighlightCell {
public:
  AppCell():
    HighlightCell(),
    m_nameView(KDText::FontSize::Small, (I18n::Message)0, 0.5f, 0.5f, KDColorBlack, KDColorWhite),
    m_visible(true) {}
  void drawRect(KDContext * ctx, KDRect rect) const override;

  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;

  void setVisible(bool visible);
  void reloadCell() override;
  void setAppDescriptor(::App::Descriptor * appDescriptor);
private:
  static constexpr KDCoordinate k_iconMargin = 18;
  static constexpr KDCoordinate k_iconWidth = 55;
  static constexpr KDCoordinate k_iconHeight = 56;
  static constexpr KDCoordinate k_nameWidthMargin = 4;
  static constexpr KDCoordinate k_nameHeightMargin = 2;
  ImageView m_iconView;
  MessageTextView m_nameView;
  bool m_visible;
};

}

#endif
