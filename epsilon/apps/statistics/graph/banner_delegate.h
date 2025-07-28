#ifndef STATISTICS_BANNER_DELEGATE_H
#define STATISTICS_BANNER_DELEGATE_H

namespace Statistics {

class BannerDelegate {
 public:
  virtual ~BannerDelegate() = default;
  virtual void updateBannerView() = 0;
};

}  // namespace Statistics

#endif
