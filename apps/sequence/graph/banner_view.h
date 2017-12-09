#ifndef SEQUENCE_BANNER_VIEW_H
#define SEQUENCE_BANNER_VIEW_H

#include <escher.h>
#include "../../shared/banner_view.h"

namespace Sequence {

class BannerView final : public Shared::BannerView {
public:
  BannerView() :
    m_abscissaView(KDText::FontSize::Small, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle),
    m_sequenceView(KDText::FontSize::Small, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle) {}
private:
  int numberOfSubviews() const override;
  TextView * textViewAtIndex(int i) const override;
  BufferTextView m_abscissaView;
  BufferTextView m_sequenceView;
};

}

#endif
