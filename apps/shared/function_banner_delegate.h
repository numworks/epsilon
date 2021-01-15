#ifndef SHARED_FUNCTION_BANNER_DELEGATE_H
#define SHARED_FUNCTION_BANNER_DELEGATE_H

#include "function_store.h"
#include "xy_banner_view.h"
#include "curve_view_cursor.h"

namespace Shared {

class FunctionBannerDelegate  {
public:
  constexpr static int k_maxNumberOfCharacters = 50;
  /* getValueDisplayedOnBanner returns the value of t as displayed in the
   * banner, unless the difference from t exceeds deltaThreshold. If so,
   * return t. For instance, when a function is plotted between 1.000001 and
   * 1.000003, and the user goes to x = 1.000002, a small deltaThreshold
   * prevents him from being sent to x = 1
   * Note : Due to double encoding, not all values of t can be properly rounded.
   * For instance, x displayed as 0.01 can at best be encoded to x=0.010...02 */
  static double getValueDisplayedOnBanner(double t, Poincare::Context * context, double deltaThreshold, bool roundToZero = false);
protected:
  void reloadBannerViewForCursorOnFunction(CurveViewCursor * cursor, Ion::Storage::Record record, FunctionStore * functionStore, Poincare::Context * context);
  virtual XYBannerView * bannerView() = 0;
};

}

#endif
