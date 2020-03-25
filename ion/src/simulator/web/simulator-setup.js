/* To use this file you should adhere to the following conventions:
 *  - Main canvas has id #canvas
 *  - Secondary canvas (fullscreen on the side) has id #secondary-canvas
 *  - Calculator keyboard has id #keyboard */

var Module;

(function () {
  var mainCanvas = document.getElementById('canvas');
  var secondaryCanvasContext = document.getElementById('secondary-canvas').getContext('2d');
  var epsilonLanguage = document.documentElement.lang || window.navigator.language.split('-')[0];
  Module = {
    canvas: mainCanvas,
    arguments: ['--language', epsilonLanguage],
    onDisplayRefresh: function() {
      secondaryCanvasContext.drawImage(mainCanvas, 0, 0);
    }
  }

  Epsilon(Module);

  var spans = document.querySelectorAll('#keyboard span');
  for (var i=0; i< spans.length; i++) {
    var span = spans[i];
    if (window.PointerEvent) {
      span.addEventListener('pointerdown', function(e) {
        Module._IonSimulatorKeyboardKeyDown(this.getAttribute('data-key'));
      });
      span.addEventListener('pointerup', function(e) {
        Module._IonSimulatorKeyboardKeyUp(this.getAttribute('data-key'));
      });
    } else {
      // If pointer events are not supported
      span.addEventListener('mousedown', function(e) {
        Module._IonSimulatorKeyboardKeyDown(this.getAttribute('data-key'));
      });
      span.addEventListener('mouseup', function(e) {
        Module._IonSimulatorKeyboardKeyUp(this.getAttribute('data-key'));
      });
    }
  }
}());

function screenshot() {
  // toDataURL needs the canvas to be refreshed
  Module._IonDisplayForceRefresh();

  var canvas = document.getElementById('canvas');
  var link = document.createElement('a');
  link.download = 'screenshot.png';
  link.href = canvas.toDataURL('image/png').replace('image/png', 'image/octet-stream');
  // TODO: Fixme???
  /* The following used to be "link.click()" but this doesn't work on Firefox.
   * See https://stackoverflow.com/questions/32225904/programmatical-click-on-a-tag-not-working-in-firefox */
  //  link.dispatchEvent(new MouseEvent('click', {bubbles: true, cancelable: true, view: window}));
  link.click();
}
