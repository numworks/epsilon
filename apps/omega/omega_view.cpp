#include "omega_view.h"
#include "apps/i18n.h"

namespace Omega {

OmegaView::OmegaView() :
  View(),
  m_bufferTextView(KDFont::LargeFont, 0.5, 0.5, Palette::PrimaryText)
{
  m_bufferTextView.setText(I18n::translate(I18n::Message::OmegaApp));
}

void OmegaView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(KDRect(0, 0, bounds().width(), bounds().height()), Palette::BackgroundApps);
}

void OmegaView::reload() {
  markRectAsDirty(bounds());
}

int OmegaView::numberOfSubviews() const {
  return 1;
}

View * OmegaView::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_bufferTextView;
}

void OmegaView::layoutSubviews(bool force) {
  m_bufferTextView.setFrame(KDRect(0, 0, bounds().width(), bounds().height()), force);
}

}
