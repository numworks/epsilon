#ifndef ION_CLIPBOARD_H
#define ION_CLIPBOARD_H

namespace Ion {
namespace Clipboard {

/* Write the text to the system clipboard. */
void write(const char* text);

/* Returns the system's clipboard text if it differs from the text previously
 * copied, and nullptr otherwise. */
const char* read();

}  // namespace Clipboard
}  // namespace Ion

#endif
