#include "app_cell.h"
#include <assert.h>
#include <apps/i18n.h>
#include <escher/palette.h>

namespace Home {

AppCell::AppCell() :
  HighlightCell(),
  m_nameView(KDFont::SmallFont, (I18n::Message)0, 0.5f, 0.5f, Palette::HomeCellText, Palette::HomeCellBackground),
  m_visible(true), m_external_app(false)
{
}


void AppCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDSize nameSize = m_nameView.minimalSizeForOptimalDisplay();
  ctx->fillRect(KDRect(0,  bounds().height()-nameSize.height() - 2*k_nameHeightMargin, bounds().width(), nameSize.height()+2*k_nameHeightMargin), Palette::HomeBackground);
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

void AppCell::setExtAppDescriptor(const char* name, const Image* icon) {
  m_external_app = true;
  m_iconView.setImage(icon);
  m_nameView.setText(name);
  m_nameView.setTextColor(Palette::HomeCellTextExternal);
  m_nameView.setMessage(I18n::Message::Default);
  layoutSubviews();
}

void AppCell::setAppDescriptor(::App::Descriptor * descriptor) {
  m_external_app = false;
  m_iconView.setImage(descriptor->icon());
  m_nameView.setMessage(descriptor->name());
  m_nameView.setTextColor(Palette::HomeCellText);
  m_nameView.setText(nullptr);
  layoutSubviews();
}

void AppCell::setVisible(bool visible) {
  if (m_visible != visible) {
    m_visible = visible;
    markRectAsDirty(bounds());
  }
}

void AppCell::reloadCell() {
  m_nameView.setTextColor(isHighlighted() ? (m_external_app ? Palette::HomeCellTextExternalActive : Palette::HomeCellTextActive) : (m_external_app ? Palette::HomeCellTextExternal : Palette::HomeCellText));
  m_nameView.setBackgroundColor(isHighlighted() ? Palette::HomeCellBackgroundActive : Palette::HomeCellBackground);
}

}
