#ifndef ION_CLIPBOARD_H
#define ION_CLIPBOARD_H

namespace Ion {
namespace Clipboard {

/* Buffer text to send to system clipboard */
void write(const char* text);

/* Returns the system's clipboard text if it differs from the text previously
 * copied, and nullptr otherwise. */
const char* read();

void sendBufferToSystemClipboard();
void fetchSystemClipboardToBuffer();

}  // namespace Clipboard
}  // namespace Ion

#endif
