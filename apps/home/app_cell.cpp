#include "app_cell.h"
#include <assert.h>

namespace Home {

AppCell::AppCell() :
  HighlightCell(),
  m_nameView(KDFont::SmallFont, (I18n::Message)0, 0.5f, 0.5f, KDColorBlack, KDColorWhite),
  m_visible(true)
{
}


void AppCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDSize nameSize = m_nameView.minimalSizeForOptimalDisplay();
  ctx->fillRect(KDRect(0,  bounds().height()-nameSize.height() - 2*k_nameHeightMargin, bounds().width(), nameSize.height()+2*k_nameHeightMargin), KDColorWhite);
}

int AppCell::numberOfSubviews() const {
  return m_visible ? 2 : 0;
}

View * AppCell::subviewAtIndex(int index) {
  View * views[] = {&m_iconView, &m_nameView};
  return views[index];
}

void AppCell::layoutSubviews(bool force) {
  m_iconView.setFrame(KDRect((bounds().width()-k_iconWidth)/2, k_iconMargin, k_iconWidth,k_iconHeight), force);
  KDSize nameSize = m_nameView.minimalSizeForOptimalDisplay();
  m_nameView.setFrame(KDRect((bounds().width()-nameSize.width())/2-k_nameWidthMargin, bounds().height()-nameSize.height() - 2*k_nameHeightMargin, nameSize.width()+2*k_nameWidthMargin, nameSize.height()+2*k_nameHeightMargin), force);
}

void AppCell::setAppDescriptor(::App::Descriptor * descriptor) {
  m_iconView.setImage(descriptor->icon());
  m_nameView.setMessage(descriptor->name());
  layoutSubviews();
}

void AppCell::setVisible(bool visible) {
  if (m_visible != visible) {
    m_visible = visible;
    markRectAsDirty(bounds());
  }
}

void AppCell::reloadCell() {
  m_nameView.setTextColor(isHighlighted() ? KDColorWhite : KDColorBlack);
  m_nameView.setBackgroundColor(isHighlighted() ? Palette::YellowDark : KDColorWhite);
}

}
