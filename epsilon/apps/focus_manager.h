#pragma once

#include <escher/blink_timer.h>
#include <escher/theme/components.h>
#include <escher/view.h>
#include <ion/display.h>
#include <ion/events.h>
#include <kandinsky/coordinate.h>
#include <stdint.h>

#include <array>

/* FocusManager sets and keep track of the focused view. It also displays an
 * indicator to visualize which part of the screen is being focused on. */

class FocusManager : public Escher::View {
 public:
  // Return true if window should be redrawn. Use blinkTimer to focus on cursor.
  bool dispatchEvent(Ion::Events::Event event,
                     const Escher::BlinkTimer& blinkTimer);
  // Display an indicator representing the screen and the focused screen.
  void drawRect(KDContext* ctx, KDRect rect) const override;

  bool isActive() const { return m_isActive; }

 private:
  // Events that will always reset focus
  constexpr static std::array<Ion::Events::Event, 7> k_eventsThatResetFocus = {
      Ion::Events::Home,    Ion::Events::ShiftMinus, Ion::Events::Back,
      Ion::Events::Toolbox, Ion::Events::Var,        Ion::Events::OK,
      Ion::Events::EXE};

  /* Focus parameters */
  constexpr static int k_factor = 2;
  constexpr static KDCoordinate k_width = Ion::Display::Width / k_factor;
  constexpr static KDCoordinate k_height = Ion::Display::Height / k_factor;

  /* Navigation parameters */
  /* |          Ion::Display::Width             |
   * | k_width |         k_hiddenWidth          |
   * | k_width | k_step * k_maxHorizontalOffset |
   */
  constexpr static KDCoordinate k_hiddenWidth = Ion::Display::Width - k_width;
  constexpr static KDCoordinate k_hiddenHeight =
      Ion::Display::Height - k_height;
  // Step has been selected to divide both width and height, and to be practical
  constexpr static KDCoordinate k_step = 40;
  static_assert(k_hiddenWidth % k_step == 0);
  static_assert(k_hiddenHeight % k_step == 0);

  static_assert(k_hiddenWidth <= UINT8_MAX * k_step,
                "k_maxHorizontalOffset can't be uint8_t");
  constexpr static uint8_t k_maxHorizontalOffset = k_hiddenWidth / k_step;
  static_assert(k_hiddenHeight <= UINT8_MAX * k_step,
                "k_maxVerticalOffset can't be uint8_t");
  constexpr static uint8_t k_maxVerticalOffset = k_hiddenHeight / k_step;

  /* Visual indicator parameters */
  constexpr static KDCoordinate k_indicatorOffset = 6;
  constexpr static KDCoordinate k_indicatorBorder = 1;
  constexpr static Escher::ThemeComponent k_indicatorColor =
      Escher::ThemeComponent::MagnifyIndicator;
  constexpr static Escher::ThemeComponent k_indicatorBackgroundColor =
      Escher::ThemeComponent::TransparencyBase;
  constexpr static uint8_t k_blendLevel = UINT8_MAX / 2;
  /* The indicator is displayed with a k_indicatorFactorNum/k_indicatorFactorDen
   * ratio compared to tha actual screen size. The values here have been
   * selected for round values, and to be practical */
  constexpr static KDCoordinate k_indicatorRatioDen = 40;
  constexpr static KDCoordinate k_indicatorRatioNum = 3;
  constexpr static KDCoordinate k_indicatorWidth =
      (Ion::Display::Width * k_indicatorRatioNum) / k_indicatorRatioDen;
  constexpr static KDCoordinate k_indicatorHeight =
      (Ion::Display::Height * k_indicatorRatioNum) / k_indicatorRatioDen;
  constexpr static KDCoordinate k_indicatorFocusedWidth =
      (k_width * k_indicatorRatioNum) / k_indicatorRatioDen;
  constexpr static KDCoordinate k_indicatorFocusedHeight =
      (k_height * k_indicatorRatioNum) / k_indicatorRatioDen;
  constexpr static KDCoordinate k_indicatorStep =
      (k_step * k_indicatorRatioNum) / k_indicatorRatioDen;
  static_assert((Ion::Display::Width * k_indicatorRatioNum) %
                    k_indicatorRatioDen ==
                0);
  static_assert((Ion::Display::Height * k_indicatorRatioDen) %
                    k_indicatorRatioDen ==
                0);
  static_assert((k_width * k_indicatorRatioNum) % k_indicatorRatioDen == 0);
  static_assert((k_height * k_indicatorRatioNum) % k_indicatorRatioDen == 0);
  static_assert((k_step * k_indicatorRatioNum) % k_indicatorRatioDen == 0);

  void focusOn(KDPoint point);

  uint8_t m_horizontalOffset = 0;
  uint8_t m_verticalOffset = 0;
  bool m_isActive = false;
};
