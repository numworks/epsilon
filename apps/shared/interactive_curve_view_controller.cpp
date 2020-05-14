#include "interactive_curve_view_controller.h"
#include <cmath>
#include <float.h>
#include <assert.h>

using namespace Poincare;

namespace Shared {

InteractiveCurveViewController::InteractiveCurveViewController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, ButtonRowController * header, InteractiveCurveViewRange * interactiveRange, CurveView * curveView, CurveViewCursor * cursor, uint32_t * modelVersion, uint32_t * previousModelsVersions, uint32_t * rangeVersion) :
  SimpleInteractiveCurveViewController(parentResponder, cursor),
  ButtonRowDelegate(header, nullptr),
  m_modelVersion(modelVersion),
  m_previousModelsVersions(previousModelsVersions),
  m_rangeVersion(rangeVersion),
  m_rangeParameterController(this, inputEventHandlerDelegate, interactiveRange),
  m_zoomParameterController(this, interactiveRange, curveView),
  m_rangeButton(this, I18n::Message::Axis, Invocation([](void * context, void * sender) {
    InteractiveCurveViewController * graphController = (InteractiveCurveViewController *) context;
    StackViewController * stack = graphController->stackController();
    stack->push(graphController->rangeParameterController());
    return true;
  }, this), KDFont::SmallFont),
  m_zoomButton(this, I18n::Message::Zoom, Invocation([](void * context, void * sender) {
    InteractiveCurveViewController * graphController = (InteractiveCurveViewController *) context;
    StackViewController * stack = graphController->stackController();
    stack->push(graphController->zoomParameterController());
    return true;
  }, this), KDFont::SmallFont),
  m_defaultInitialisationButton(this, I18n::Message::Initialization, Invocation([](void * context, void * sender) {
    InteractiveCurveViewController * graphController = (InteractiveCurveViewController *) context;
    StackViewController * stack = graphController->stackController();
    stack->push(graphController->initialisationParameterController());
    return true;
  }, this), KDFont::SmallFont)
{
}

float InteractiveCurveViewController::addMargin(float y, float range, bool isVertical, bool isMin) {
  /* The provided min or max range limit y is altered by adding a margin.
   * In pixels, the view's height occupied by the vertical range is equal to
   *   viewHeight - topMargin - bottomMargin.
   * Hence one pixel must correspond to
   *   range / (viewHeight - topMargin - bottomMargin).
   * Finally, adding topMargin pixels of margin, say at the top, comes down
   * to adding
   *   range * topMargin / (viewHeight - topMargin - bottomMargin)
   * which is equal to
   *   range * topMarginRatio / ( 1 - topMarginRatio - bottomMarginRatio)
   * where
   *   topMarginRation = topMargin / viewHeight
   *   bottomMarginRatio = bottomMargin / viewHeight.
   * The same goes horizontally.
   */
  float topMarginRatio = isVertical ? cursorTopMarginRatio() : k_cursorRightMarginRatio;
  float bottomMarginRatio = isVertical ? cursorBottomMarginRatio() : k_cursorLeftMarginRatio;
  assert(topMarginRatio + bottomMarginRatio < 1); // Assertion so that the formula is correct
  float ratioDenominator = 1 - bottomMarginRatio - topMarginRatio;
  float ratio = isMin ? -bottomMarginRatio : topMarginRatio;
  /* We want to add slightly more than the required margin, so that
   * InteractiveCurveViewRange::panToMakePointVisible does not think a point is
   * invisible due to precision problems when checking if it is outside the
   * required margin. This is why we add a 1.05f factor. */
  ratio = 1.05f * ratio / ratioDenominator;
  return y + ratio * range;
}

const char * InteractiveCurveViewController::title() {
  return I18n::translate(I18n::Message::GraphTab);
}

