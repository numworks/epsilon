#include "title_bar_view.h"
#include "exam_icon.h"
#include <escher/palette.h>
#include <poincare/print.h>
#include <poincare/preferences.h>
extern "C" {
#include <assert.h>
}

using namespace Poincare;
using namespace Escher;

TitleBarView::TitleBarView() :
  View(),
  m_titleView(KDFont::Size::Small, I18n::Message::Default, KDContext::k_alignCenter,
              KDContext::k_alignCenter, KDColorWhite, Palette::YellowDark),
  m_preferenceView(KDFont::Size::Small, KDContext::k_alignRight, KDContext::k_alignCenter, KDColorWhite, Palette::YellowDark)
{
  m_examModeIconView.setImage(ImageStore::ExamIcon);
}

void TitleBarView::drawRect(KDContext * ctx, KDRect rect) const {
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

bool TitleBarView::setShiftAlphaLockStatus(Ion::Events::ShiftAlphaStatus status) {
  return m_shiftAlphaLockView.setStatus(status);
}

int TitleBarView::numberOfSubviews() const {
  return 5;
}

View * TitleBarView::subviewAtIndex(int index) {
  if (index == 0) {
    return &m_titleView;
  }
  if (index == 1) {
    return &m_preferenceView;
  }
  if (index == 2) {
    return &m_examModeIconView;
  }
  if (index == 3) {
    return &m_shiftAlphaLockView;
  }
  return &m_batteryView;
}

void TitleBarView::layoutSubviews(bool force) {
  /* We here cheat to layout the main title. The application title is written
   * with upper cases. But, as upper letters are on the same baseline as lower
   * letters, they seem to be slightly above when they are perferctly centered
   * (because their glyph never cross the baseline). To avoid this effect, we
   * translate the frame of the title downwards.*/
  m_titleView.setFrame(KDRect(0, 2, bounds().width(), bounds().height()-2), force);
  m_preferenceView.setFrame(KDRect(Metric::TitleBarExternHorizontalMargin, 0, m_preferenceView.minimalSizeForOptimalDisplay().width(), bounds().height()), force);
  KDSize batterySize = m_batteryView.minimalSizeForOptimalDisplay();
  m_batteryView.setFrame(KDRect(bounds().width() - batterySize.width() - Metric::TitleBarExternHorizontalMargin, (bounds().height()- batterySize.height())/2, batterySize), force);
  if (Preferences::sharedPreferences()->isInExamMode()) {
    m_examModeIconView.setFrame(KDRect(k_examIconMargin, (bounds().height() - k_examIconHeight)/2, k_examIconWidth, k_examIconHeight), force);
  } else {
    m_examModeIconView.setFrame(KDRectZero, force);
  }
  KDSize shiftAlphaLockSize = m_shiftAlphaLockView.minimalSizeForOptimalDisplay();
  m_shiftAlphaLockView.setFrame(KDRect(bounds().width()-batterySize.width()-Metric::TitleBarExternHorizontalMargin-k_alphaRightMargin-shiftAlphaLockSize.width(), (bounds().height()- shiftAlphaLockSize.height())/2, shiftAlphaLockSize), force);
}

void TitleBarView::refreshPreferences() {
  constexpr size_t bufferSize = 13;
  char buffer[bufferSize];
  Preferences * preferences = Preferences::sharedPreferences();
  // Display Sci/ or Eng/ if the print float mode is not decimal
  const Preferences::PrintFloatMode printFloatMode = preferences->displayMode();
  I18n::Message floatModeMessage = printFloatMode == Preferences::PrintFloatMode::Decimal ? I18n::Message::Default : (printFloatMode == Preferences::PrintFloatMode::Scientific ? I18n::Message::Sci : I18n::Message::Eng);
  // Display the angle unit
  const Preferences::AngleUnit angleUnit = preferences->angleUnit();
  I18n::Message angleMessage = angleUnit == Preferences::AngleUnit::Degree ?
     I18n::Message::Deg :
     (angleUnit == Preferences::AngleUnit::Radian ? I18n::Message::Rad : I18n::Message::Gon);
  Poincare::Print::CustomPrintf(buffer, bufferSize, "%s%s", I18n::translate(floatModeMessage), I18n::translate(angleMessage));
  m_preferenceView.setText(buffer);
  // Layout the exam mode icon if needed
  layoutSubviews();
}

void TitleBarView::reload() {
  refreshPreferences();
  markRectAsDirty(bounds());
}
