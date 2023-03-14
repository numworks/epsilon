#ifndef SHARED_SUB_MENU_CELL_H
#define SHARED_SUB_MENU_CELL_H

#include <escher/chevron_view.h>
#include <escher/glyphs_view.h>
#include <escher/menu_cell.h>

/* This class only purpose is to override the default constructor for
 * the sub label to align it on the right.*/

namespace Shared {

template <typename Label, typename SubLabel>
class SubMenuCell
    : public Escher::MenuCell<Label, SubLabel, Escher::ChevronView> {
#ifndef PLATFORM_DEVICE
  // Poor's man constraint
  static_assert(std::is_base_of<Escher::GlyphsView, Label>(),
                "Label is not a GlyphsView");
#endif
  SubMenuCell()
      : Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                         Escher::ChevronView>() {
    m_subLabel.setGlyphFormat(k_rightAlignedSubLabel);
  }

  constexpr static KDGlyph::Format k_rightAlignedSubLabel{
      .horizontalAlignment = KDGlyph::k_alignRight,
      .glyphColor = Escher::Palette::GrayDark,
      .font = KDFont::Size::Small};
};

}  // namespace Shared

#endif
