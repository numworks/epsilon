#pragma once

#include <apps/i18n.h>
#include <escher/buffer_text_view.h>
#include <escher/image_view.h>
#include <escher/solid_color_view.h>
#include <escher/text_format.h>
#include <escher/themed_image_view.h>

#include "battery_view.h"
#include "shift_alpha_lock_view.h"

class TitleBarView : public Escher::View {
 public:
  TitleBarView();

  void drawRect(KDContext* ctx, KDRect rect) const override;

  void setTitle(I18n::Message title);
  bool setChargeState(Ion::Battery::Charge chargeState);
  void updateBatteryAnimation();
  bool setIsCharging(bool isCharging);
  bool setIsPlugged(bool isPlugged);
  bool setShiftAlphaLockStatus(Ion::Events::ShiftAlphaStatus status);
  void refreshPreferences();
  void reload();

  constexpr static Escher::ThemeComponent k_backgroundColor =
      Escher::ThemeComponent::BannerYellow;

 private:
  constexpr static size_t k_preferenceTextSize = sizeof("tech/rad");
  constexpr static KDCoordinate k_alphaRightMargin = 5;
  constexpr static KDCoordinate k_examIconWidth = 18;
  constexpr static KDCoordinate k_examIconHeight = 9;
  constexpr static KDCoordinate k_examIconMargin = 93;
  constexpr static KDCoordinate k_examTextWidth =
      KDFont::GlyphWidth(KDFont::Size::Large) * 3;
  constexpr static Escher::TextFormat k_glyphsFormat = {
      .style = {.glyphColor = Escher::ThemeComponent::TextWhite,
                .backgroundColor = k_backgroundColor,
                .font = KDFont::Size::Small},
      .horizontalAlignment = KDGlyph::k_alignCenter,
      .verticalAlignment = KDGlyph::k_alignCenter};
  /* We cheat to layout the main title. The application title is written
   * with upper cases. But, as upper letters are on the same baseline as lower
   * letters, they seem to be slightly above when they are perfectly centered
   * (because their glyph never cross the baseline). To avoid this effect, we
   * translate the frame of the title downwards.*/
  constexpr static int k_verticalShift = 2;

  int numberOfSubviews() const override;
  void layoutSubviews(bool force = false) override;
  Escher::View* subviewAtIndex(int index) override;

  Escher::MessageTextView m_titleView;
  BatteryView m_batteryView;
  ShiftAlphaLockView m_shiftAlphaLockView;
  Escher::BufferTextView<k_preferenceTextSize> m_preferenceView;
  Escher::ThemedImageView m_examModeIconView;
  Escher::MessageTextView m_examModeTextView;
  Escher::SolidColorView m_separatorView{
      Escher::ThemeComponent::SeparatorTitleBar};
};
