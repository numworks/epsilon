#ifndef COMPARE_SCREENSHOT_H
#define COMPARE_SCREENSHOT_H

namespace Ion {
namespace Simulator {

class Screenshot {
 public:
  Screenshot(const char * path = nullptr);
  void init(const char * path);
  void capture();
  static Screenshot * commandlineScreenshot();

 private:
  const char * m_path;
};

}  // namespace Simulator
}  // namespace Ion

#endif /* COMPARE_SCREENSHOT_H */
