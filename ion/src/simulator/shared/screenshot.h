#ifndef COMPARE_SCREENSHOT_H
#define COMPARE_SCREENSHOT_H

namespace Ion {
namespace Simulator {

class Screenshot {
 public:
  Screenshot(const char * path = nullptr);
  void initEachStep(const char * path) { init(path, true); }
  void init(const char * path, bool eachStep = false);
  void captureStep();
  void capture();
  static Screenshot * commandlineScreenshot();

 private:
  const char * m_path;
  int m_stepNumber;
  bool m_eachStep;
};

}  // namespace Simulator
}  // namespace Ion

#endif /* COMPARE_SCREENSHOT_H */
