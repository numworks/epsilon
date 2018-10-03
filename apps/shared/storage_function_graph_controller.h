#ifndef SHARED_STORAGE_FUNCTION_GRAPH_CONTROLLER_H
#define SHARED_STORAGE_FUNCTION_GRAPH_CONTROLLER_H

#include <escher.h>
#include "initialisation_parameter_controller.h"
#include "storage_function_banner_delegate.h"
#include "interactive_curve_view_controller.h"
#include "storage_function_store.h"
#include "storage_function_graph_view.h"
#include "storage_function_curve_parameter_controller.h"
#include "text_field_delegate_app.h"
#include <assert.h>
#include <cmath>
#include <float.h>

namespace Shared {

template <class T>
class StorageFunctionGraphController : public InteractiveCurveViewController, public StorageFunctionBannerDelegate<T> {
public:
  StorageFunctionGraphController(Responder * parentResponder, ButtonRowController * header, InteractiveCurveViewRange * interactiveRange, CurveView * curveView, CurveViewCursor * cursor, int * indexFunctionSelectedByCursor, uint32_t * modelVersion, uint32_t * rangeVersion, Poincare::Preferences::AngleUnit * angleUnitVersion) :
    InteractiveCurveViewController(parentResponder, header, interactiveRange, curveView, cursor, modelVersion, rangeVersion),
    m_initialisationParameterController(this, interactiveRange),
    m_angleUnitVersion(angleUnitVersion),
    m_indexFunctionSelectedByCursor(indexFunctionSelectedByCursor)
  {
  }

  bool isEmpty() const override {
    if (functionStore()->numberOfActiveFunctions() == 0) {
      return true;
    }
    return false;
  }
  ViewController * initialisationParameterController() override {
    return &m_initialisationParameterController;
  }
  void viewWillAppear() override {
    functionGraphView()->setCursorView(cursorView());
    functionGraphView()->setBannerView(this->bannerView());
    functionGraphView()->setAreaHighlight(NAN,NAN);

    if (functionGraphView()->context() == nullptr) {
      TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
      functionGraphView()->setContext(myApp->localContext());
    }
    Poincare::Preferences::AngleUnit newAngleUnitVersion = Poincare::Preferences::sharedPreferences()->angleUnit();
    if (*m_angleUnitVersion != newAngleUnitVersion) {
      *m_angleUnitVersion = newAngleUnitVersion;
      initCursorParameters();
    }
    InteractiveCurveViewController::viewWillAppear();
  }

protected:
  constexpr static float k_cursorTopMarginRatio = 0.068f;   // (cursorHeight/2)/graphViewHeight
  constexpr static float k_cursorBottomMarginRatio = 0.15f; // (cursorHeight/2+bannerHeigh)/graphViewHeight
  void reloadBannerView() override {
    if (functionStore()->numberOfActiveFunctions() == 0) {
      return;
    }
    T f = functionStore()->activeFunctionAtIndex(indexFunctionSelectedByCursor());
    this->reloadBannerViewForCursorOnFunction(m_cursor, &f, functionStore()->symbol());
  }
  bool handleEnter() override {
    T f = functionStore()->activeFunctionAtIndex(indexFunctionSelectedByCursor());
    curveParameterController()->setFunction(&f);
    StackViewController * stack = stackController();
    stack->push(curveParameterController());
    return true;
  }
  int indexFunctionSelectedByCursor() const {
    return *m_indexFunctionSelectedByCursor;
  }
  virtual void selectFunctionWithCursor(int functionIndex) {
    *m_indexFunctionSelectedByCursor = functionIndex;
  }
  virtual double defaultCursorAbscissa() {
    return (interactiveCurveViewRange()->xMin()+interactiveCurveViewRange()->xMax())/2.0f;
  }
private:
  /* When y auto is ticked, we use a display margin to be ensure that the user
   * can move the cursor along the curve without panning the window */
  constexpr static float k_displayTopMarginRatio = 0.09f;
  constexpr static float k_displayBottomMarginRatio = 0.2f;

