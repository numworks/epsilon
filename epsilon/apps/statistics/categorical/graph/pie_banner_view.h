#pragma once

#include <apps/i18n.h>
#include <escher/buffer_text_view.h>
#include <escher/even_odd_buffer_text_cell.h>
#include <escher/message_text_view.h>
#include <escher/palette.h>
#include <escher/solid_color_view.h>
#include <escher/view.h>
#include <shared/poincare_helpers.h>

#include "../data/store.h"

namespace Statistics::Categorical {

/* Side banner view of the pie graph. Displays various information about the
 * selected category */
class PieBannerView : public Escher::View {
  static constexpr KDColor k_backgroundColor = Escher::Palette::GrayMiddle;
  static constexpr KDGlyph::Format k_titleFormat = {
      .style = {.backgroundColor = k_backgroundColor},
      .horizontalAlignment = KDGlyph::k_alignCenter};
  static constexpr KDGlyph::Format k_labelFormat = {
      .style = {.backgroundColor = k_backgroundColor,
                .font = KDFont::Size::Small}};
  static constexpr KDGlyph::Format k_valueFormat = {
      .style = {.backgroundColor = k_backgroundColor,
                .font = KDFont::Size::Small},
      .horizontalAlignment = KDGlyph::k_alignRight};

  static constexpr KDCoordinate k_titleCellWidth = 40;
  static constexpr KDCoordinate k_colorCellHeight = 15;
  static constexpr KDCoordinate k_colorCellWidth = 40;
  static constexpr KDCoordinate k_textHeight = 20;

  static constexpr KDCoordinate k_leftRightMargin = 15;
  static constexpr KDCoordinate k_topBottomMargin = 5;

  static constexpr int k_numberOfSubviews = 7;

 public:
  PieBannerView(Store* store);

  void toggleSelection(bool isSelected);
  void setCategory(int category);
  void setGroup(int group);

 private:
  void drawRect(KDContext* ctx, KDRect rect) const override {
    ctx->fillRect(bounds(), m_isSelected ? k_backgroundColor : KDColorWhite);
  }

  int numberOfSubviews() const override {
    return m_isSelected ? k_numberOfSubviews : 0;
  }
  void layoutSubviews(bool force = false) override;
  Escher::View* subviewAtIndex(int index) override;

  static constexpr int k_precision =
      Escher::AbstractEvenOddBufferTextCell::k_defaultPrecision;
  Escher::BufferTextView<sizeof(Store::Label)> m_categoryTitle;
  Escher::BufferTextView<9> m_colorLabel;
  Escher::BufferTextView<14> m_freqLabel;
  Escher::BufferTextView<13> m_relativeLabel;
  Escher::SolidColorWithBorderView m_colorCell;
  Escher::FloatBufferTextView<k_precision> m_freqValue;
  Escher::FloatBufferTextView<k_precision> m_relativeValue;
  Store* m_store;
  static constexpr uint8_t k_invalidGroupOrCategory = UINT8_MAX;
  uint8_t m_category = k_invalidGroupOrCategory;
  uint8_t m_group = k_invalidGroupOrCategory;
  bool m_isSelected = true;
};

}  // namespace Statistics::Categorical
