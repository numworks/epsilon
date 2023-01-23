#ifndef SHARED_INTERACTIVE_CURVE_VIEW_CONTROLLER_H
#define SHARED_INTERACTIVE_CURVE_VIEW_CONTROLLER_H

#include <escher/alternate_empty_view_delegate.h>
#include <escher/button_row_controller.h>
#include <escher/button_state.h>
#include <escher/tab_view_controller.h>
#include <escher/toggleable_dot_view.h>
#include <escher/unequal_view.h>
#include <poincare/coordinate_2D.h>
#include "cursor_view.h"
#include "curve_selection_controller.h"
#include "function_zoom_and_pan_curve_view_controller.h"
#include "plot_view.h"
#include "range_parameter_controller.h"
#include "simple_interactive_curve_view_controller.h"

namespace Shared {

class InteractiveCurveViewController : public SimpleInteractiveCurveViewController, public InteractiveCurveViewRangeDelegate, public Escher::ButtonRowDelegate, public Escher::AlternateEmptyViewDelegate {
public:
  constexpr static int k_graphControllerStackDepth = 1;

  InteractiveCurveViewController(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Escher::ButtonRowController * header, InteractiveCurveViewRange * interactiveRange, AbstractPlotView * curveView, CurveViewCursor * cursor, I18n::Message calculusButtonMessage);

  const char * title() override;
  ViewController::TitlesDisplay titlesDisplay() override { return ViewController::TitlesDisplay::NeverDisplayOwnTitle; }
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  TELEMETRY_ID("Graph");

  RangeParameterController * rangeParameterController();
  ViewController * zoomParameterController();

  int numberOfButtons(Escher::ButtonRowController::Position position) const override;
  Escher::AbstractButtonCell * buttonAtIndex(int index, Escher::ButtonRowController::Position position) const override;

  Escher::Responder * responderWhenEmpty() override;

  void viewWillAppear() override;
  void willExitResponderChain(Responder * nextFirstResponder) override;
  bool textFieldDidFinishEditing(Escher::AbstractTextField * textField, const char * text, Ion::Events::Event event) override;
  bool textFieldDidReceiveEvent(Escher::AbstractTextField * textField, Ion::Events::Event event) override;

  virtual bool openMenuForCurveAtIndex(int index) = 0;
  virtual void moveCursorAndCenterIfNeeded(double t);

protected:
  constexpr static float k_maxFloat = InteractiveCurveViewRange::k_maxFloat;

  Escher::TabViewController * tabController() const;
  Escher::StackViewController * stackController() const;

  virtual void initCursorParameters(bool ignoreMargins) = 0;
  virtual bool moveCursorVertically(int direction) = 0;
  bool isCursorVisibleAtPosition(Poincare::Coordinate2D<float> position, bool ignoreMargins = false, bool acceptNanOrInfiniteY = false);
  bool isCursorCurrentlyVisible(bool ignoreMargins = false, bool acceptNanOrInfiniteY = false) {
    return isCursorVisibleAtPosition(Poincare::Coordinate2D<float>(m_cursor->x(), m_cursor->y()), ignoreMargins, acceptNanOrInfiniteY);
  }

  virtual bool selectedModelIsValid() const = 0;
  virtual Poincare::Coordinate2D<double> selectedModelXyValues(double t) const = 0;
  virtual CurveSelectionController * curveSelectionController() const = 0;

  // Closest vertical curve helper
  int closestCurveIndexVertically(bool goingUp, int currentSelectedCurve, Poincare::Context * context, int currentSubCurveIndex = 0, int * subCurveIndex = nullptr) const;
  virtual bool closestCurveIndexIsSuitable(int newIndex, int currentIndex, int newSubIndex, int currentSubIndex) const;
  virtual int selectedCurveIndex(bool relativeIndex = true) const = 0;
  virtual Poincare::Coordinate2D<double> xyValues(int curveIndex, double t, Poincare::Context * context, int subCurveIndex = 0) const = 0;
  virtual bool suitableYValue(double y) const { return true; }
  virtual int numberOfCurves() const = 0;
  virtual int numberOfSubCurves(int curveIndex) const = 0;
  virtual bool isAlongY(int curveIndex) const = 0;

  // SimpleInteractiveCurveViewController
  bool handleEnter() override { return openMenu(); }
  bool handleZoom(Ion::Events::Event event) override;

  int m_selectedSubCurveIndex;
private:
  constexpr static KDFont::Size k_buttonFont = KDFont::Size::Small;
  void refreshCursor(bool ignoreMargins = false, bool forceFiniteY = false);

  // InteractiveCurveViewRangeDelegate
  float addMargin(float x, float range, bool isVertical, bool isMin) const override;
  void updateZoomButtons() override;
  void refreshCursorAfterComputingRange() override { refreshCursor(true, true); }

  void setCurveViewAsMainView(bool resetInterrupted, bool forceReload);
  bool openMenu() { return openMenuForCurveAtIndex(selectedCurveIndex()); };

  // Button invocations
  Escher::Invocation autoButtonInvocation();
  Escher::Invocation rangeButtonInvocation();
  Escher::Invocation navigationButtonInvocation();
  Escher::Invocation calculusButtonInvocation();

  RangeParameterController m_rangeParameterController;
  FunctionZoomAndPanCurveViewController m_zoomParameterController;
  InteractiveCurveViewRange * m_interactiveRange;
  // Auto button
  Escher::ButtonState m_autoButton;
  Escher::ToggleableDotView m_autoDotView;
  // Range button
  Escher::ButtonState m_rangeButton;
  Escher::UnequalView m_rangeUnequalView;
  // Navigation button
  Escher::AbstractButtonCell m_navigationButton;
  // Calculus button
  Escher::AbstractButtonCell m_calculusButton;
};

}

#endif
