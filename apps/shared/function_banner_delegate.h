#ifndef SHARED_FUNCTION_BANNER_DELEGATE_H
#define SHARED_FUNCTION_BANNER_DELEGATE_H

#include "function_store.h"
#include "banner_view.h"
#include "curve_view_cursor.h"

namespace Shared {

class FunctionBannerDelegate  {
public:
  constexpr static int k_maxNumberOfCharacters = 50;
  constexpr static int k_maxDigitLegendLength = 11;
protected:
  void reloadBannerViewForCursorOnFunction(CurveViewCursor * cursor, Function * function, char symbol);
  virtual BannerView * bannerView() = 0;
};

}

#endif
