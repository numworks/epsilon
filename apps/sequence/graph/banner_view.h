#ifndef SEQUENCE_BANNER_VIEW_H
#define SEQUENCE_BANNER_VIEW_H

#include <escher.h>
#include "../../shared/banner_view.h"

namespace Sequence {

class BannerView : public Shared::BannerView {
public:
  BannerView();
private:
  static constexpr int k_numberOfSubviews = 2;
  int numberOfSubviews() const override { return k_numberOfSubviews; }
  TextView * textViewAtIndex(int i) const override;
  BufferTextView m_abscissaView;
  BufferTextView m_sequenceView;
};

}

#endif
