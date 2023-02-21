#ifndef ION_CLIPBOARD_HELPER_H
#define ION_CLIPBOARD_HELPER_H

#include <stddef.h>

namespace Ion {
namespace Clipboard {

void sendToSystemClipboard(const char* text);
void fetchFromSystemClipboard(char* buffer, size_t bufferSize);

}  // namespace Clipboard
}  // namespace Ion

#endif
