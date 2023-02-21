#include "../shared/clipboard_helper.h"

#include <SDL.h>
#include <assert.h>
#include <emscripten.h>
#include <limits.h>
#include <stdint.h>
#include <string.h>

enum class AsyncStatus : uint32_t { Pending, Success, Failure };

/* When using emscripten, the SDL_Clipboard methods do not interact with the
 * system clipboard, but only with an internal buffer. We thus need to
 * implement our own methods via the JavaScript Async Clipboard API.
 *
 * On most browsers, accessing the clipboard is restricted to user-gestures, ie
 * it must be handled directly during certain user input events. This article
 * (https://webkit.org/blog/10855/async-clipboard-api/), which describe the
 * Async Clipboard API for Safari 13.1 states : "The request to write to the
 * clipboard must be triggered during a user gesture. A call to clipboard.write
 * or clipboard.writeText outside the scope of a user gesture (such as "click"
 * or "touch" event handlers) will result in the immediate rejection of the
 * promise returned by the API call."
 *
 * On Chrome and Chromium-based browsers, this restriction has been relaxed.
 * Interacting with the clipboard is possible if a user-gesture was detected
 * beforehand.
 * See this article for reference :
 *   https://discourse.wicg.io/t/user-gesture-restrictions-and-async-code/1640
 */

EM_JS(void, set_clipboard_text, (const char* text), {
  try {
    navigator.clipboard.writeText(UTF8ToString(text));
  } catch (error) {
    console.error(error);
  }
});

EM_JS(void, get_clipboard_text,
      (char* buffer, uint32_t bufferSize, AsyncStatus* status,
       AsyncStatus failure, AsyncStatus success),
      {
        try {
          navigator.clipboard.readText().then(
              function(text) {
                var lenghtBytes =
                    Math.min(lengthBytesUTF8(text) + 1, bufferSize);
                stringToUTF8(text, buffer, lenghtBytes);
                HEAP32[status >> 2] = success;
              },
              function(text) { HEAP32[status >> 2] = failure; });
        } catch (error) {
          console.error(error);
          HEAP32[status >> 2] = failure;
        }
      });

namespace Ion {
namespace Clipboard {

void sendToSystemClipboard(const char* text) {
  /* As the rest of the execution does not depend on the system clipboard being
   * properly filled at this point, the call to set_clipboard_text does not
   * need to be made synchronous. */
  set_clipboard_text(text);
}

void fetchFromSystemClipboard(char* buffer, size_t bufferSize) {
  AsyncStatus lock = AsyncStatus::Pending;
  static_assert(sizeof(size_t) <= sizeof(uint32_t),
                "Cast from size_t to uint32_t may overflow.");
  get_clipboard_text(buffer, static_cast<uint32_t>(bufferSize), &lock,
                     AsyncStatus::Failure, AsyncStatus::Success);
  while (lock == AsyncStatus::Pending) {
    emscripten_sleep_with_yield(10);
  }
  if (lock == AsyncStatus::Success) {
    return;
  }
}

}  // namespace Clipboard
}  // namespace Ion
