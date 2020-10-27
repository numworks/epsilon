#ifndef STATISTICS_BOX_BANNER_VIEW_H
#define STATISTICS_BOX_BANNER_VIEW_H

#include <apps/i18n.h>
#include <escher/buffer_text_view.h>
#include <escher/message_text_view.h>
#include "../shared/banner_view.h"

namespace Statistics {

class BoxBannerView : public Shared::BannerView {
public:
  BoxBannerView();
  BufferTextView * seriesName() { return &m_seriesName; }
  MessageTextView * calculationName() { return &m_calculationName; }
  BufferTextView * calculationValue() { return &m_calculationValue; }
private:
  static constexpr int k_numberOfSubviews = 3;
  int numberOfSubviews() const override { return k_numberOfSubviews; }
  View * subviewAtIndex(int index) override;
  BufferTextView m_seriesName;
  MessageTextView m_calculationName;
  BufferTextView m_calculationValue;
};

}

#endif
