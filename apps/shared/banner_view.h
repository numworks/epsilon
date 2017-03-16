#ifndef SHARED_BANNER_VIEW_H
#define SHARED_BANNER_VIEW_H

#include <escher.h>

namespace Shared {

class BannerView : public View {
public:
  void setLegendAtIndex(char * text, int index);
  void setMessageAtIndex(I18n::Message text, int index);
  KDSize minimalSizeForOptimalDisplay() const override;
private:
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  int numberOfLines() const;
  virtual TextView * textViewAtIndex(int i) const = 0;
  virtual MessageTextView * messageTextViewAtIndex(int i) const;
};

}

#endif
