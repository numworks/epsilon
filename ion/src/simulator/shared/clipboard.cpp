#include <ion.h>
#include <ion/clipboard.h>
#include <string.h>

#include "clipboard_helper.h"
#include "window.h"

namespace Ion {
namespace Clipboard {

uint32_t localClipboardVersion;

void write(const char *text) {
  if (Simulator::Window::isHeadless()) {
    // Do not use system clipboard when headless
    return;
  }
  /* FIXME : Handle the error if need be. */
  sendToSystemClipboard(text);
  localClipboardVersion =
      crc32Byte(reinterpret_cast<const uint8_t *>(text), strlen(text));
}

const char *read() {
  if (Simulator::Window::isHeadless()) {
    // Do not use system clipboard when headless
    return nullptr;
  }
  /* The buffer size is chosen to be around the size of a typical large
   * python script, allowing the user to insert most scripts into the
   * simulator using the paste feature. */
  constexpr size_t bufferSize = 8192;
  static char buffer[bufferSize];
  fetchFromSystemClipboard(buffer, bufferSize);
  if (buffer[0] == '\0') {
    return nullptr;
  }

  /* If version has not changed, the user has not copied any text since the
   * last call to write. A copy of the text already exists in
   * Escher::Clipboard, and has been translated to best suit the current app :
   * we return nullptr to use that text.  */
  uint32_t version =
      crc32Byte(reinterpret_cast<const uint8_t *>(buffer), strlen(buffer));
  if (version == localClipboardVersion) {
    return nullptr;
  }
  return buffer;
}

}  // namespace Clipboard
}  // namespace Ion
