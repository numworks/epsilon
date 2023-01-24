#include "function_banner_delegate.h"
#include "interactive_curve_view_controller.h"
#include "text_field_delegate_app.h"
#include <escher/tab_view_controller.h>
#include <cmath>
#include <float.h>
#include <assert.h>

using namespace Escher;
using namespace Poincare;

namespace Shared {

InteractiveCurveViewController::InteractiveCurveViewController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, ButtonRowController * header, InteractiveCurveViewRange * interactiveRange, AbstractPlotView * curveView, CurveViewCursor * cursor, I18n::Message calculusButtonMessage) :
  SimpleInteractiveCurveViewController(parentResponder, cursor),
  ButtonRowDelegate(header, nullptr),
  m_selectedSubCurveIndex(0),
  m_rangeParameterController(this, inputEventHandlerDelegate, interactiveRange),
  m_zoomParameterController(this, interactiveRange, curveView),
  m_interactiveRange(interactiveRange),
  m_autoButton(this, I18n::Message::DefaultSetting, autoButtonInvocation(), &m_autoDotView, k_buttonFont),
  m_rangeButton(this, I18n::Message::Axis, rangeButtonInvocation(), &m_rangeUnequalView, k_buttonFont),
  m_navigationButton(this, I18n::Message::Navigate, navigationButtonInvocation(), k_buttonFont),
  m_calculusButton(this, calculusButtonMessage, calculusButtonInvocation(), k_buttonFont)
{
  m_autoButton.setState(m_interactiveRange->zoomAuto());
  m_rangeButton.setState(!m_interactiveRange->zoomNormalize());
}

bool InteractiveCurveViewController::handleEvent(Ion::Events::Event event) {
  if (!curveView()->hasFocus()) {
    if (event == Ion::Events::Down) {
      setCurveViewAsMainView(false, false);
      return true;
    }
    if (event == Ion::Events::Up) {
      header()->setSelectedButton(-1);
      tabController()->selectTab();
      return true;
    }
  } else if (event == Ion::Events::Down || event == Ion::Events::Up) {
    int direction = event == Ion::Events::Down ? -1 : 1;
    if (moveCursorVertically(direction)) {
      reloadBannerView();
      interactiveCurveViewRange()->panToMakePointVisible(
        m_cursor->x(), m_cursor->y(),
        cursorTopMarginRatio(), cursorRightMarginRatio(), cursorBottomMarginRatio(), cursorLeftMarginRatio(),
        curveView()->pixelWidth()
      );
      curveView()->reload();
      return true;
    }
    if (event == Ion::Events::Up) {
      curveView()->setFocus(false);
      header()->setSelectedButton(0);
      return true;
    }
    return false;
  } else if (event == Ion::Events::Toolbox) {
    return openMenu();
  }
  return SimpleInteractiveCurveViewController::handleEvent(event);
}

