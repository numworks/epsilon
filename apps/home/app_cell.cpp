#include "app_cell.h"

#include <assert.h>

using namespace Escher;

namespace Home {

AppCell::AppCell()
    : HighlightCell(),
      m_messageNameView(KDFont::Size::Small, (I18n::Message)0,
                        KDContext::k_alignCenter, KDContext::k_alignCenter,
                        KDColorBlack, KDColorWhite),
      m_image(0, 0, nullptr, 0),
      m_pointerNameView(KDFont::Size::Small, nullptr, KDContext::k_alignCenter,
                        KDContext::k_alignCenter, KDColorBlack, KDColorWhite) {}

void AppCell::drawRect(KDContext *ctx, KDRect rect) const {
  KDSize nameSize = textView()->minimalSizeForOptimalDisplay();
  ctx->fillRect(
      KDRect(0, bounds().height() - nameSize.height() - 2 * k_nameHeightMargin,
             bounds().width(), nameSize.height() + 2 * k_nameHeightMargin),
      KDColorWhite);
}

int AppCell::numberOfSubviews() const { return isVisible() ? 2 : 0; }

View *AppCell::subviewAtIndex(int index) {
  View *views[] = {&m_iconView, const_cast<TextView *>(textView())};
  return views[index];
}

void AppCell::layoutSubviews(bool force) {
  m_iconView.setFrame(KDRect((bounds().width() - k_iconWidth) / 2, k_iconMargin,
                             k_iconWidth, k_iconHeight),
                      force);
  KDSize nameSize = textView()->minimalSizeForOptimalDisplay();
  const_cast<TextView *>(textView())
      ->setFrame(
          KDRect((bounds().width() - nameSize.width()) / 2 - k_nameWidthMargin,
                 bounds().height() - nameSize.height() - 2 * k_nameHeightMargin,
                 nameSize.width() + 2 * k_nameWidthMargin,
                 nameSize.height() + 2 * k_nameHeightMargin),
          force);
}

void AppCell::setBuiltinAppDescriptor(const ::App::Descriptor *descriptor) {
  m_iconView.setImage(descriptor->icon());
  m_messageNameView.setMessage(descriptor->name());
  m_pointerNameView.setText(nullptr);
  layoutSubviews();
}

void AppCell::setExternalApp(Ion::ExternalApps::App app) {
  m_pointerNameView.setText(app.name());
  m_messageNameView.setMessage((I18n::Message)0);
  m_image = Image(k_iconWidth, k_iconHeight, app.iconData(), app.iconSize());
  m_iconView.setImage(&m_image);
  layoutSubviews();
}

void AppCell::setVisible(bool visible) {
  if (isVisible() != visible) {
    Escher::HighlightCell::setVisible(visible);
    markRectAsDirty(bounds());
  }
}

void AppCell::reloadCell() {
  TextView *t = const_cast<TextView *>(textView());
  t->setTextColor(isHighlighted() ? KDColorWhite : KDColorBlack);
  t->setBackgroundColor(isHighlighted() ? Palette::YellowDark : KDColorWhite);
}

const Escher::TextView *AppCell::textView() const {
  if (m_pointerNameView.text()) {
    return &m_pointerNameView;
  } else {
    return &m_messageNameView;
  }
}

}  // namespace Home
