#include "app_cell.h"
#include <assert.h>

namespace Home {

AppCell::AppCell() :
  HighlightCell(),
  m_nameView(MessageTextView(KDText::FontSize::Small, (I18n::Message)0, 0.5f, 0.5f, KDColorBlack, KDColorWhite)),
  m_visible(true)
{
}

int AppCell::numberOfSubviews() const {
  return m_visible ? 2 : 0;
}

View * AppCell::subviewAtIndex(int index) {
  View * views[] = {&m_iconView, &m_nameView};
  return views[index];
}

void AppCell::layoutSubviews() {
  m_iconView.setFrame(KDRect((bounds().width()-k_iconWidth)/2, k_iconMargin, k_iconWidth,k_iconHeight));
  KDSize nameSize = m_nameView.minimalSizeForOptimalDisplay();
  m_nameView.setFrame(KDRect((bounds().width()-nameSize.width())/2-k_nameWidthMargin,  bounds().height()-nameSize.height() - 2*k_nameHeightMargin, nameSize.width()+2*k_nameWidthMargin, nameSize.height()+2*k_nameHeightMargin));
}

void AppCell::setApp(::App * app) {
  m_iconView.setImage(app->icon());
  m_nameView.setMessage(app->name());
  layoutSubviews();
}

void AppCell::setVisible(bool visible) {
  if (m_visible != visible) {
    m_visible = visible;
    markRectAsDirty(bounds());
  }
}

void AppCell::reloadCell() {
  HighlightCell::reloadCell();
  m_nameView.setBackgroundColor(isHighlighted() ? Palette::YellowDark : KDColorWhite);
}

}
