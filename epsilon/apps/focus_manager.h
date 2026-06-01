#pragma once

#include <escher/blink_timer.h>
#include <escher/theme/components.h>
#include <escher/view.h>
#include <ion/display.h>
#include <ion/events.h>
#include <kandinsky/coordinate.h>
#include <stdint.h>

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
  constexpr static Ion::Events::Event k_eventsThatResetFocus[] = {
      Ion::Events::Home,    Ion::Events::ShiftMinus, Ion::Events::Back,
      Ion::Events::Toolbox, Ion::Events::Var,        Ion::Events::OK,
      Ion::Events::EXE};

  /* Focus parameters */
  constexpr static int k_factor = 2;
  constexpr static KDCoordinate k_width = Ion::Display::Width / k_factor;
  constexpr static KDCoordinate k_height = Ion::Display::Height / k_factor;

  /* Navigation parameters */
  // k_step = GCD(k_width, k_height)
  constexpr static KDCoordinate k_step = 40;
  static_assert(k_width % k_step == 0);
  static_assert(k_height % k_step == 0);
  static_assert(Ion::Display::Width <= UINT8_MAX * k_step * k_factor);
  static_assert(Ion::Display::Height <= UINT8_MAX * k_step * k_factor);
  constexpr static uint8_t k_maxHorizontalOffset = k_width / k_step;
  constexpr static uint8_t k_maxVerticalOffset = k_height / k_step;

  /* Visual indicator parameters */
  constexpr static KDCoordinate k_pixelsPerStep = 3;
  constexpr static KDCoordinate k_indicatorOffset = 6;
  constexpr static KDCoordinate k_indicatorWidth = 1;
  constexpr static Escher::ThemeComponent k_indicatorColor =
      Escher::ThemeComponent::TokenEllipsis;
  constexpr static Escher::ThemeComponent k_indicatorBackgroundColor =
      Escher::ThemeComponent::BackgroundWhite;
  constexpr static uint8_t k_blendLevel = UINT8_MAX / 2;

  void focusOn(KDPoint point);

  uint8_t m_horizontalOffset = 0;
  uint8_t m_verticalOffset = 0;
  bool m_isActive = false;
};
