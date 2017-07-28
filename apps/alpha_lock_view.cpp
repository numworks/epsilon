#include "alpha_lock_view.h"

AlphaLockView::AlphaLockView() :
  View(),
  m_alphaView(KDText::FontSize::Small, I18n::Message::Default, 1.0f, 0.5f, KDColorWhite, Palette::YellowDark),
  m_status(Ion::Events::ShiftAlphaStatus::Default)
{
}

void AlphaLockView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), Palette::YellowDark);
}

bool AlphaLockView::setStatus(Ion::Events::ShiftAlphaStatus status) {
  if (status != m_status) {
    m_status = status;
    switch (status) {
      case Ion::Events::ShiftAlphaStatus::Alpha:
      case Ion::Events::ShiftAlphaStatus::AlphaLock:
      case Ion::Events::ShiftAlphaStatus::AlphaLockShift:
        m_alphaView.setMessage(I18n::Message::Alpha);
        break;
      case Ion::Events::ShiftAlphaStatus::ShiftAlpha:
      case Ion::Events::ShiftAlphaStatus::ShiftAlphaLock:
        m_alphaView.setMessage(I18n::Message::CapitalAlpha);
        break;
      default:
        m_alphaView.setMessage(I18n::Message::Default);
        break;
    }
    markRectAsDirty(bounds());
    return true;
  }
  return false;
}

KDSize AlphaLockView::minimalSizeForOptimalDisplay() const {
  KDSize alphaSize = KDText::stringSize(I18n::translate(I18n::Message::Alpha));
  KDSize lockSize = m_lockView.minimalSizeForOptimalDisplay();
  KDCoordinate height = lockSize.height() > alphaSize.height() ? lockSize.height() : alphaSize.height();
  return KDSize(alphaSize.width() + lockSize.width() + k_lockRightMargin, height);
}

int AlphaLockView::numberOfSubviews() const {
  switch (m_status) {
    case Ion::Events::ShiftAlphaStatus::Alpha:
    case Ion::Events::ShiftAlphaStatus::ShiftAlpha:
      return 1;
    case Ion::Events::ShiftAlphaStatus::AlphaLock:
    case Ion::Events::ShiftAlphaStatus::AlphaLockShift:
    case Ion::Events::ShiftAlphaStatus::ShiftAlphaLock:
      return 2;
    default:
      return 0;
  }
}

View * AlphaLockView::subviewAtIndex(int index) {
  if (index == 0) {
    return &m_alphaView;
  }
  return &m_lockView;
}

void AlphaLockView::layoutSubviews() {
  KDSize alphaSize = KDText::stringSize(I18n::translate(I18n::Message::Alpha), KDText::FontSize::Small);
  m_alphaView.setFrame(KDRect(bounds().width() - alphaSize.width(), (bounds().height()- alphaSize.height())/2, alphaSize));

  KDSize lockSize = m_lockView.minimalSizeForOptimalDisplay();
  m_lockView.setFrame(KDRect(bounds().width() - alphaSize.width() - lockSize.width() - k_lockRightMargin, (bounds().height()- lockSize.height())/2, lockSize));
}
