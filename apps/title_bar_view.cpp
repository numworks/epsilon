#include "title_bar_view.h"

#include <escher/palette.h>
#include <poincare/preferences.h>
#include <poincare/print.h>

#include "exam_icon.h"
#include "staar_icon.h"
extern "C" {
#include <assert.h>
}

using namespace Poincare;
using namespace Escher;

TitleBarView::TitleBarView()
    : View(),
      m_titleView(KDFont::Size::Small, I18n::Message::Default,
                  KDContext::k_alignCenter, KDContext::k_alignCenter,
                  KDColorWhite, Palette::YellowDark),
      m_preferenceView(KDFont::Size::Small, KDContext::k_alignRight,
                       KDContext::k_alignCenter, KDColorWhite,
                       Palette::YellowDark),
      m_examModeTextView(KDFont::Size::Small, I18n::Message::Default,
                         KDContext::k_alignCenter, KDContext::k_alignCenter,
                         KDColorWhite, Palette::YellowDark) {
  m_examModeIconView.setImage(ImageStore::ExamIcon);
}

void TitleBarView::drawRect(KDContext* ctx, KDRect rect) const {
  /* As we cheated to layout the title view, we have to fill a very thin
   * rectangle at the top with the background color. */
  ctx->fillRect(KDRect(0, 0, bounds().width(), 2), Palette::YellowDark);
}

void TitleBarView::setTitle(I18n::Message title) {
  m_titleView.setMessage(title);
}

bool TitleBarView::setChargeState(Ion::Battery::Charge chargeState) {
  return m_batteryView.setChargeState(chargeState);
}

bool TitleBarView::setIsCharging(bool isCharging) {
  return m_batteryView.setIsCharging(isCharging);
}

bool TitleBarView::setIsPlugged(bool isPlugged) {
  return m_batteryView.setIsPlugged(isPlugged);
}

bool TitleBarView::setShiftAlphaLockStatus(
    Ion::Events::ShiftAlphaStatus status) {
  return m_shiftAlphaLockView.setStatus(status);
}

void TitleBarView::updateBatteryAnimation() {
  m_batteryView.updateBatteryAnimation();
}

int TitleBarView::numberOfSubviews() const { return 6; }

View* TitleBarView::subviewAtIndex(int index) {
  View* views[] = {&m_titleView,          &m_preferenceView,
                   &m_examModeIconView,   &m_examModeTextView,
                   &m_shiftAlphaLockView, &m_batteryView};
  assert(0 <= index && index < sizeof(views) / sizeof(View*));
  return views[index];
}

void TitleBarView::layoutSubviews(bool force) {
  /* We here cheat to layout the main title. The application title is written
   * with upper cases. But, as upper letters are on the same baseline as lower
   * letters, they seem to be slightly above when they are perfectly centered
   * (because their glyph never cross the baseline). To avoid this effect, we
   * translate the frame of the title downwards.*/
  constexpr int k_verticalShift = 2;
  m_titleView.setFrame(KDRect(0, k_verticalShift, bounds().width(),
                              bounds().height() - k_verticalShift),
                       force);
  m_preferenceView.setFrame(
      KDRect(Metric::TitleBarExternHorizontalMargin, 0,
             m_preferenceView.minimalSizeForOptimalDisplay().width(),
             bounds().height()),
      force);
  KDSize batterySize = m_batteryView.minimalSizeForOptimalDisplay();
  m_batteryView.setFrame(
      KDRect(bounds().width() - batterySize.width() -
                 Metric::TitleBarExternHorizontalMargin,
             (bounds().height() - batterySize.height()) / 2, batterySize),
      force);
  if (Preferences::sharedPreferences->examMode().isActive()) {
    m_examModeIconView.setFrame(
        KDRect(k_examIconMargin, (bounds().height() - k_examIconHeight) / 2,
               k_examIconWidth, k_examIconHeight),
        force);
    m_examModeTextView.setFrame(
        KDRect(k_examIconMargin - k_examTextWidth, k_verticalShift,
               k_examTextWidth, bounds().height() - k_verticalShift),
        force);
    I18n::Message examModeMessage;

    ExamMode::Ruleset ruleset =
        Preferences::sharedPreferences->examMode().ruleset();
    if (ruleset == ExamMode::Ruleset::STAAR) {
      m_examModeIconView.setImage(ImageStore::StaarIcon);
    } else {
      m_examModeIconView.setImage(ImageStore::ExamIcon);
    }

    switch (ruleset) {
      case ExamMode::Ruleset::English:
        examModeMessage = I18n::Message::ExamModeTitleBarUK;
        break;
      case ExamMode::Ruleset::Dutch:
        examModeMessage = I18n::Message::ExamModeTitleBarNL;
        break;
      case ExamMode::Ruleset::Portuguese:
        examModeMessage = I18n::Message::ExamModeTitleBarPT;
        break;
      case ExamMode::Ruleset::IBTest:
        examModeMessage = I18n::Message::ExamModeTitleBarIB;
        break;
      case ExamMode::Ruleset::STAAR:
        examModeMessage = I18n::Message::ExamModeTitleBarSTAAR;
        break;
      case ExamMode::Ruleset::Keystone:
        examModeMessage = I18n::Message::ExamModeTitleBarKeystone;
        break;
      default:
        examModeMessage = I18n::Message::Default;
    }
    m_examModeTextView.setMessage(examModeMessage);
  } else {
    m_examModeIconView.setFrame(KDRectZero, force);
    m_examModeTextView.setMessage(I18n::Message::Default);
  }
  KDSize shiftAlphaLockSize =
      m_shiftAlphaLockView.minimalSizeForOptimalDisplay();
  m_shiftAlphaLockView.setFrame(
      KDRect(bounds().width() - batterySize.width() -
                 Metric::TitleBarExternHorizontalMargin - k_alphaRightMargin -
                 shiftAlphaLockSize.width(),
             (bounds().height() - shiftAlphaLockSize.height()) / 2,
             shiftAlphaLockSize),
      force);
}

void TitleBarView::refreshPreferences() {
  constexpr size_t bufferSize = 13;
  char buffer[bufferSize];
  Preferences* preferences = Preferences::sharedPreferences;
  // Display Sci/ or Eng/ if the print float mode is not decimal
  const Preferences::PrintFloatMode printFloatMode = preferences->displayMode();
  I18n::Message floatModeMessage =
      printFloatMode == Preferences::PrintFloatMode::Decimal
          ? I18n::Message::Default
          : (printFloatMode == Preferences::PrintFloatMode::Scientific
                 ? I18n::Message::Sci
                 : I18n::Message::Eng);
  // Display the angle unit
  const Preferences::AngleUnit angleUnit = preferences->angleUnit();
  I18n::Message angleMessage =
      angleUnit == Preferences::AngleUnit::Degree
          ? I18n::Message::Deg
          : (angleUnit == Preferences::AngleUnit::Radian ? I18n::Message::Rad
                                                         : I18n::Message::Gon);
  Poincare::Print::CustomPrintf(buffer, bufferSize, "%s%s",
                                I18n::translate(floatModeMessage),
                                I18n::translate(angleMessage));
  m_preferenceView.setText(buffer);
  // Layout the exam mode icon if needed
  layoutSubviews();
}

void TitleBarView::reload() {
  refreshPreferences();
  markRectAsDirty(bounds());
}
