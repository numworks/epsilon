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

Module["preRun"] = function () {
  if (Module['env'] && typeof Module['env'] === 'object') {
    for (var key in Module['env']) {
      if (Module['env'].hasOwnProperty(key))
        ENV[key] = Module['env'][key];
    }
  }
}
