#include "script_store.h"

namespace Code {

constexpr char ScriptStore::k_scriptExtension[];

bool ScriptStore::ScriptNameIsFree(const char* baseName) {
  return ScriptBaseNamed(baseName).isNull();
}

void ScriptStore::InitTemplates() {
  AddScriptFromTemplate(ScriptTemplate::Squares());
  AddScriptFromTemplate(ScriptTemplate::Parabola());
  AddScriptFromTemplate(ScriptTemplate::Mandelbrot());
  AddScriptFromTemplate(ScriptTemplate::Polynomial());
}

void ScriptStore::DeleteAllScripts() {
  for (int i = NumberOfScripts() - 1; i >= 0; i--) {
    ScriptAtIndex(i).destroy();
  }
}

bool ScriptStore::IsFull() {
  return Ion::Storage::FileSystem::sharedFileSystem->availableSize() <
         k_fullFreeSpaceSizeLimit;
}

const char* ScriptStore::contentOfScript(const char* name,
                                         bool markAsFetched) const {
  Script script = ScriptNamed(name);
  if (script.isNull()) {
    return nullptr;
  }
  if (markAsFetched) {
    script.setFetchedFromConsole(true);
  }
  return script.content();
}

void ScriptStore::ClearVariableBoxFetchInformation() {
  // TODO optimize fetches
  const int scriptsCount = NumberOfScripts();
  for (int i = 0; i < scriptsCount; i++) {
    ScriptAtIndex(i).setFetchedForVariableBox(false);
  }
}

void ScriptStore::ClearConsoleFetchInformation() {
  // TODO optimize fetches
  const int scriptsCount = NumberOfScripts();
  for (int i = 0; i < scriptsCount; i++) {
    ScriptAtIndex(i).setFetchedFromConsole(false);
  }
}

}  // namespace Code
