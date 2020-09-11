#include <ion/clipboard.h>

namespace Ion {

/* Dummy implementation
 * On the device, the clipboard is fully handled by Escher::Clipboard. */

void Clipboard::write(const char * text) {}
void Clipboard::read(char * buffer, size_t bufferSize) {}

}
