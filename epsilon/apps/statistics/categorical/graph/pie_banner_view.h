#pragma once

#include <apps/i18n.h>
#include <escher/buffer_text_view.h>
#include <escher/palette.h>
#include <escher/view.h>

#include "../data/store.h"

namespace Statistics::Categorical {

class PieBannerView : public Escher::View {
 public:
  PieBannerView(Store* store)
      : m_categoryTitle({.style = {.backgroundColor = k_backgroundColor},
                         .horizontalAlignment = KDGlyph::k_alignCenter}),
        m_store(store) {}

  void toggleSelection(bool isSelected) {
    if (m_isSelected != isSelected) {
      m_isSelected = isSelected;
    }
  }

  void setCategory(int category) {
    if (m_category != category) {
      m_category = category;
      char buffer[20];
      m_store->getCategoryName(m_category, buffer, sizeof(buffer));
      m_categoryTitle.setText(buffer);
    }
  }

  void setGroup(int group) {
    if (m_group != group) {
      m_group = group;
      m_category = k_invalidGroupOrCategory;
    }
  }

  void reload() { markWholeFrameAsDirty(); }

 private:
  static constexpr KDColor k_backgroundColor = Escher::Palette::GrayMiddle;
  void drawRect(KDContext* ctx, KDRect rect) const override {
    ctx->fillRect(bounds(), k_backgroundColor);
  }

  void layoutSubviews(bool force = false) override {
    KDRect titleRect = KDRect(0, 0, bounds().width(), 50);
    setChildFrame(&m_categoryTitle, titleRect, force);
  }

  Escher::View* subviewAtIndex(int index) override { return &m_categoryTitle; }
  int numberOfSubviews() const override {
    if (!m_isSelected) {
      return 0;
    }
    return 1;
  }
  Escher::BufferTextView<20> m_categoryTitle;
  Store* m_store;
  static constexpr uint8_t k_invalidGroupOrCategory = UINT8_MAX;
  uint8_t m_category = k_invalidGroupOrCategory;
  uint8_t m_group = k_invalidGroupOrCategory;
  bool m_isSelected = true;
};

}  // namespace Statistics::Categorical
