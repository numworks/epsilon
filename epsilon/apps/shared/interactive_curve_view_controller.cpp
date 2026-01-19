#include "interactive_curve_view_controller.h"

#include <apps/global_preferences.h>
#include <assert.h>
#include <escher/tab_view_controller.h>
#include <float.h>

#include <cmath>

#include "function_banner_delegate.h"

using namespace Escher;
using namespace Poincare;

namespace Shared {

InteractiveCurveViewController::InteractiveCurveViewController(
    Responder* parentResponder, ButtonRowController* header,
    InteractiveCurveViewRange* interactiveRange, AbstractPlotView* curveView,
    CurveViewCursor* cursor, I18n::Message calculusButtonMessage,
    int* selectedCurveIndex)
    : SimpleInteractiveCurveViewController(parentResponder, cursor),
      ButtonRowDelegate(header, nullptr),
      m_selectedCurveIndex(selectedCurveIndex),
      m_selectedSubCurveIndex(0),
      m_rangeParameterController(this, interactiveRange),
      m_zoomParameterController(this, interactiveRange, curveView),
      m_interactiveRange(interactiveRange),
      m_autoButton(this, I18n::Message::DefaultSetting, autoButtonInvocation(),
                   &m_autoDotView, k_buttonFont),
      m_rangeButton(this, I18n::Message::Axis, rangeButtonInvocation(),
                    &m_rangeUnequalView, k_buttonFont),
      m_navigationButton(this, I18n::Message::Navigate,
                         navigationButtonInvocation(), k_buttonFont),
      m_calculusButton(this, calculusButtonMessage, calculusButtonInvocation(),
                       k_buttonFont) {
  m_autoButton.setState(m_interactiveRange->zoomAndGridUnitAuto());
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
    if (moveCursorVertically(event.direction())) {
      reloadBannerView();
      panToMakeCursorVisible();
      curveView()->reload();
      return true;
    }
    if (event == Ion::Events::Up) {
      curveView()->setFocus(false);
      header()->setSelectedButton(0);
      return true;
    }
  } else if (event == Ion::Events::Toolbox) {
    openMenu();
    return true;
  }
  return SimpleInteractiveCurveViewController::handleEvent(event);
}

void InteractiveCurveViewController::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
    if (!curveView()->hasFocus()) {
      header()->setSelectedButton(0);
    }
  } else if (event.type == ResponderChainEventType::WillExit) {
    if (event.nextFirstResponder == tabController()) {
      assert(tabController() != nullptr);
      curveView()->setFocus(false);
      header()->setSelectedButton(-1);
      /* The curve view controller will not be around to reset interruption when
       * an OnOff event is fired, so they are reset now. */
      curveView()->reload(true);
    }
  } else {
    SimpleInteractiveCurveViewController::handleResponderChainEvent(event);
  }
}

RangeParameterController*
InteractiveCurveViewController::rangeParameterController() {
  return &m_rangeParameterController;
}

ViewController* InteractiveCurveViewController::zoomParameterController() {
  return &m_zoomParameterController;
}

int InteractiveCurveViewController::numberOfButtons(
    ButtonRowController::Position) const {
  if (isEmpty()) {
    return 0;
  }
  return 4;
}

ButtonCell* InteractiveCurveViewController::buttonAtIndex(
    int index, ButtonRowController::Position position) const {
  const ButtonCell* buttons[] = {&m_autoButton, &m_rangeButton,
                                 &m_navigationButton, &m_calculusButton};
  return (ButtonCell*)buttons[index];
}

Responder* InteractiveCurveViewController::responderWhenEmpty() {
  tabController()->selectTab();
  return tabController();
}

void InteractiveCurveViewController::viewWillAppear() {
  /* Set to true in case we come from the Navigate menu. */
  static_cast<TabViewController*>(tabController())->setDisplayTabs(true);

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
  } else {
    panToMakeCursorVisible();
  }

  SimpleInteractiveCurveViewController::viewWillAppear();
}

void InteractiveCurveViewController::refreshCursor(bool ignoreMargins,
                                                   bool forceFiniteY) {
  /* Warning: init cursor parameter before reloading banner view. Indeed,
   * reloading banner view needs an updated cursor to load the right data. */
  double t = m_cursor->t();
  bool cursorHasAPosition = !std::isnan(t);
  bool cursorIsValid = false;

  if (cursorHasAPosition && selectedModelIsValid()) {
    // Move the cursor onto the selected curve
    Poincare::Coordinate2D<double> xy = selectedModelXyValues(t);
    m_cursor->moveTo(t, xy.x(), xy.y());
    cursorIsValid = isCursorCurrentlyVisible(ignoreMargins, !forceFiniteY);
  }

  if (!cursorIsValid) {
    initCursorParameters(ignoreMargins);
  }

  reloadBannerView();
}

