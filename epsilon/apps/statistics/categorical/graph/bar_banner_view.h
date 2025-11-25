#pragma once

#include <apps/i18n.h>
#include <apps/shared/banner_view.h>
#include <escher/buffer_text_view.h>
#include <escher/message_text_view.h>

namespace Statistics {

class BarBannerView : public Shared::BannerView {
 public:
  BarBannerView();
  BannerBufferTextView* categoryNameView() { return &m_categoryName; }
  BannerBufferTextView* groupNameView() { return &m_groupName; }
  BannerBufferTextView* frequencyView() { return &m_frequency; }
  BannerBufferTextView* relativeFrequencyView() { return &m_relativeFrequency; }

  void drawRect(KDContext* ctx, KDRect rect) const override;
  void reload() override { markWholeFrameAsDirty(); }
  void toggleSelection(bool isSelected);

 private:
  constexpr static int k_numberOfSubviews = 4;
  int numberOfSubviews() const override {
    return m_isSelected ? k_numberOfSubviews : 0;
  }
  Escher::View* subviewAtIndex(int index) override;
  bool lineBreakBeforeSubview(Escher::View* subview) const override;

  BannerBufferTextView m_categoryName;
  BannerBufferTextView m_groupName;
  BannerBufferTextView m_frequency;
  BannerBufferTextView m_relativeFrequency;
  bool m_isSelected;
};

}  // namespace Statistics
