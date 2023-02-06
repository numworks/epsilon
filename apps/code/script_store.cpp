#include "script_store.h"

namespace Code {

constexpr char ScriptStore::k_scriptExtension[];

bool ScriptStore::ScriptNameIsFree(const char * baseName) {
  return ScriptBaseNamed(baseName).isNull();
}

ScriptStore::ScriptStore() {
  addScriptFromTemplate(ScriptTemplate::Squares());
  addScriptFromTemplate(ScriptTemplate::Parabola());
  addScriptFromTemplate(ScriptTemplate::Mandelbrot());
  addScriptFromTemplate(ScriptTemplate::Polynomial());
}

void ScriptStore::deleteAllScripts() {
  for (int i = numberOfScripts() - 1; i >= 0; i--) {
    scriptAtIndex(i).destroy();
  }
}

bool ScriptStore::isFull() {
  return Ion::Storage::FileSystem::sharedFileSystem->availableSize() < k_fullFreeSpaceSizeLimit;
}

const char * ScriptStore::contentOfScript(const char * name, bool markAsFetched) {
  Script script = ScriptNamed(name);
  if (script.isNull()) {
    return nullptr;
  }
  if (markAsFetched) {
    script.setFetchedFromConsole(true);
  }
  return script.content();
}

void ScriptStore::clearVariableBoxFetchInformation() {
  // TODO optimize fetches
  const int scriptsCount = numberOfScripts();
  for (int i = 0; i < scriptsCount; i++) {
    scriptAtIndex(i).setFetchedForVariableBox(false);
  }
}

void ScriptStore::clearConsoleFetchInformation() {
  // TODO optimize fetches
  const int scriptsCount = numberOfScripts();
  for (int i = 0; i < scriptsCount; i++) {
    scriptAtIndex(i).setFetchedFromConsole(false);
  }
}

}