bool InteractiveCurveViewController::handleEvent(Ion::Events::Event event) {
  if (!curveView()->isMainViewSelected()) {
    if (event == Ion::Events::Down) {
      header()->setSelectedButton(-1);
      curveView()->selectMainView(true);
      Container::activeApp()->setFirstResponder(this);
      reloadBannerView();
      curveView()->reload();
      return true;
    }
    if (event == Ion::Events::Up) {
      header()->setSelectedButton(-1);
      Container::activeApp()->setFirstResponder(tabController());
      return true;
    }
    return false;
  }
  if (event == Ion::Events::Down || event == Ion::Events::Up) {
    int direction = event == Ion::Events::Down ? -1 : 1;
    if (moveCursorVertically(direction)) {
      interactiveCurveViewRange()->panToMakePointVisible(
        m_cursor->x(), m_cursor->y(),
        cursorTopMarginRatio(), k_cursorRightMarginRatio, cursorBottomMarginRatio(), k_cursorLeftMarginRatio
      );
      reloadBannerView();
      curveView()->reload();
      return true;
    }
    if (event == Ion::Events::Up) {
      curveView()->selectMainView(false);
      header()->setSelectedButton(0);
      return true;
    }
    return false;
  }
  return SimpleInteractiveCurveViewController::handleEvent(event);
}

void InteractiveCurveViewController::didBecomeFirstResponder() {
  if (!curveView()->isMainViewSelected()) {
    header()->setSelectedButton(0);
  }
}

ViewController * InteractiveCurveViewController::rangeParameterController() {
  return &m_rangeParameterController;
}

ViewController * InteractiveCurveViewController::zoomParameterController() {
  return &m_zoomParameterController;
}

int InteractiveCurveViewController::numberOfButtons(ButtonRowController::Position) const {
  if (isEmpty()) {
    return 0;
  }
  return 3;
}

Button * InteractiveCurveViewController::buttonAtIndex(int index, ButtonRowController::Position position) const {
  const Button * buttons[3] = {&m_rangeButton, &m_zoomButton, &m_defaultInitialisationButton};
  return (Button *)buttons[index];
}

Responder * InteractiveCurveViewController::defaultController() {
  return tabController();
}

bool InteractiveCurveViewController::previousModelsWereAllDeleted() {
  bool result = true;
  const int modelsCount = numberOfCurves();
  const int memoizationCount = numberOfMemoizedVersions();

  // Look for a current model that is the same as in the previous version
  for (int i = 0; i < modelsCount; i++) {
    uint32_t currentVersion = modelVersionAtIndex(i);
    for (int j = 0; j < memoizationCount; j++) {
      uint32_t * previousVersion = m_previousModelsVersions + j;
      if (currentVersion == *previousVersion) {
        result = false;
        break;
      }
    }
    if (!result) {
      break;
    }
  }

  // Update the memoization
  for (int i = 0; i < memoizationCount; i++) {
    uint32_t * previousVersion = m_previousModelsVersions + i;
    uint32_t newVersion = modelVersionAtIndex(i);
    if (*previousVersion != newVersion) {
      *previousVersion = newVersion;
    }
  }
  return result;
}

void InteractiveCurveViewController::viewWillAppear() {
  SimpleInteractiveCurveViewController::viewWillAppear();
  uint32_t newModelVersion = modelVersion();
  if (*m_modelVersion != newModelVersion) {
    // Put previousModelsWereAllDeleted first to update the model versions
    if (previousModelsWereAllDeleted() || *m_modelVersion == 0 || numberOfCurves() == 1 || shouldSetDefaultOnModelChange()) {
      interactiveCurveViewRange()->setDefault();
    }
    *m_modelVersion = newModelVersion;
    didChangeRange(interactiveCurveViewRange());
    /* Warning: init cursor parameter before reloading banner view. Indeed,
     * reloading banner view needs an updated cursor to load the right data. */
    initCursorParameters();
  }
  uint32_t newRangeVersion = rangeVersion();
  if (*m_rangeVersion != newRangeVersion) {
    *m_rangeVersion = newRangeVersion;
    if (!isCursorVisible()) {
      initCursorParameters();
    }
  }
  curveView()->setOkView(&m_okView);
  if (!curveView()->isMainViewSelected()) {
    curveView()->selectMainView(true);
    header()->setSelectedButton(-1);
  }
  reloadBannerView();
  curveView()->reload();
}

void InteractiveCurveViewController::viewDidDisappear() {
  *m_rangeVersion = rangeVersion();
}

void InteractiveCurveViewController::willExitResponderChain(Responder * nextFirstResponder) {
  if (nextFirstResponder == tabController()) {
    assert(tabController() != nullptr);
    curveView()->selectMainView(false);
    header()->setSelectedButton(-1);
    curveView()->reload();
  }
}

