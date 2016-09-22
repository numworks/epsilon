#include <escher/switch_view.h>
#include <escher/palette.h>

/* k_switchHeight and k_switchWidth are the dimensions of the switch (including
* the outline of the switch). The outline thickness is k_separatorThickness. */
constexpr KDCoordinate SwitchView::k_switchHeight;
constexpr KDCoordinate SwitchView::k_switchWidth;
constexpr KDCoordinate SwitchView::k_separatorThickness;


SwitchView::SwitchView() :
ChildlessView(),
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
  /* Draw the switch in the center of the view. The widthCenter and heightCenter
  * are the coordinates of the middle of the view. That way, for instance,
  * (widthCenter - switchHalfWidth) and (heightCenter-switchHalfHeight) indicate
  * the top left corner of the view. */
  KDCoordinate widthCenter = bounds().width()/2;
  KDCoordinate heightCenter =  bounds().height()/2;
  KDCoordinate switchHalfWidth = k_switchWidth/2;
  KDCoordinate switchHalfHeight = k_switchHeight/2;

  // These 4 lines draw the outline of the switch.
  ctx->fillRect(KDRect(widthCenter - switchHalfWidth, heightCenter-switchHalfHeight, k_switchWidth, k_separatorThickness), Palette::LineColor);
  ctx->fillRect(KDRect(widthCenter - switchHalfWidth, heightCenter-switchHalfHeight+k_separatorThickness, k_separatorThickness, k_switchHeight-k_separatorThickness), Palette::LineColor);
  ctx->fillRect(KDRect(widthCenter - switchHalfWidth+k_separatorThickness, heightCenter+switchHalfHeight-k_separatorThickness, k_switchWidth-k_separatorThickness, k_separatorThickness), Palette::LineColor);
  ctx->fillRect(KDRect(widthCenter + switchHalfWidth, heightCenter-switchHalfHeight, k_separatorThickness, k_switchHeight), Palette::LineColor);
  // These next lines fill the switch with black and green/red.
  if (m_state) {
    ctx->fillRect(KDRect(widthCenter - switchHalfWidth+k_separatorThickness, heightCenter-switchHalfHeight+k_separatorThickness, switchHalfWidth, k_switchHeight-2*k_separatorThickness), KDColorBlack);
    ctx->fillRect(KDRect(widthCenter+k_separatorThickness, heightCenter-switchHalfHeight+k_separatorThickness, switchHalfWidth-k_separatorThickness, k_switchHeight-2*k_separatorThickness), KDColorGreen);
  } else {
    ctx->fillRect(KDRect(widthCenter - switchHalfWidth+k_separatorThickness, heightCenter-switchHalfHeight+k_separatorThickness, switchHalfWidth, k_switchHeight-2*k_separatorThickness), KDColorRed);
  ctx->fillRect(KDRect(widthCenter+k_separatorThickness, heightCenter-switchHalfHeight+k_separatorThickness, switchHalfWidth-k_separatorThickness, k_switchHeight-2*k_separatorThickness), KDColorBlack);
  }
}
