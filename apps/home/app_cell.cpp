#include "app_cell.h"
#include <assert.h>

namespace Home {

AppCell::AppCell() :
  TableViewCell(),
  m_nameView(PointerTextView(KDText::FontSize::Small)),
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
  m_iconView.setFrame(KDRect(0,0,k_iconWidth,k_iconHeight));
  if (bounds().height() > k_iconHeight) {
    m_nameView.setFrame(KDRect(0, k_iconHeight, bounds().width(), bounds().height()-k_iconHeight));
  }
}

void AppCell::setApp(::App * app) {
  m_iconView.setImage(app->icon());
  m_nameView.setText(app->name());
}

void AppCell::setVisible(bool visible) {
  if (m_visible != visible) {
    m_visible = visible;
    markRectAsDirty(bounds());
  }
}

void AppCell::reloadCell() {
  TableViewCell::reloadCell();
  m_nameView.setBackgroundColor(isHighlighted() ? KDColorRed : KDColorWhite);
}

}
