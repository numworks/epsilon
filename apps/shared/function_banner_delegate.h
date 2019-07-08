#ifndef SHARED_FUNCTION_BANNER_DELEGATE_H
#define SHARED_FUNCTION_BANNER_DELEGATE_H

#include "function_store.h"
#include "xy_banner_view.h"
#include "curve_view_cursor.h"

namespace Shared {

class FunctionBannerDelegate  {
public:
  constexpr static int k_maxNumberOfCharacters = 50;
protected:
  void reloadBannerViewForCursorOnFunction(CurveViewCursor * cursor, Ion::Storage::Record record, FunctionStore * functionStore);
  virtual XYBannerView * bannerView() = 0;
};

}

#endif
