#ifndef STATISTICS_BOX_BANNER_VIEW_H
#define STATISTICS_BOX_BANNER_VIEW_H

#include <apps/i18n.h>
#include <apps/shared/banner_view.h>
#include <escher/buffer_text_view.h>
#include <escher/message_text_view.h>

namespace Statistics {

class BoxBannerView : public Shared::BannerView {
 public:
  BoxBannerView();
  Escher::BufferTextView* seriesName() { return &m_seriesName; }
  Escher::BufferTextView* calculationValue() { return &m_calculationBuffer; }

 private:
  constexpr static int k_numberOfSubviews = 2;
  int numberOfSubviews() const override { return k_numberOfSubviews; }
  Escher::View* subviewAtIndex(int index) override;
  Escher::BufferTextView m_seriesName;
  Escher::BufferTextView m_calculationBuffer;
};

}  // namespace Statistics

#endif
