#include <ion/clipboard.h>
#include <emscripten.h>
#include <SDL.h>
#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <string.h>

enum class AsyncStatus : uint32_t {
  Pending,
  Success,
  Failure
};

/* When using emscripten, the SDL_Clipboard methods do not interact with the
 * system clipboard, but only with an internal buffer. We thus need to
 * implement our own methods via the JavaScript API.
 * However, we still call the SDL_Clipboard methods as a fallback to preserve
 * the copy-paste feature in case of calls to set_clipboard_text and
 * get_clipboard_text failing. */

EM_JS(void, set_clipboard_text, (const char * text, AsyncStatus * status, AsyncStatus failure, AsyncStatus success), {
  try {
    navigator.clipboard.writeText(UTF8ToString(text)).then(
      function () { HEAP32[status>>2] = success; },
      function () { HEAP32[status>>2] = failure; }
    );
  } catch (error) {
    console.error(error);
    HEAP32[status>>2] = failure;
  }
});

EM_JS(void, get_clipboard_text, (char * buffer, uint32_t bufferSize, AsyncStatus * status, AsyncStatus failure, AsyncStatus success), {
  try {
    navigator.clipboard.readText().then(
      function(text) {
        var lenghtBytes = Math.min(lengthBytesUTF8(text) + 1, bufferSize);
        stringToUTF8(text, buffer, lenghtBytes);
        HEAP32[status>>2] = success;
      },
      function(text) { HEAP32[status>>2] = failure; }
    );
  } catch (error) {
    console.error(error);
    HEAP32[status>>2] = failure;
  }
});

namespace Ion {

void Clipboard::write(const char * text) {
  /* FIXME : Handle the error if need be. */
  /* As the rest of the execution does not depend on the system clipboard being
   * properly filled at this point, the call to set_clipboard_text does not
   * need to be made synchronous. */
  AsyncStatus lock;
  set_clipboard_text(text, &lock, AsyncStatus::Failure, AsyncStatus::Success);
  /* Store a local copy of the text in case the browser does not grant access
   * to the clipboard. */
  SDL_SetClipboardText(text);
}

void Clipboard::read(char * buffer, size_t bufferSize) {
  AsyncStatus lock = AsyncStatus::Pending;
  static_assert(sizeof(size_t) <= sizeof(uint32_t), "Cast from size_t to uint32_t may overflow.");
  get_clipboard_text(buffer, static_cast<uint32_t>(bufferSize), &lock, AsyncStatus::Failure, AsyncStatus::Success);
  while (lock == AsyncStatus::Pending) {
    emscripten_sleep_with_yield(10);
  }
  if (lock == AsyncStatus::Success) {
    return;
  }
  /* If the browser does not grant access to the clipboard, read from a local
   * copy to at least maintain the basic copy-paste functionnality. */
  if (!SDL_HasClipboardText()) {
    buffer[0] = '\0';
    return;
  }
  char * text = SDL_GetClipboardText();
  if (text) {
    strlcpy(buffer, text, bufferSize);
    SDL_free(text);
  }
}

}
