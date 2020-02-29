#include "omega_view.h"
#include "apps/i18n.h"

namespace Omega {

OmegaView::OmegaView() :
  View(),
  m_omegaTextView(KDFont::LargeFont, I18n::Message::OmegaApp, 0.5, 0.1, Palette::Toolbar, Palette::BackgroundApps),
  m_urlTextView(KDFont::SmallFont, I18n::Message::OmegaURL, 0.5, 0.2, Palette::SecondaryText, Palette::BackgroundApps)
{
}

void OmegaView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(KDRect(0, 0, bounds().width(), bounds().height()), Palette::BackgroundApps);
}

void OmegaView::reload() {
  markRectAsDirty(bounds());
}

int OmegaView::numberOfSubviews() const {
  return 3;
}

View * OmegaView::subviewAtIndex(int index) {
  assert(index >= 0 && index <= 2);
  switch (index)
  {
    case 1:
      return &m_urlTextView;
    default:
      return &m_omegaTextView;
  }
}

void OmegaView::layoutSubviews(bool force) {
  KDCoordinate textHeight = KDFont::SmallFont->glyphSize().height();
  m_omegaTextView.setFrame(KDRect(0, 30, bounds().width(), textHeight + 12), force);
  m_urlTextView.setFrame(KDRect(0, 55, bounds().width(), textHeight), force);
}

}
