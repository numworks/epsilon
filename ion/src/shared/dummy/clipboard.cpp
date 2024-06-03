#include <ion.h>
#include <ion/clipboard.h>

namespace Ion {
namespace Clipboard {

char *buffer() { return nullptr; }

void write(const char *text) {}

const char *read() { return ""; }

void sendBufferToSystemClipboard() {}

void fetchSystemClipboardToBuffer() {}

}  // namespace Clipboard
}  // namespace Ion
