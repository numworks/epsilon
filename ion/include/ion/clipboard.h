#ifndef ION_CLIPBOARD_H
#define ION_CLIPBOARD_H

#include <stddef.h>

namespace Ion {
namespace Clipboard {

/* Write the text to the system clipboard. */
void write(const char * text);

/* Fill the buffer with text from the system clipboard. */
void read(char * buffer, size_t bufferSize);

}
}

#endif