bool InteractiveCurveViewController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  double floatBody;
  if (textFieldDelegateApp()->hasUndefinedValue(text, floatBody)) {
    return false;
  }
  Coordinate2D<double> xy = xyValues(selectedCurveIndex(), floatBody, textFieldDelegateApp()->localContext());
  m_cursor->moveTo(floatBody, xy.x1(), xy.x2());
  interactiveCurveViewRange()->panToMakePointVisible(m_cursor->x(), m_cursor->y(), cursorTopMarginRatio(), k_cursorRightMarginRatio, cursorBottomMarginRatio(), k_cursorLeftMarginRatio);
  reloadBannerView();
  curveView()->reload();
  return true;
}

bool InteractiveCurveViewController::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  if ((event == Ion::Events::Plus || event == Ion::Events::Minus) && !textField->isEditing()) {
    return handleEvent(event);
  }
  return SimpleInteractiveCurveViewController::textFieldDidReceiveEvent(textField, event);
}

Responder * InteractiveCurveViewController::tabController() const{
  return (stackController()->parentResponder());
}

StackViewController * InteractiveCurveViewController::stackController() const{
  return (StackViewController *)(parentResponder()->parentResponder()->parentResponder());
}

bool InteractiveCurveViewController::isCursorVisible() {
  InteractiveCurveViewRange * range = interactiveCurveViewRange();
  float xRange = range->xMax() - range->xMin();
  float yRange = range->yMax() - range->yMin();
  return
    m_cursor->x() >= range->xMin() +  k_cursorLeftMarginRatio   * xRange &&
    m_cursor->x() <= range->xMax() - k_cursorRightMarginRatio   * xRange &&
    m_cursor->y() >= range->yMin() +  cursorBottomMarginRatio() * yRange &&
    m_cursor->y() <= range->yMax() -     cursorTopMarginRatio() * yRange;
}

int InteractiveCurveViewController::closestCurveIndexVertically(bool goingUp, int currentCurveIndex, Poincare::Context * context) const {
  double x = m_cursor->x();
  double y = m_cursor->y();
  double nextY = goingUp ? DBL_MAX : -DBL_MAX;
  int nextCurveIndex = -1;
  int curvesCount = numberOfCurves();
  for (int i = 0; i < curvesCount; i++) {
    if (!closestCurveIndexIsSuitable(i, currentCurveIndex)) {
      continue;
    }
    double newY = xyValues(i, x, context).x2();
    if (!suitableYValue(newY)) {
      continue;
    }
    bool isNextCurve = false;
    /* Choosing the closest vertical curve is quite complex because we need to
     * take care of curves that have the same value at the current x.
     * When moving up, if several curves have the same value y1, we choose the
     * curve:
     * - Of index lower than the current curve index if the current curve has
     *   the value y1 at the current x.
     * - Of highest index possible.
     * When moving down, if several curves have the same value y1, we choose the
     * curve:
     * - Of index higher than the current curve index if the current curve has
     *   the value y1 at the current x.
     * - Of lowest index possible. */
    if (goingUp) {
      if (newY > y && newY < nextY) {
        isNextCurve = true;
      } else if (newY == nextY) {
        assert(i > nextCurveIndex);
        if (newY != y || currentCurveIndex < 0 || i < currentCurveIndex) {
          isNextCurve = true;
        }
      } else if (newY == y && i < currentCurveIndex) {
        isNextCurve = true;
      }
    } else {
      if (newY < y && newY > nextY) {
        isNextCurve = true;
      } else if (newY == nextY) {
        assert(i > nextCurveIndex);
      } else if (newY == y && i > currentCurveIndex) {
        isNextCurve = true;
      }
    }
    if (isNextCurve) {
      nextY = newY;
      nextCurveIndex = i;
    }
  }
  return nextCurveIndex;
}

float InteractiveCurveViewController::cursorBottomMarginRatio() {
  return (curveView()->cursorView()->minimalSizeForOptimalDisplay().height()/2+estimatedBannerHeight())/(k_viewHeight-1);
}

float InteractiveCurveViewController::estimatedBannerHeight() const {
  return BannerView::HeightGivenNumberOfLines(estimatedBannerNumberOfLines());
}

}
