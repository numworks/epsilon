/* Use:
 *
 *  var options = {
      canvas: ...,
      env: {
        VARIABLE_NAME: VARIABLE_VALUE
      }
    };
    Epsilon(options);
 *
 * Cf https://github.com/emscripten-core/emscripten/issues/9827
 * */

Module['preRun'] = function () {
  if (Module['env'] && typeof Module['env'] === 'object') {
    for (var key in Module['env']) {
      if (Module['env'].hasOwnProperty(key)) ENV[key] = Module['env'][key];
    }
  }
  /* When figuring up in which DOM element to draw, SDL2/emscripten looks for
   * an element with an id of "canvas". That's pretty annoying because it pretty
   * much prevents us from running multiple instances of Epsilon in a single web
   * page even if Epsilon is built using MODULARIZE=1.
   * Enters specialHTMLTargets. This is a nifty hack that allows one to override
   * DOM node lookups made by Emscripten! Using this feature, we can instruct
   * emscripten to use a custom element when looking for "#canvas".
   * See https://github.com/emscripten-core/emscripten/pull/10659 */
  specialHTMLTargets['#canvas'] = Module.canvas;
};