  // InteractiveCurveViewController
  float displayTopMarginRatio() override { return k_displayTopMarginRatio; }
  float displayBottomMarginRatio() override { return k_displayBottomMarginRatio; }
  // InteractiveCurveViewRangeDelegate
  InteractiveCurveViewRangeDelegate::Range computeYRange(InteractiveCurveViewRange * interactiveCurveViewRange) override {
    TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
    float min = FLT_MAX;
    float max = -FLT_MAX;
    float xMin = interactiveCurveViewRange->xMin();
    float xMax = interactiveCurveViewRange->xMax();
    if (functionStore()->numberOfActiveFunctions() <= 0) {
      InteractiveCurveViewRangeDelegate::Range range;
      range.min = xMin;
      range.max = xMax;
      return range;
    }
    for (int i=0; i<functionStore()->numberOfActiveFunctions(); i++) {
      T f = functionStore()->activeFunctionAtIndex(i);
      float y = 0.0f;
      float res = curveView()->resolution();
      /* Scan x-range from the middle to the extrema in order to get balanced
       * y-range for even functions (y = 1/x). */
      for (int j = -res/2; j <= res/2; j++) {
        float x = (xMin+xMax)/2.0+(xMax-xMin)*j/res;
        y = f.evaluateAtAbscissa(x, myApp->localContext());
        if (!std::isnan(y) && !std::isinf(y)) {
          min = min < y ? min : y;
          max = max > y ? max : y;
        }
      }
    }
    InteractiveCurveViewRangeDelegate::Range range;
    range.min = min;
    range.max = max;
    return range;
  }

  void initRangeParameters() override {
    interactiveCurveViewRange()->setDefault();
    initCursorParameters();
    selectFunctionWithCursor(0);
  }
  void initCursorParameters() override {
    double x = defaultCursorAbscissa();
    TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
    int functionIndex = 0;
    double y = 0;
    do {
      T firstFunction = functionStore()->activeFunctionAtIndex(functionIndex++);
      y = firstFunction.evaluateAtAbscissa(x, myApp->localContext());
    } while ((std::isnan(y) || std::isinf(y)) && functionIndex < functionStore()->numberOfActiveFunctions());
    m_cursor->moveTo(x, y);
    functionIndex = (std::isnan(y) || std::isinf(y)) ? 0 : functionIndex - 1;
    selectFunctionWithCursor(functionIndex);
    interactiveCurveViewRange()->panToMakePointVisible(x, y, k_displayTopMarginRatio, k_cursorRightMarginRatio, k_displayBottomMarginRatio, k_cursorLeftMarginRatio);
  }
  bool moveCursorVertically(int direction) override {
    T actualFunction = functionStore()->activeFunctionAtIndex(indexFunctionSelectedByCursor());
    TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
    double y = actualFunction.evaluateAtAbscissa(m_cursor->x(), myApp->localContext());
    T nextFunction = actualFunction;
    double nextY = direction > 0 ? DBL_MAX : -DBL_MAX;
    for (int i = 0; i < functionStore()->numberOfActiveFunctions(); i++) {
      T f = functionStore()->activeFunctionAtIndex(i);
      double newY = f.evaluateAtAbscissa(m_cursor->x(), myApp->localContext());
      bool isNextFunction = direction > 0 ? (newY > y && newY < nextY) : (newY < y && newY > nextY);
      if (isNextFunction) {
        selectFunctionWithCursor(i);
        nextY = newY;
        nextFunction = f;
      }
    }
    if (nextFunction == actualFunction) {
      return false;
    }
    m_cursor->moveTo(m_cursor->x(), nextY);
    interactiveCurveViewRange()->panToMakePointVisible(m_cursor->x(), m_cursor->y(), k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
    return true;
  }
  CurveView * curveView() override {
    return functionGraphView();
  }
  uint32_t modelVersion() override {
    return functionStore()->storeChecksum();
  }
  uint32_t rangeVersion() override {
    return interactiveCurveViewRange()->rangeChecksum();
  }
  bool isCursorVisible() override {
    return interactiveCurveViewRange()->isCursorVisible(k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
  }
  virtual StorageFunctionGraphView<T> * functionGraphView() = 0;
  virtual View * cursorView() = 0;
  virtual StorageFunctionStore<T> * functionStore() const = 0;
  virtual StorageFunctionCurveParameterController<T> * curveParameterController() = 0;
  InitialisationParameterController m_initialisationParameterController;
  Poincare::Preferences::AngleUnit * m_angleUnitVersion;
  int * m_indexFunctionSelectedByCursor;
};

}

#endif
