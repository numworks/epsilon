#include <escher/switch_view.h>
#include <escher/palette.h>

SwitchView::SwitchView() :
View(),
m_state(true)
{
}

bool SwitchView::state() {
  return m_state;
}

void SwitchView::setState(bool state) {
  m_state = state;
  markRectAsDirty(bounds());
}

void SwitchView::drawRect(KDContext * ctx, KDRect rect) const {
  /* Draw the switch aligned on the right of the view and vertically centered.
   * The heightCenter is the coordinate of the vertical middle of the view. That
   * way, (heightCenter-switchHalfHeight) indicates the top the switch. */
  KDCoordinate width = bounds().width();
  KDCoordinate heightCenter =  bounds().height()/2;
  KDCoordinate switchHalfHeight = k_switchHeight/2;
  KDCoordinate switchHalfWidth = k_switchWidth/2;

  // These 4 lines draw the outline of the switch.
  ctx->fillRect(KDRect(width - 2*k_separatorThickness - k_switchMargin - k_switchWidth, heightCenter-switchHalfHeight - k_separatorThickness, k_switchWidth + 2*k_separatorThickness, k_separatorThickness), Palette::LineColor);
  ctx->fillRect(KDRect(width - 2*k_separatorThickness - k_switchMargin - k_switchWidth, heightCenter-switchHalfHeight, k_separatorThickness, k_switchHeight+k_separatorThickness), Palette::LineColor);
  ctx->fillRect(KDRect(width - k_separatorThickness - k_switchMargin - k_switchWidth, heightCenter+switchHalfHeight, k_switchWidth+k_separatorThickness, k_separatorThickness), Palette::LineColor);
  ctx->fillRect(KDRect(width - k_separatorThickness - k_switchMargin, heightCenter-switchHalfHeight, k_separatorThickness, k_switchHeight), Palette::LineColor);
  // These next lines fill the switch with black and green/red.
  if (m_state) {
    ctx->fillRect(KDRect(width - k_switchMargin - k_switchWidth - k_separatorThickness, heightCenter-switchHalfHeight, switchHalfWidth, k_switchHeight), KDColorBlack);
    ctx->fillRect(KDRect(width - k_switchMargin - switchHalfWidth - k_separatorThickness, heightCenter-switchHalfHeight, switchHalfWidth, k_switchHeight), KDColorGreen);
  } else {
    ctx->fillRect(KDRect(width - k_switchMargin - k_switchWidth - k_separatorThickness, heightCenter-switchHalfHeight, switchHalfWidth, k_switchHeight), KDColorRed);
    ctx->fillRect(KDRect(width - k_switchMargin - switchHalfWidth - k_separatorThickness, heightCenter-switchHalfHeight, switchHalfWidth, k_switchHeight), KDColorBlack);
  }
}
