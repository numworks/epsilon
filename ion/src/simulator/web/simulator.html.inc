<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>NumWorks graphing calculator</title>
    <style>
#include "calculator.css"
#include "simulator.css"
    </style>
  </head>
  <body>
    <div class="row">
      <div class="col-calculator">
        <div class="calculator-container">
          <picture>
            <source srcset="background.webp" type="image/webp">
            <source srcset="background.jpg" type="image/jpeg">
            <img src="background.jpg" alt="NumWorks Calculator">
          </picture>
#include "calculator.html"
          <div class="actions">
            <a id="action-fullscreen" class="action">
              <svg viewBox="0 0 16 16" version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink"><g stroke="none" stroke-width="1" fill="none" fill-rule="evenodd"><g fill="#434343"><path d="M5.075,6.95 L6.918,5.088 L3.938,2.062 L5.955,0.018 L0.052,0.018 L0.052,6.004 L2.098,3.928 L5.075,6.95 Z" class="si-glyph-fill"></path><path d="M16.0034788,9.916 L13.832,12.013 L10.799,8.96 L8.918,10.841 L11.957,13.897 L9.961,15.9813842 L16.0034788,15.9813842 L16.0034788,9.916 Z"></path></g></g></svg>
            </a>
            <a id="action-screenshot" class="action">
              <svg viewBox="0 1.5 17 17" version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink"><title>871</title><defs></defs><g stroke="none" stroke-width="1" fill="none" fill-rule="evenodd"><g transform="translate(0.995000, 2.980000)" fill="#434343"><circle cx="7.958" cy="6.958" r="2.958" class="si-glyph-fill"></circle><path d="M14.666,2.042 L10.953,2.042 L9.937,0.031 L6,0.031 L5,2.042 L1.333,2.042 C0.597,2.042 0,2.639 0,3.375 L0,10.625 C0,11.361 0.597,11.959 1.333,11.959 L14.666,11.959 C15.402,11.959 16,11.361 16,10.625 L16,3.375 C16,2.639 15.402,2.042 14.666,2.042 L14.666,2.042 Z M6.953,0.969 L9.047,0.969 L9.047,2.031 L6.953,2.031 L6.953,0.969 L6.953,0.969 Z M8.002,11.033 C5.764,11.033 3.947,9.221 3.947,6.985 C3.947,4.749 5.763,2.937 8.002,2.937 C10.242,2.937 12.057,4.749 12.057,6.985 C12.057,9.221 10.242,11.033 8.002,11.033 L8.002,11.033 Z M14,4.031 L11.953,4.031 L11.953,2.969 L14,2.969 L14,4.031 L14,4.031 Z" class="si-glyph-fill"></path></g></g></svg>
            </a>
          </div>
        </div>
      </div>
      <div class="col-fullscreen">
        <canvas class="calculator-mirror" width="320" height="240"></canvas>
      </div>
    </div>
    <script src="epsilon.js"></script>
    <script>
#include "calculator.js"
var Module = {};
Calculator(Module);
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
  Module.takeScreenshot(e);
});
    </script>
    <script async src="https://www.numworks.com/simulator/update.js"></script>
  </body>
</html>
