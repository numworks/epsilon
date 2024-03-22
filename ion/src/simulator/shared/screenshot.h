#ifndef COMPARE_SCREENSHOT_H
#define COMPARE_SCREENSHOT_H

#include <ion/events.h>

namespace Ion {
namespace Simulator {

class Screenshot {
 public:
  Screenshot(const char* path = nullptr);
  void init(const char* path, bool eachStep = false, bool computeCRC32 = false);
  void capture(Events::Event nextEvent = Events::None);
  static Screenshot* commandlineScreenshot();
  void setSkipIdle(bool skipIdle) { m_skipIdle = skipIdle; }

 private:
  void printCRC32();

  const char* m_path;
  int m_stepNumber;
  bool m_eachStep;
  bool m_computeCRC32;
  bool m_skipIdle;
  uint32_t m_CRC32;
};

}  // namespace Simulator
}  // namespace Ion

#endif
