#include "shift_alpha_lock_view.h"

#include <escher/palette.h>

#include "title_bar_view.h"

using namespace Escher;

ShiftAlphaLockView::ShiftAlphaLockView()
    : View(),
      m_shiftAlphaView(
          I18n::Message::Default,
          {.style = {.glyphColor = KDColorWhite,
                     .backgroundColor = TitleBarView::k_backgroundColor,
                     .font = KDFont::Size::Small},
           .horizontalAlignment = KDGlyph::k_alignRight}),
      m_status(Ion::Events::ShiftAlphaStatus()) {}

void ShiftAlphaLockView::drawRect(KDContext* ctx, KDRect rect) const {
  ctx->fillRect(bounds(), TitleBarView::k_backgroundColor);
}

bool ShiftAlphaLockView::setStatus(Ion::Events::ShiftAlphaStatus status) {
  if (status != m_status) {
    m_status = status;

    constexpr I18n::Message k_messages[2][2] = {
        {I18n::Message::Default, I18n::Message::Shift},
        {I18n::Message::Alpha, I18n::Message::CapitalAlpha}};
    m_shiftAlphaView.setMessage(
        k_messages[m_status.alphaIsActive()][m_status.shiftIsActive()]);

    layoutSubviews();
    markWholeFrameAsDirty();
    return true;
  }
  return false;
}

KDSize ShiftAlphaLockView::minimalSizeForOptimalDisplay() const {
  KDSize modifierSize = KDFont::Font(KDFont::Size::Small)
                            ->stringSize(I18n::translate(I18n::Message::Alpha));
  KDSize lockSize = m_lockView.minimalSizeForOptimalDisplay();
  KDCoordinate height = lockSize.height() > modifierSize.height()
                            ? lockSize.height()
                            : modifierSize.height();
  return KDSize(modifierSize.width() + lockSize.width() + k_lockRightMargin,
                height);
}

int ShiftAlphaLockView::numberOfSubviews() const {
  if (m_status.alphaIsLocked()) {
    return 2;
  }
  return (m_status.alphaIsActive() || m_status.shiftIsActive()) ? 1 : 0;
}

View* ShiftAlphaLockView::subviewAtIndex(int index) {
  if (index == 0) {
    return &m_shiftAlphaView;
  }
  return &m_lockView;
}

void ShiftAlphaLockView::layoutSubviews(bool force) {
  KDSize modifierSize = KDFont::Font(KDFont::Size::Small)
                            ->stringSize(I18n::translate(I18n::Message::Alpha));
  setChildFrame(
      &m_shiftAlphaView,
      KDRect(bounds().width() - modifierSize.width(),
             (bounds().height() - modifierSize.height()) / 2, modifierSize),
      force);

  KDSize lockSize = m_lockView.minimalSizeForOptimalDisplay();
  setChildFrame(&m_lockView,
                KDRect(bounds().width() - modifierSize.width() -
                           lockSize.width() - k_lockRightMargin,
                       (bounds().height() - lockSize.height()) / 2, lockSize),
                force);
}
