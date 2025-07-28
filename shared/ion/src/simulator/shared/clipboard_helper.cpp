#include "clipboard_helper.h"

#include "window.h"

namespace Ion {
namespace Clipboard {

void sendBufferToSystemClipboard() {
  if (!Simulator::Window::isHeadless()) {
    sendToSystemClipboard(buffer());
  }
}

void fetchSystemClipboardToBuffer() {
  if (!Simulator::Window::isHeadless()) {
    fetchFromSystemClipboard(buffer(), k_bufferSize);
  }
}

}  // namespace Clipboard
}  // namespace Ion
