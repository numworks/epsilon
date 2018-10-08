#ifndef GRAPH_STORAGE_GRAPH_CONTROLLER_HELPER_H
#define GRAPH_STORAGE_GRAPH_CONTROLLER_HELPER_H

#include "../../shared/interactive_curve_view_range.h"
#include "../../shared/storage_cartesian_function.h"
#include "../../shared/storage_function_banner_delegate.h"
#include "../../shared/text_field_delegate_app.h"

namespace Graph {

class StorageGraphControllerHelper {
protected:
  constexpr static int k_maxDigitLegendLength = 10;
  bool privateMoveCursorHorizontally(Shared::CurveViewCursor * cursor, int direction, Shared::InteractiveCurveViewRange * range, int numberOfStepsInGradUnit, Shared::StorageCartesianFunction * function, Shared::TextFieldDelegateApp * app, float cursorTopMarginRatio, float cursorRightMarginRatio, float cursorBottomMarginRatio, float cursorLeftMarginRatio);
  void reloadDerivativeInBannerViewForCursorOnFunction(Shared::CurveViewCursor * cursor, Shared::StorageCartesianFunction * function, Shared::TextFieldDelegateApp * app);
  virtual Shared::BannerView * bannerView() = 0;
};

}

#endif
