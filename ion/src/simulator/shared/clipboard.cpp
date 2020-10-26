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
  constexpr size_t bufferSize = 32768;
  static char buffer[bufferSize];
  fetchFromSystemClipboard(buffer, bufferSize);
  if (buffer[0] == '\0') {
    return nullptr;
  }

  uint32_t version = crc32Byte(reinterpret_cast<const uint8_t *>(buffer), strlen(buffer));
  if (version == localClipboardVersion) {
    return nullptr;
  }
  return buffer;
}

}
}
