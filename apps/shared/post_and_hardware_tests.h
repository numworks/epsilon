#ifndef SHARED_POST_AND_HARDWARE_TESTS_H
#define SHARED_POST_AND_HARDWARE_TESTS_H

#include <kandinsky/color.h>
#include <ion/display.h>

namespace Shared {

class POSTAndHardwareTests {
public:
  static bool BatteryOK();
  static bool VBlankOK();
  static bool TextLCDTestOK();
  /* LCDDataOK carefully tests the LCD controller. It verifies that:
   * - Command/data switching is OK,
   * - Data is correctly sent,
   * - There are no short-circuits between the data wires.
   * LCDDataOK thus sends a tiled pattern (to test command/data switching),
   * where each tile is a checker of a color and its contrary (to tests that
   * Data is sent OK). To test each of the 16 data wires for short-circuits, 16
   * colors are used: 2**k with 0 <= k < 16. */
  static bool LCDDataOK();
private:
  constexpr static int k_stampSize = 8;
  constexpr static int k_numberOfTilingLCDIterations = 20;
  static_assert(Ion::Display::Width % k_stampSize == 0, "Stamps must tesselate the display");
  static_assert(Ion::Display::Height % k_stampSize == 0, "Stamps must tesselate the display");
  static_assert(k_stampSize % 2 == 0, "Even number of XOR needed.");
};

}

#endif

