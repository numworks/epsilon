#ifndef SHARED_STORAGE_FUNCTION_BANNER_DELEGATE_H
#define SHARED_STORAGE_FUNCTION_BANNER_DELEGATE_H

#include "storage_function_store.h"
#include "banner_view.h"
#include "curve_view_cursor.h"

namespace Shared {

class StorageFunctionBannerDelegate  {
public:
  constexpr static int k_maxNumberOfCharacters = 50;
  constexpr static int k_maxDigitLegendLength = 11;
protected:
  void reloadBannerViewForCursorOnFunction(CurveViewCursor * cursor, StorageFunction * function, char symbol);
  virtual BannerView * bannerView() = 0;
};

}

#endif
