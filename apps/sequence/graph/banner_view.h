#ifndef SEQUENCE_BANNER_VIEW_H
#define SEQUENCE_BANNER_VIEW_H

#include <escher.h>
#include "../../shared/banner_view.h"

namespace Sequence {

class BannerView : public Shared::BannerView {
public:
  BannerView();
private:
  int numberOfSubviews() const override;
  TextView * textViewAtIndex(int i) const override;
  BufferTextView m_abscissaView;
  BufferTextView m_sequenceView;
};

}

#endif
