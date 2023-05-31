#ifndef ION_CLIPBOARD_HELPER_H
#define ION_CLIPBOARD_HELPER_H

#include <stddef.h>

namespace Ion {
namespace Clipboard {

/* The buffer size is chosen to be around the size of a typical large
 * python script, allowing the user to insert most scripts into the
 * simulator using the paste feature. */
constexpr size_t k_bufferSize = 8192;
char* buffer();

void sendToSystemClipboard(const char* text);
void fetchFromSystemClipboard(char* buffer, size_t bufferSize);

}  // namespace Clipboard
}  // namespace Ion

#endif
