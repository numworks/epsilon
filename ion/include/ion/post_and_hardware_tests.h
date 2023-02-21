#ifndef ION_DEVICE_SHARED_POST_AND_HARDWARE_TESTS_H
#define ION_DEVICE_SHARED_POST_AND_HARDWARE_TESTS_H

namespace Ion {
namespace POSTAndHardwareTests {

// Tests if battery is Full
bool BatteryOK();
// Tests if VBlank signal is received
bool VBlankOK();
// Tests if tiling of different colors is displayed OK
int LCDDataGlyphFailures();
// Tests if pixels sent fast are displayed OK
int LCDTimingGlyphFailures();
// Tests if uniform tiling is displayed OK
int ColorsLCDPixelFailures();
// Tests if text is displayed OK
bool TextLCDGlyphFailures();

}  // namespace POSTAndHardwareTests
}  // namespace Ion

#endif
