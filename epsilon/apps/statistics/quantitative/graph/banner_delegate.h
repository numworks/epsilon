#pragma once

namespace Statistics {

class BannerDelegate {
 public:
  virtual ~BannerDelegate() = default;
  virtual void updateBannerView() = 0;
};

}  // namespace Statistics
