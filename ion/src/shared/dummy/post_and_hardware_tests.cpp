#include <ion/post_and_hardware_tests.h>

namespace Ion {
namespace POSTAndHardwareTests {

bool BatteryOK() { return true; }

bool VBlankOK() { return true; }

int LCDDataGlyphFailures() { return 0; }

int LCDTimingGlyphFailures() { return 0; }

int ColorsLCDPixelFailures() { return 0; }

bool TextLCDGlyphFailures() { return false; }

}  // namespace POSTAndHardwareTests
}  // namespace Ion
