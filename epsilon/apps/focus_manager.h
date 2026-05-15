#pragma once

#include <escher/window.h>
#include <ion/display.h>
#include <ion/events.h>
#include <kandinsky/coordinate.h>
#include <stdint.h>

class FocusManager {
 public:
  // Return true if window should be redrawn
  bool dispatchEvent(Ion::Events::Event event);

 private:
  // Events that will always reset focus
  constexpr static Ion::Events::Event k_eventsThatResetFocus[] = {
      Ion::Events::Home,    Ion::Events::ShiftMinus, Ion::Events::Back,
      Ion::Events::Toolbox, Ion::Events::Var,        Ion::Events::OK,
      Ion::Events::EXE};
  constexpr static int k_factor = 2;
  // k_step = GCD(Display::Width / k_factor, Display::Height / k_factor)
  constexpr static KDCoordinate k_step = 40;

  static_assert((Ion::Display::Width / k_factor) % k_step == 0);
  static_assert((Ion::Display::Height / k_factor) % k_step == 0);
  static_assert(Ion::Display::Width <= UINT8_MAX * k_step * k_factor);
  static_assert(Ion::Display::Height <= UINT8_MAX * k_step * k_factor);

  constexpr static uint8_t k_maxHorizontalOffset =
      (Ion::Display::Width / k_factor) / k_step;
  constexpr static uint8_t k_maxVerticalOffset =
      (Ion::Display::Height / k_factor) / k_step;

  uint8_t m_horizontalOffset = 0;
  uint8_t m_verticalOffset = 0;
  bool m_isActive = false;
};