bool InteractiveCurveViewController::textFieldDidFinishEditing(
    AbstractTextField* textField, Ion::Events::Event event) {
  double floatBody = ParseInputFloatValue<double>(textField->draftText());
  if (HasUndefinedValue(floatBody)) {
    return false;
  }
  /* If possible, round floatBody so that we go to the evaluation of the
   * displayed floatBody */
  floatBody = FunctionBannerDelegate::GetValueDisplayedOnBanner(
      floatBody, App::app()->localContext(),
      GlobalPreferences::SharedGlobalPreferences()->numberOfSignificantDigits(),
      curveView()->pixelWidth(), false);
  moveCursorAndCenterIfNeeded(floatBody);
  return true;
}

bool InteractiveCurveViewController::textFieldDidReceiveEvent(
    AbstractTextField* textField, Ion::Events::Event event) {
  if ((event == Ion::Events::Plus || event == Ion::Events::Minus ||
       event == Ion::Events::Toolbox) &&
      !textField->isEditing()) {
    return handleEvent(event);
  }
  return SimpleInteractiveCurveViewController::textFieldDidReceiveEvent(
      textField, event);
}

void InteractiveCurveViewController::moveCursorAndCenterIfNeeded(double t) {
  Coordinate2D<double> xy =
      xyValues(selectedCurveIndex(), t, m_selectedSubCurveIndex);
  m_cursor->moveTo(t, xy.x(), xy.y());
  reloadBannerView();
  if (!isCursorCurrentlyVisible(false, true)) {
    interactiveCurveViewRange()->centerAxisAround(OMG::Axis::Horizontal,
                                                  m_cursor->x());
    interactiveCurveViewRange()->centerAxisAround(OMG::Axis::Vertical,
                                                  m_cursor->y());
  }
  curveView()->reload();
}

Escher::TabViewController* InteractiveCurveViewController::tabController()
    const {
  return static_cast<Escher::TabViewController*>(
      stackController()->parentResponder());
}

StackViewController* InteractiveCurveViewController::stackController() const {
  return (StackViewController*)(parentResponder()
                                    ->parentResponder()
                                    ->parentResponder());
}

bool InteractiveCurveViewController::isCursorVisibleAtPosition(
    Coordinate2D<float> position, bool ignoreMargins,
    bool acceptNanOrInfiniteY) {
  InteractiveCurveViewRange* range = interactiveCurveViewRange();
  float xRange = range->xMax() - range->xMin();
  float yRange = range->yMax() - range->yMin();
  float x = position.x(), y = position.y();
  return x >= range->xMin() +
                  (ignoreMargins ? 0.f : cursorLeftMarginRatio() * xRange) &&
         x <= range->xMax() -
                  (ignoreMargins ? 0.f : cursorRightMarginRatio() * xRange) &&
         ((!std::isfinite(y) && acceptNanOrInfiniteY) ||
          (std::isfinite(y) &&
           y >=
               range->yMin() +
                   (ignoreMargins ? 0.f : cursorBottomMarginRatio() * yRange) &&
           y <= range->yMax() -
                    (ignoreMargins ? 0.f : cursorTopMarginRatio() * yRange)));
}

InteractiveCurveViewController::CurveCursor
InteractiveCurveViewController::nextCurveIndexVertically(
    OMG::VerticalDirection direction, int curveIndex, int subCurveIndex) const {
  // 1 - Search for closest curve along X at same abscissa
  CurveCursor curveCursor =
      closestCurveIndex(direction, curveIndex, subCurveIndex, false);
  if (curveCursor.curveIndex < 0) {
    // 2 - Search for closest curve along Y at same ordinate
    curveCursor = closestCurveIndex(direction, curveIndex, subCurveIndex, true);
  }
  return curveCursor;
}

