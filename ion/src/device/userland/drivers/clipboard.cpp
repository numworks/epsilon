#include <ion/clipboard.h>

namespace Ion {
namespace Clipboard {

/* Dummy implementation
 * On the device, the clipboard is fully handled by Escher::Clipboard. */

void write(const char* text) {}

const char* read() { return nullptr; }

void sendBufferToSystemClipboard() {}

void fetchSystemClipboardToBuffer() {}

}  // namespace Clipboard
}  // namespace Ion
