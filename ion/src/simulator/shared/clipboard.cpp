#include <ion/clipboard.h>
#include "clipboard_helper.h"
#include <ion.h>
#include <string.h>

namespace Ion {
namespace Clipboard {

uint32_t localClipboardVersion;

void write(const char * text) {
  /* FIXME : Handle the error if need be. */
  sendToSystemClipboard(text);
  localClipboardVersion = crc32Byte(reinterpret_cast<const uint8_t *>(text), strlen(text));
}

const char * read() {
  /* The buffer size is chosen to be around the size of a typical large
   * python script, allowing the user to insert most scripts into the
   * simulator using the paste feature. */
  constexpr size_t bufferSize = 8192;
  static char buffer[bufferSize];
  fetchFromSystemClipboard(buffer, bufferSize);
  if (buffer[0] == '\0') {
    return nullptr;
  }

  char * cursor = &buffer[0];
  while(*cursor) {
    if(*cursor == '\r' && *(cursor + 1) == '\n') {
      memmove(cursor, cursor + 1, strlen(cursor));
    }
    cursor++;
  }

  /* If version has not changed, the user has not copied any text since the
   * last call to write. A copy of the text already exists in
   * Escher::Clipboard, and has been translated to best suit the current app :
   * we return nullptr to use that text.  */
  uint32_t version = crc32Byte(reinterpret_cast<const uint8_t *>(buffer), strlen(buffer));
  if (version == localClipboardVersion) {
    return nullptr;
  }
  return buffer;
}

}
}