InteractiveCurveViewController::CurveCursor
InteractiveCurveViewController::closestCurveIndex(
    OMG::VerticalDirection direction, int curveIndex, int subCurveIndex,
    bool alongY) const {
  double x = m_cursor->x();
  double y = m_cursor->y();
  double t = alongY ? y : x;
  double f = alongY ? x : y;
  if (std::isnan(f)) {
    f = direction.isUp() ? -INFINITY : INFINITY;
  }
  double nextF = direction.isUp() ? DBL_MAX : -DBL_MAX;
  int nextCurveIndex = -1;
  int nextSubCurveIndex = 0;
  int indexScore = IndexScore(curveIndex, subCurveIndex);
  int curvesCount = numberOfCurves();
  for (int otherCurveIndex = 0; otherCurveIndex < curvesCount;
       otherCurveIndex++) {
    int nSubCurves = numberOfSubCurves(otherCurveIndex);
    assert(0 <= nSubCurves && nSubCurves <= k_maxNumberOfSubcurves);
    if (isAlongY(otherCurveIndex) != alongY) {
      continue;
    }
    for (int otherSubCurveIndex = 0; otherSubCurveIndex < nSubCurves;
         otherSubCurveIndex++) {
      if (otherCurveIndex == curveIndex &&
          otherSubCurveIndex == subCurveIndex) {
        // Nothing to check for
        continue;
      }
      Poincare::Coordinate2D<double> newXY =
          xyValues(otherCurveIndex, t, otherSubCurveIndex);
      double newY = newXY.y();
      double newX = newXY.x();
      double newF = alongY ? newX : newY;
      assert(newY == y || newX == x);
      if (!suitableYValue(newY) || !suitableXValue(newX)) {
        continue;
      }
      bool isNextCurve = false;
      /* Choosing the closest curve is quite complex because we need to take
       * care of curves that have the same value at the current t. When moving
       * up, if several curves have the same value f1, we choose the curve:
       * - Of index score lower than the current curve index score if the
       *   current curve has the value f1 at the current t.
       * - Of highest index score possible.
       * When moving down, if several curves have the same value f1, we choose
       * the curve:
       * - Of index score higher than the current curve index score if the
       *   current curve has the value f1 at the current t.
       * - Of lowest index score possible.
       * Index score is computed so that both primary and sub curve (with
       * a lesser weight) indexes are taken into account. */
      int newIndexScore = IndexScore(otherCurveIndex, otherSubCurveIndex);
      if (direction.isUp()) {
        if (newF > f && newF < nextF) {
          isNextCurve = true;
        } else if (newF == nextF) {
          assert(newIndexScore > IndexScore(nextCurveIndex, nextSubCurveIndex));
          if (newF != f || indexScore < 0 || newIndexScore < indexScore) {
            isNextCurve = true;
          }
        } else if (newF == f && newIndexScore < indexScore) {
          isNextCurve = true;
        }
      } else {
        if (newF < f && newF > nextF) {
          isNextCurve = true;
        } else if (newF == nextF) {
          assert(newIndexScore > IndexScore(nextCurveIndex, nextSubCurveIndex));
        } else if (newF == f && newIndexScore > indexScore) {
          isNextCurve = true;
        }
      }
      if (isNextCurve) {
        nextF = newF;
        nextCurveIndex = otherCurveIndex;
        nextSubCurveIndex = otherSubCurveIndex;
      }
    }
  }
  return {nextCurveIndex, nextSubCurveIndex,
          nextCurveIndex >= 0 ? isAlongY(nextCurveIndex) ? y : x : NAN};
}

bool InteractiveCurveViewController::handleZoom(Ion::Events::Event event) {
  if (!curveView()->hasFocus()) {
    return false;
  }
  return SimpleInteractiveCurveViewController::handleZoom(event);
}

float InteractiveCurveViewController::addMargin(float y, float range,
                                                bool isVertical,
                                                bool isMin) const {
  return DefaultAddMargin(y, range, isVertical, isMin, cursorTopMarginRatio(),
                          cursorBottomMarginRatio(), cursorLeftMarginRatio(),
                          cursorRightMarginRatio());
}

void InteractiveCurveViewController::updateZoomButtons() {
  m_autoButton.setState(m_interactiveRange->zoomAndGridUnitAuto());
  m_rangeButton.setState(!m_interactiveRange->zoomNormalize());
  header()->reloadButtons();
}

void InteractiveCurveViewController::setCurveViewAsMainView(
    bool resetInterrupted, bool forceReload) {
  header()->setSelectedButton(-1);
  curveView()->setFocus(true);
  App::app()->setFirstResponder(this);
  reloadBannerView();
  panToMakeCursorVisible();
  curveView()->reload(resetInterrupted, forceReload);
}

Invocation InteractiveCurveViewController::autoButtonInvocation() {
  return Invocation::Builder<InteractiveCurveViewController>(
      [](InteractiveCurveViewController* graphController, void* sender) {
        bool newAuto =
            !graphController->m_interactiveRange->zoomAndGridUnitAuto();
        graphController->m_interactiveRange->setZoomAuto(newAuto);
        if (newAuto) {
          // Only set grid unit auto to false when user gives a step
          graphController->m_interactiveRange->setGridUnitAuto();
        }
        graphController->m_interactiveRange->computeRanges();
        if (newAuto) {
          graphController->setCurveViewAsMainView(true, true);
        }
        return true;
      },
      this);
}

Invocation InteractiveCurveViewController::rangeButtonInvocation() {
  return Invocation::Builder<InteractiveCurveViewController>(
      [](InteractiveCurveViewController* graphController, void* sender) {
        graphController->rangeParameterController()->setRange(
            graphController->interactiveCurveViewRange());
        StackViewController* stack = graphController->stackController();
        stack->push(graphController->rangeParameterController());
        return true;
      },
      this);
}

Invocation InteractiveCurveViewController::navigationButtonInvocation() {
  return Invocation::Builder<InteractiveCurveViewController>(
      [](InteractiveCurveViewController* graphController, void* sender) {
        static_cast<TabViewController*>(graphController->tabController())
            ->setDisplayTabs(false);
        graphController->stackController()->push(
            graphController->zoomParameterController());
        return true;
      },
      this);
}

Invocation InteractiveCurveViewController::calculusButtonInvocation() {
  return Invocation::Builder<InteractiveCurveViewController>(
      [](InteractiveCurveViewController* graphController, void* sender) {
        if (graphController->curveSelectionController()->numberOfRows() > 1) {
          graphController->stackController()->push(
              graphController->curveSelectionController());
        } else {
          graphController->openMenuForCurveAtIndex(0);
        }
        return true;
      },
      this);
}

}  // namespace Shared
