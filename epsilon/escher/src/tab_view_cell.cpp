#include <escher/tab_view_cell.h>
#include <escher/tab_view_controller.h>
#include <escher/theme/color_picker.h>
#include <escher/theme/highlight.h>
extern "C" {
#include <assert.h>
}

namespace Escher {

TabViewCell::TabViewCell()
    : View(), m_active(false), m_selected(false), m_controller(nullptr) {}

void TabViewCell::setTabController(TabViewController* controller,
                                   uint8_t tabNumber) {
  m_tabNumber = tabNumber;
  m_controller = controller;
  markWholeFrameAsDirty();
}

void TabViewCell::setActive(bool active) {
  m_active = active;
  markWholeFrameAsDirty();
}

void TabViewCell::setSelected(bool selected) {
  m_selected = selected;
  markWholeFrameAsDirty();
}

KDSize TabViewCell::minimalSizeForOptimalDisplay() const {
  return KDFont::Font(KDFont::Size::Small)
      ->stringSize(m_controller->tabName(m_tabNumber));
}

void TabViewCell::drawRect(KDContext* ctx, KDRect rect) const {
  KDCoordinate height = bounds().height();
  KDCoordinate width = bounds().width();
  // choose the background color
  ThemeComponent text =
      m_active ? ThemeComponent::TextPurple : ThemeComponent::TextWhite;
  ThemeComponent inactiveBackground = m_controller->tabBackgroundColor();
  ThemeComponent background =
      m_active ? ThemeComponent::BackgroundWhite : inactiveBackground;
  ThemeComponent selection = m_active ? ThemeComponent::ActiveTabSelected
                                      : ThemeComponent::InactiveTabSelected;
  background = m_selected ? selection : background;
  text = Highlight(text, m_selected);
  // Color the background according to the state of the tab cell
  if (m_active || m_selected) {
    ctx->fillRect(KDRect(0, 0, width, 1),
                  ColorPicker::Resolve(inactiveBackground));
    ctx->fillRect(KDRect(0, 1, width, height - 1),
                  ColorPicker::Resolve(background));
  } else {
    ctx->fillRect(KDRect(0, 0, width, height),
                  ColorPicker::Resolve(background));
  }
  // Write title
  ctx->alignAndDrawString(
      m_controller->tabName(m_tabNumber), KDPointZero, bounds().size(),
      {.style = {.glyphColor = ColorPicker::Resolve(text),
                 .backgroundColor = ColorPicker::Resolve(background),
                 .font = KDFont::Size::Small},
       .horizontalAlignment = KDGlyph::k_alignCenter});
}

#if ESCHER_VIEW_LOGGING
const char* TabViewCell::className() const { return "TabViewCell"; }

void TabViewCell::logAttributes(std::ostream& os) const {
  View::logAttributes(os);
  os << " active=\"" << m_active << "\"";
  os << " name=\"" << m_controller->tabName(m_tabNumber) << "\"";
}
#endif

}  // namespace Escher
