<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>NumWorks graphing calculator</title>
    <style>
#include "calculator.css"
#include "style/calculator_shared.css"
#include "style/simulator_layout.css"
#include "style/external_shared.css"
    </style>
  </head>
  <body>
    <div class="row">
      <div class="col-calculator">
#include "calculator.html"
        <div class="actions">
          <a id="action-fullscreen" class="action" title="Toggle fullscreen">
            <svg viewBox="0 0 16 16" version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink"><g stroke="none" stroke-width="1" fill="none" fill-rule="evenodd"><g fill="#434343"><path d="M5.075,6.95 L6.918,5.088 L3.938,2.062 L5.955,0.018 L0.052,0.018 L0.052,6.004 L2.098,3.928 L5.075,6.95 Z" class="si-glyph-fill"></path><path d="M16.0034788,9.916 L13.832,12.013 L10.799,8.96 L8.918,10.841 L11.957,13.897 L9.961,15.9813842 L16.0034788,15.9813842 L16.0034788,9.916 Z"></path></g></g></svg>
          </a>
          <a id="action-screenshot" class="action" title="Take a screenshot (Alt: copy to clipboard)">
            <svg viewBox="0 1.5 17 17" version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink"><g stroke="none" stroke-width="1" fill="none" fill-rule="evenodd"><g transform="translate(0.995000, 2.980000)" fill="#434343"><circle cx="7.958" cy="6.958" r="2.958" class="si-glyph-fill"></circle><path d="M14.666,2.042 L10.953,2.042 L9.937,0.031 L6,0.031 L5,2.042 L1.333,2.042 C0.597,2.042 0,2.639 0,3.375 L0,10.625 C0,11.361 0.597,11.959 1.333,11.959 L14.666,11.959 C15.402,11.959 16,11.361 16,10.625 L16,3.375 C16,2.639 15.402,2.042 14.666,2.042 L14.666,2.042 Z M6.953,0.969 L9.047,0.969 L9.047,2.031 L6.953,2.031 L6.953,0.969 L6.953,0.969 Z M8.002,11.033 C5.764,11.033 3.947,9.221 3.947,6.985 C3.947,4.749 5.763,2.937 8.002,2.937 C10.242,2.937 12.057,4.749 12.057,6.985 C12.057,9.221 10.242,11.033 8.002,11.033 L8.002,11.033 Z M14,4.031 L11.953,4.031 L11.953,2.969 L14,2.969 L14,4.031 L14,4.031 Z" class="si-glyph-fill"></path></g></g></svg>
          </a>
          <a id="action-share" class="action" title="Copy a shareable link of the current state">
            <svg class="action-share-icon" viewBox="0 0 16 16" version="1.1" xmlns="http://www.w3.org/2000/svg"><g fill="#434343" stroke="#434343" stroke-width="1.2"><line x1="3.5" y1="8" x2="12" y2="3"></line><line x1="3.5" y1="8" x2="12" y2="13"></line><circle cx="12" cy="3" r="2.5" stroke="none"></circle><circle cx="12" cy="13" r="2.5" stroke="none"></circle><circle cx="3.5" cy="8" r="2.5" stroke="none"></circle></g></svg>
            <svg class="action-share-done" viewBox="0 0 16 16" version="1.1" xmlns="http://www.w3.org/2000/svg" style="display:none"><g stroke="none" stroke-width="1" fill="none" fill-rule="evenodd"><path d="M2.5,8.5 L6,12 L13.5,4" stroke="#34a853" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round"></path></g></svg>
          </a>
        </div>
      </div>
      <div class="col-fullscreen">
        <canvas class="calculator-mirror" width=ION_DISPLAY_WIDTH height=ION_DISPLAY_HEIGHT></canvas>
      </div>
    </div>
    <script src=EM_MODULE_JS></script>
    <script>
#include "calculator.js"
      var ModuleAsync = {
          /* Every event replayed by the simulator is logged here (the same byte
           * stream a .nws file stores after its header): plain events are one
           * byte, an ExternalText event is followed by its null-terminated text.
           * The "share" action turns this back into a ?nws= link. Events injected
           * at boot via ?nws= are replayed too, so the recording stays cumulative. */
          recordedEvents: [],
          onIonEvent: function (byte, isEvent) {
            this.recordedEvents.push(byte & 0xff);
          },
          /* An event stream can be injected with epsilon.html?nws=<base64>.
           * The payload may be a full .nws file (starting with the "NWSF" magic,
           * whose 15-byte header is stripped) or a bare event stream. Either way
           * we only push the events through the pre-existing
           * IonSimulatorEventsPushEventArray export, so this stays compatible with
           * already-deployed wasm builds. */
          onLoaded: function () {
            var nwsBase64 = new URLSearchParams(window.location.search).get('nws');
            if (!nwsBase64) {
              return;
            }
            var binary = atob(nwsBase64.replace(/-/g, '+').replace(/_/g, '/'));
            var bytes = new Uint8Array(binary.length);
            for (var i = 0; i < binary.length; i++) {
              bytes[i] = binary.charCodeAt(i);
            }
            /* NWSF header: "NWSF"(4) + version(8) + properties(1) + language(2) */
            var magic = String.fromCharCode(bytes[0], bytes[1], bytes[2], bytes[3]);
            var events = magic === 'NWSF' ? bytes.subarray(15) : bytes;
            var ptr = this._malloc(events.length);
            this.HEAPU8.set(events, ptr);
            this._IonSimulatorEventsPushEventArray(ptr, events.length, 0, -1);
            this._free(ptr);
          }
      };
      var Module = {
          UTF8ToString: (x => x),
          _IonSoftwareVersion: (() => SOFTWARE_VERSION),
          _IonPatchLevel: (() => COMMIT_HASH)
          };
Calculator(ModuleAsync);
    </script>
    <script>
document.getElementById("action-fullscreen").addEventListener("click", function(e){
  if (document.body.className == "fullscreen") {
    document.body.className = "";
  } else {
    document.body.className = "fullscreen";
  }
});

document.getElementById("action-screenshot").addEventListener("click", function(e) {
  ModuleAsync.takeScreenshot(e);
});

document.getElementById("action-share").addEventListener("click", function(e) {
  /* Url-safe base64 the recorded event stream into a ?nws= link copied to the
   * clipboard (no header: onLoaded injects a bare event stream directly). */
  var binary = "";
  var events = ModuleAsync.recordedEvents;
  for (var i = 0; i < events.length; i++) {
    binary += String.fromCharCode(events[i]);
  }
  var nws = btoa(binary).replace(/\+/g, "-").replace(/[/]/g, "_");
  var url = new URL(window.location.href);
  url.searchParams.set("nws", nws);
  navigator.clipboard.writeText(url.toString()).then(function () {
    /* Briefly swap the share icon for a green tick to confirm the copy. */
    var icon = document.querySelector(".action-share-icon");
    var done = document.querySelector(".action-share-done");
    icon.style.display = "none";
    done.style.display = "block";
    setTimeout(function () {
      icon.style.display = "block";
      done.style.display = "none";
    }, 1000);
  });
});
    </script>
    <script async src="https://www.numworks.com/simulator/update.js"></script>
  </body>
</html>