void InteractiveCurveViewController::didBecomeFirstResponder() {
  if (!curveView()->hasFocus()) {
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

AbstractButtonCell * InteractiveCurveViewController::buttonAtIndex(int index, ButtonRowController::Position position) const {
  const AbstractButtonCell * buttons[] = {&m_autoButton, &m_rangeButton, &m_navigationButton, &m_calculusButton};
  return (AbstractButtonCell *)buttons[index];
}

Responder * InteractiveCurveViewController::responderWhenEmpty() {
  tabController()->selectTab();
  return tabController();
}

void InteractiveCurveViewController::viewWillAppear() {
  /* Set to true in case we come from the Navigate menu. */
  static_cast<TabViewController *>(tabController())->setDisplayTabs(true);

  SimpleInteractiveCurveViewController::viewWillAppear();

  m_interactiveRange->computeRanges();
  if (!m_interactiveRange->zoomAuto()) {
    /* InteractiveCurveViewRange already refreshes the cursor when requiring an
     * update to the bottom margin. If auto is off, the margin is never updated
     * and the cursor risks being out of place. */
    refreshCursor();
  }

  if (!curveView()->hasFocus()) {
    curveView()->setFocus(true);
    header()->setSelectedButton(-1);
  }

  curveView()->reload(true);
}

void InteractiveCurveViewController::refreshCursor(bool ignoreMargins, bool forceFiniteY) {
  /* Warning: init cursor parameter before reloading banner view. Indeed,
   * reloading banner view needs an updated cursor to load the right data. */
  double t = m_cursor->t();
  bool cursorHasAPosition = !std::isnan(t);
  bool cursorIsValid = false;

  if (cursorHasAPosition && selectedModelIsValid()) {
    // Move the cursor onto the selected curve
    Poincare::Coordinate2D<double> xy = selectedModelXyValues(t);
    m_cursor->moveTo(t, xy.x1(), xy.x2());
    cursorIsValid = isCursorCurrentlyVisible(ignoreMargins, !forceFiniteY);
  }

  if (!cursorIsValid) {
    initCursorParameters(ignoreMargins);
  }

  reloadBannerView();
}

void InteractiveCurveViewController::willExitResponderChain(Responder * nextFirstResponder) {
  if (nextFirstResponder == tabController()) {
    assert(tabController() != nullptr);
    m_selectedSubCurveIndex = 0;
    curveView()->setFocus(false);
    header()->setSelectedButton(-1);
    /* The curve view controller will not be around to reset interruption when
     * an OnOff event is fired, so they are reset now. */
    curveView()->reload(true);
  }
}

bool InteractiveCurveViewController::textFieldDidFinishEditing(AbstractTextField * textField, const char * text, Ion::Events::Event event) {
  double floatBody;
  if (textFieldDelegateApp()->hasUndefinedValue(text, &floatBody)) {
    return false;
  }
  /* If possible, round floatBody so that we go to the evaluation of the
   * displayed floatBody */
  floatBody = FunctionBannerDelegate::GetValueDisplayedOnBanner(floatBody, textFieldDelegateApp()->localContext(), Poincare::Preferences::sharedPreferences->numberOfSignificantDigits(), curveView()->pixelWidth(), false);
  moveCursorAndCenterIfNeeded(floatBody);
  return true;
}

bool InteractiveCurveViewController::textFieldDidReceiveEvent(AbstractTextField * textField, Ion::Events::Event event) {
  if ((event == Ion::Events::Plus || event == Ion::Events::Minus || event == Ion::Events::Toolbox) && !textField->isEditing()) {
    return handleEvent(event);
  }
  return SimpleInteractiveCurveViewController::textFieldDidReceiveEvent(textField, event);
}

void InteractiveCurveViewController::moveCursorAndCenterIfNeeded(double t) {
  Coordinate2D<double> xy = xyValues(selectedCurveIndex(false), t, textFieldDelegateApp()->localContext(), 0);
  m_cursor->moveTo(t, xy.x1(), xy.x2());
  reloadBannerView();
  if (!isCursorCurrentlyVisible(false, true)) {
    interactiveCurveViewRange()->centerAxisAround(CurveViewRange::Axis::X, m_cursor->x());
    interactiveCurveViewRange()->centerAxisAround(CurveViewRange::Axis::Y, m_cursor->y());
  }
  curveView()->reload();
}

Escher::TabViewController * InteractiveCurveViewController::tabController() const{
  return static_cast<Escher::TabViewController *>(stackController()->parentResponder());
}

StackViewController * InteractiveCurveViewController::stackController() const{
  return (StackViewController *)(parentResponder()->parentResponder()->parentResponder());
}

bool InteractiveCurveViewController::isCursorVisibleAtPosition(Coordinate2D<float> position, bool ignoreMargins, bool acceptNanOrInfiniteY) {
  InteractiveCurveViewRange * range = interactiveCurveViewRange();
  float xRange = range->xMax() - range->xMin();
  float yRange = range->yMax() - range->yMin();
  float x = position.x1(), y = position.x2();
  return x >= range->xMin() + (ignoreMargins ? 0.f : cursorLeftMarginRatio() * xRange) &&
         x <= range->xMax() - (ignoreMargins ? 0.f : cursorRightMarginRatio() * xRange) &&
         ((!std::isfinite(y) && acceptNanOrInfiniteY) ||
          (std::isfinite(y) &&
           y >= range->yMin() + (ignoreMargins ? 0.f : cursorBottomMarginRatio() * yRange) &&
           y <= range->yMax() - (ignoreMargins ? 0.f : cursorTopMarginRatio() * yRange)));
}

int InteractiveCurveViewController::closestCurveIndexVertically(bool goingUp, int currentCurveIndex, Poincare::Context * context, int currentSubCurveIndex, int * newSubCurveIndex) const {
  /* Vertical curves are quite hard to handle when moving the cursor.
   * To simplify things here, we consider vertical curves as if it they were
   * rotated by 90 degrees by swapping x and y when dealing with them. */
  double x = m_cursor->x();
  double y = m_cursor->y();
  if (isAlongY(currentCurveIndex)) {
    double temp = x;
    x = y;
    y = temp;
  }
  if (std::isnan(y)) {
    y = goingUp ? -INFINITY : INFINITY;
  }
  double nextY = goingUp ? DBL_MAX : -DBL_MAX;
  int nextCurveIndex = -1;
  int nextSubCurveIndex = 0;
  int curvesCount = numberOfCurves();
  for (int curveIndex = 0; curveIndex < curvesCount; curveIndex++) {
    for (int subCurveIndex = 0; subCurveIndex < numberOfSubCurves(curveIndex); subCurveIndex++) {
      if (!closestCurveIndexIsSuitable(curveIndex, currentCurveIndex, subCurveIndex, currentSubCurveIndex)) {
        // Nothing to check for
        continue;
      }
      Poincare::Coordinate2D<double> newXY = xyValues(curveIndex, x, context, subCurveIndex);
      double newY = newXY.x2();
      if (isAlongY(curveIndex)) {
        newY = newXY.x1();
      }
      if (!suitableYValue(newY)) {
        continue;
      }
      bool isNextCurve = false;
      /* Choosing the closest vertical curve is quite complex because we need to
       * take care of curves that have the same value at the current x.
       * When moving up, if several curves have the same value y1, we choose the
       * curve:
       * - Of index score lower than the current curve index score if the
       *   current curve has the value y1 at the current x.
       * - Of highest index score possible.
       * When moving down, if several curves have the same value y1, we choose
       * the curve:
       * - Of index score higher than the current curve index score if the
       *   current curve has the value y1 at the current x.
       * - Of lowest index score possible.
       * Index score is computed so that both primary and sub curve (with
       * a lesser weight) indexes are taken into account. */
      int currentIndexScore = 2 * currentCurveIndex + currentSubCurveIndex;
      int newIndexScore = 2 * curveIndex + subCurveIndex;
      if (goingUp) {
        if (newY > y && newY < nextY) {
          isNextCurve = true;
        } else if (newY == nextY) {
          assert(newIndexScore > 2 * nextCurveIndex + nextSubCurveIndex);
          if (newY != y || currentIndexScore < 0 || newIndexScore < currentIndexScore) {
            isNextCurve = true;
          }
        } else if (newY == y && newIndexScore < currentIndexScore) {
          isNextCurve = true;
        }
      } else {
        if (newY < y && newY > nextY) {
          isNextCurve = true;
        } else if (newY == nextY) {
          assert(newIndexScore > 2 * nextCurveIndex + nextSubCurveIndex);
        } else if (newY == y && newIndexScore > currentIndexScore) {
          isNextCurve = true;
        }
      }
      if (isNextCurve) {
        nextY = newY;
        nextCurveIndex = curveIndex;
        nextSubCurveIndex = subCurveIndex;
      }
    }
  }
  if (newSubCurveIndex) {
    *newSubCurveIndex = nextSubCurveIndex;
  }
  return nextCurveIndex;
}

bool InteractiveCurveViewController::closestCurveIndexIsSuitable(int newIndex, int currentIndex, int newSubIndex, int currentSubIndex) const {
  return newIndex != currentIndex || newSubIndex != currentSubIndex;
}

bool InteractiveCurveViewController::handleZoom(Ion::Events::Event event) {
  if (!curveView()->hasFocus()) {
    return false;
  }
  return SimpleInteractiveCurveViewController::handleZoom(event);
}


float InteractiveCurveViewController::addMargin(float y, float range, bool isVertical, bool isMin) const {
  return DefaultAddMargin(y, range, isVertical, isMin, cursorTopMarginRatio(), cursorBottomMarginRatio(), cursorLeftMarginRatio(), cursorRightMarginRatio());
}

void InteractiveCurveViewController::updateZoomButtons() {
  m_autoButton.setState(m_interactiveRange->zoomAuto());
  m_rangeButton.setState(!m_interactiveRange->zoomNormalize());
  header()->reloadButtons();
}

void InteractiveCurveViewController::setCurveViewAsMainView(bool resetInterrupted, bool forceReload) {
  header()->setSelectedButton(-1);
  curveView()->setFocus(true);
  Container::activeApp()->setFirstResponder(this);
  reloadBannerView();
  curveView()->reload(resetInterrupted, forceReload);
}

Invocation InteractiveCurveViewController::autoButtonInvocation() {
  return Invocation::Builder<InteractiveCurveViewController>([](InteractiveCurveViewController * graphController, void * sender) {
    graphController->m_interactiveRange->setZoomAuto(!graphController->m_interactiveRange->zoomAuto());
    graphController->m_interactiveRange->computeRanges();
    if (graphController->m_interactiveRange->zoomAuto()) {
      graphController->setCurveViewAsMainView(true, true);
    }
    return true;
  }, this);
}

Invocation InteractiveCurveViewController::rangeButtonInvocation() {
  return Invocation::Builder<InteractiveCurveViewController>([](InteractiveCurveViewController * graphController, void * sender) {
    graphController->rangeParameterController()->setRange(graphController->interactiveCurveViewRange());
    StackViewController * stack = graphController->stackController();
    stack->push(graphController->rangeParameterController());
    return true;
  }, this);
}

Invocation InteractiveCurveViewController::navigationButtonInvocation() {
  return Invocation::Builder<InteractiveCurveViewController>([](InteractiveCurveViewController * graphController, void * sender) {
    static_cast<TabViewController *>(graphController->tabController())->setDisplayTabs(false);
    graphController->stackController()->push(graphController->zoomParameterController());
    return true;
  }, this);
}

Invocation InteractiveCurveViewController::calculusButtonInvocation() {
  return Invocation::Builder<InteractiveCurveViewController>([](InteractiveCurveViewController * graphController, void * sender) {
    if (graphController->curveSelectionController()->numberOfRows() > 1) {
      graphController->stackController()->push(graphController->curveSelectionController());
    } else {
      graphController->openMenuForCurveAtIndex(0);
    }
    return true;
  }, this);
}

}
