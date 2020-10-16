#include "function_banner_delegate.h"
#include "interactive_curve_view_controller.h"
#include <cmath>
#include <float.h>
#include <assert.h>

using namespace Poincare;

namespace Shared {

InteractiveCurveViewController::InteractiveCurveViewController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, ButtonRowController * header, InteractiveCurveViewRange * interactiveRange, CurveView * curveView, CurveViewCursor * cursor, uint32_t * rangeVersion) :
  SimpleInteractiveCurveViewController(parentResponder, cursor),
  ButtonRowDelegate(header, nullptr),
  m_rangeVersion(rangeVersion),
  m_rangeParameterController(this, inputEventHandlerDelegate, interactiveRange),
  m_zoomParameterController(this, interactiveRange, curveView),
  m_interactiveRange(interactiveRange),
  m_autoButton(this, I18n::Message::DefaultSetting, Invocation([](void * context, void * sender) {
    InteractiveCurveViewController * graphController = (InteractiveCurveViewController *) context;
    graphController->autoButtonAction();
    return true;
  }, this), KDFont::SmallFont),
  m_normalizeButton(this, I18n::Message::Orthonormal, Invocation([](void * context, void * sender) {
    InteractiveCurveViewController * graphController = (InteractiveCurveViewController *) context;
    graphController->normalizeButtonAction();
    return true;
  }, this), KDFont::SmallFont),
  m_navigationButton(this, I18n::Message::Navigate, Invocation([](void * context, void * sender) {
    InteractiveCurveViewController * graphController = (InteractiveCurveViewController *) context;
    graphController->navigationButtonAction();
    return true;
  }, this), KDFont::SmallFont),
  m_rangeButton(this, I18n::Message::Axis, Invocation([](void * context, void * sender) {
    InteractiveCurveViewController * graphController = (InteractiveCurveViewController *) context;
    graphController->rangeParameterController()->setRange(graphController->interactiveRange());
    StackViewController * stack = graphController->stackController();
    stack->push(graphController->rangeParameterController());
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
  float topMarginRatio = isVertical ? cursorTopMarginRatio() : cursorRightMarginRatio();
  float bottomMarginRatio = isVertical ? cursorBottomMarginRatio() : cursorLeftMarginRatio();
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

void InteractiveCurveViewController::setCurveViewAsMainView() {
  header()->setSelectedButton(-1);
  curveView()->selectMainView(true);
  Container::activeApp()->setFirstResponder(this);
  reloadBannerView();
  curveView()->reload();
}

bool InteractiveCurveViewController::handleEvent(Ion::Events::Event event) {
  if (!curveView()->isMainViewSelected()) {
    if (event == Ion::Events::Down) {
      setCurveViewAsMainView();
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
        cursorTopMarginRatio(), cursorRightMarginRatio(), cursorBottomMarginRatio(), cursorLeftMarginRatio(),
        curveView()->pixelWidth()
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

RangeParameterController * InteractiveCurveViewController::rangeParameterController() {
  return &m_rangeParameterController;
}

ViewController * InteractiveCurveViewController::zoomParameterController() {
  return &m_zoomParameterController;
}

int InteractiveCurveViewController::numberOfButtons(ButtonRowController::Position) const {
  if (isEmpty()) {
    return 0;
  }
  return 4;
}

Button * InteractiveCurveViewController::buttonAtIndex(int index, ButtonRowController::Position position) const {
  const Button * buttons[] = {&m_autoButton, &m_normalizeButton, &m_navigationButton, &m_rangeButton};
  return (Button *)buttons[index];
}

Responder * InteractiveCurveViewController::defaultController() {
  return tabController();
}

void InteractiveCurveViewController::viewWillAppear() {
  SimpleInteractiveCurveViewController::viewWillAppear();

  if (m_interactiveRange->zoomAuto()) {
    m_interactiveRange->setDefault();
  }

  /* Warning: init cursor parameter before reloading banner view. Indeed,
   * reloading banner view needs an updated cursor to load the right data. */
  uint32_t newRangeVersion = rangeVersion();
  if ((*m_rangeVersion != newRangeVersion && !isCursorVisible()) || isCursorHanging()) {
    initCursorParameters();
  }
  *m_rangeVersion = newRangeVersion;

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
  /* If possible, round floatBody so that we go to the evaluation of the
   * displayed floatBody */
  floatBody = FunctionBannerDelegate::getValueDisplayedOnBanner(floatBody, textFieldDelegateApp()->localContext(), curveView()->pixelWidth(), false);

  Coordinate2D<double> xy = xyValues(selectedCurveIndex(), floatBody, textFieldDelegateApp()->localContext());
  m_cursor->moveTo(floatBody, xy.x1(), xy.x2());
  interactiveCurveViewRange()->panToMakePointVisible(m_cursor->x(), m_cursor->y(), cursorTopMarginRatio(), cursorRightMarginRatio(), cursorBottomMarginRatio(), cursorLeftMarginRatio(), curveView()->pixelWidth());
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
    m_cursor->x() >= range->xMin() +   cursorLeftMarginRatio() * xRange &&
    m_cursor->x() <= range->xMax() -  cursorRightMarginRatio() * xRange &&
    m_cursor->y() >= range->yMin() + cursorBottomMarginRatio() * yRange &&
    m_cursor->y() <= range->yMax() -    cursorTopMarginRatio() * yRange;
}

int InteractiveCurveViewController::closestCurveIndexVertically(bool goingUp, int currentCurveIndex, Poincare::Context * context) const {
  double x = m_cursor->x();
  double y = m_cursor->y();
  if (std::isnan(y)) {
    y = goingUp ? -INFINITY : INFINITY;
  }
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

bool InteractiveCurveViewController::autoButtonAction() {
  if (m_interactiveRange->zoomAuto()) {
    m_interactiveRange->setZoomAuto(false);
  } else {
    m_interactiveRange->setDefault();
    initCursorParameters();
    setCurveViewAsMainView();
  }
  return m_interactiveRange->zoomAuto();
}

bool InteractiveCurveViewController::normalizeButtonAction() {
  if (m_interactiveRange->zoomNormalize()) {
    m_interactiveRange->setZoomNormalize(false);
  } else {
    m_interactiveRange->setZoomAuto(false);
    m_interactiveRange->normalize();
    setCurveViewAsMainView();
  }
  return m_interactiveRange->zoomNormalize();
}

void InteractiveCurveViewController::navigationButtonAction() {
  m_interactiveRange->setZoomAuto(false);
  stackController()->push(zoomParameterController());
}


}
