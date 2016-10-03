#include "app_cell.h"
#include <assert.h>

namespace Home {

AppCell::AppCell() :
  View(),
  m_visible(true),
  m_active(false)
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
  m_iconView.setFrame(KDRect(0,0,k_iconSize,k_iconSize));
  if (bounds().height() > k_iconSize) {
    m_nameView.setFrame(KDRect(0, k_iconSize, bounds().width(), bounds().height()-k_iconSize));
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

void AppCell::setActive(bool active) {
  if (m_active != active) {
    m_active = active;
    m_nameView.setBackgroundColor(m_active ? KDColorRed : KDColorWhite);

    markRectAsDirty(bounds());
  }
}

}
