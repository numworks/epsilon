#include "logo_view.h"

#include <assert.h>

#include "logo_icon.h"

using namespace Escher;

namespace OnBoarding {

LogoView::LogoView() : View() { m_logoView.setImage(ImageStore::LogoIcon); }

void LogoView::drawRect(KDContext* ctx, KDRect rect) const {
  ctx->fillRect(bounds(), KDColorWhite);
}

int LogoView::numberOfSubviews() const { return 1; }

View* LogoView::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_logoView;
}

void LogoView::layoutSubviews(bool force) {
  m_logoView.setFrame(
      KDRect((Ion::Display::Width - ImageStore::LogoIcon->width()) / 2,
             (Ion::Display::Height - ImageStore::LogoIcon->height()) / 2,
             ImageStore::LogoIcon->width(), ImageStore::LogoIcon->height()),
      force);
}

}  // namespace OnBoarding
