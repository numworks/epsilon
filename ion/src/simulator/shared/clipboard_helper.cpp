#include "clipboard_helper.h"

namespace Ion {
namespace Clipboard {

void sendBufferToSystemClipboard() { sendToSystemClipboard(buffer()); }

void fetchSystemClipboardToBuffer() {
  fetchFromSystemClipboard(buffer(), k_bufferSize);
}

}  // namespace Clipboard
}  // namespace Ion
