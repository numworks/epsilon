#ifndef COMPARE_SCREENSHOT_H
#define COMPARE_SCREENSHOT_H

namespace Ion {
namespace CompareScreenshot {

void saveCompareScreenshotTo(const char * path);
bool isCompareScreenshotEnabled();
void saveCompareScreenshot();

}  // namespace CompareScreenshot
}  // namespace Ion

#endif /* COMPARE_SCREENSHOT_H */
