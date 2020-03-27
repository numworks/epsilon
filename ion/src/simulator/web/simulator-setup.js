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

  document.querySelectorAll('#keyboard span').forEach(function(span){
    function eventHandler(keyHandler) {
      return function(ev) {
        var key = this.getAttribute('data-key');
        keyHandler(key);
        /* Always prevent default action of event.
         * First, this will prevent the browser from delaying that event. Indeed
         * the browser would otherwise try to see if that event could have any
         * other meaning (e.g. a click) and might delay it as a result.
         * Second, this prevents touch events to be handled twice. Indeed, for
         * backward compatibility reasons, mobile browsers usually create a fake
         * mouse event after each real touch event. This allows desktop websites
         * to work unmodified on mobile devices. But here we are explicitly
         * handling both touch and mouse events. We therefore need to disable
         * the default action of touch events, otherwise the handler would get
         * called twice. */
        ev.preventDefault();
      };
    }
    /* We decide to hook both to touch and mouse events
     * On most mobile browsers, mouse events are generated if addition to touch
     * events, so this could seem pointless. But those mouse events are not
     * generated in real time: instead, they are buffered and eventually fired
     * in a very rapid sequence. This prevents Epsilon from generating an event
     * since this quick sequence will trigger the debouncer. */
    ['touchstart', 'mousedown'].forEach(function(type){
      span.addEventListener(type, eventHandler(Module._IonSimulatorKeyboardKeyDown));
    });
    ['touchend', 'mouseup'].forEach(function(type){
      span.addEventListener(type, eventHandler(Module._IonSimulatorKeyboardKeyUp));
    });
  });
}());

function screenshot() {
  // toDataURL needs the canvas to be refreshed
  Module._IonDisplayForceRefresh();

  var canvas = document.getElementById('canvas');
  var link = document.createElement('a');
  link.download = 'screenshot.png';
  link.href = canvas.toDataURL('image/png').replace('image/png', 'image/octet-stream');
  link.click();
}
